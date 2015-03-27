/*
 * connection.cpp
 *
 *  Created on: Mar 25, 2015
 *      Author: jzhao
 */

#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <atomic.h>
#include "connection.h"
#include "debugoutput.h"
#include "connectionstatusinterface.h"
#include "datareceiverinterface.h"

Connection::Connection(void)
{
	m_pConnectionStatusInterface = NULL;
	m_pDataReceiverInterface = NULL;
	m_WorkThreadId = 0;
	SetStatus(CS_Disconnect);
	m_nPort = 0;
	m_listenSocket = 0;
	m_socket = 0;
}

Connection::~Connection(void)
{
	Close();
}

int Connection::Initialize(void)
{
	return 0;
}

void Connection::SetConnectionStatusInterface(ConnectionStatusInterface* pConnectionStatusInterface)
{
	m_pConnectionStatusInterface = pConnectionStatusInterface;
}

void Connection::SetDataReceiverInterface(DataReceiverInterface* pDataReceiverInterface)
{
	m_pDataReceiverInterface = pDataReceiverInterface;
}

int Connection::StartListen(int nPort)
{
	assert(GetStatus() == CS_Disconnect && m_WorkThreadId == 0);

	m_nPort = nPort;

	pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_create(&m_WorkThreadId, &attr, ListenThreadFunc, this);

	return -1;
}

int Connection::StartConnect(const char* pAddress, int nPort)
{
	assert(pAddress != NULL && GetStatus() == CS_Disconnect && m_WorkThreadId == 0);

	sockaddr_in srvaddr;
	memset(&srvaddr, 0, sizeof(srvaddr));
	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(nPort);
	srvaddr.sin_addr.s_addr = inet_addr(pAddress);

	int result = connect(m_socket, (sockaddr*)&srvaddr, sizeof(sockaddr));
	if (result == -1)
	{
		DebugOutput::DebugError("Connect to %s:%d failed with error: %d.", pAddress, nPort, errno);
		result = errno;
	}

	SetStatus(CS_Connected);

	pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_create(&m_WorkThreadId, &attr, ReceiveThreadFunc, this);

	return result;
}

int Connection::Close(void)
{
	if (m_listenSocket != 0)
	{
		close(m_listenSocket);
		m_listenSocket = 0;
	}

	if (m_socket != 0)
	{
		close(m_socket);
		m_socket = 0;
	}

	if (m_WorkThreadId > 0)
	{
		pthread_join(m_WorkThreadId, NULL);
		m_WorkThreadId = 0;
	}

	SetStatus(CS_Disconnect);

	return 0;
}

int Connection::SendMessage(const char* pData, int nSize)
{
	assert(GetStatus() == CS_Connected);

	int result = -1;
	ssize_t ret = send(m_socket, pData, nSize, 0);
	if (ret == -1)
	{
		DebugOutput::DebugError("Send failed with error: %d.", errno);
		result = errno;
	}
	else if (ret != nSize)
	{
		result = -1;
	}
	else
	{
		result = 0;
	}
	return result;
}

ConnectionStatus Connection::GetStatus(void) const
{
	return m_nConnectionStatus;
}

void* Connection::ListenThreadFunc(void* pParam)
{
	Connection* self = static_cast<Connection*>(pParam);

	self->m_listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (self->m_listenSocket < 0)
	{
		DebugOutput::DebugError("Create listen socket failed with error: %d.", errno);
		return NULL;
	}

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(self->m_nPort);
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	int ret = bind(self->m_listenSocket, (struct sockaddr*)&saddr, sizeof(saddr));
	if (ret < 0)
	{
		DebugOutput::DebugError("Socket bind to %d port failed with error: %d.", self->m_nPort, errno);
		return NULL;
	}

	ret = listen(self->m_listenSocket, 1);
	if (ret < 0)
	{
		DebugOutput::DebugError("Socket listen failed with error: %d.", errno);
		return NULL;
	}

	self->SetStatus(CS_Listening);

	struct sockaddr remoteAddr;
	size_t addLength = sizeof(remoteAddr);
	self->m_socket = accept(self->m_listenSocket, &remoteAddr, &addLength);
	if (self->m_socket < 0)
	{
		DebugOutput::DebugError("Socket accept failed with error: %d.", errno);
		self->SetStatus(CS_Disconnect);
		return NULL;
	}

	{
		// Get remote IP address
		sockaddr_in* pAddr = (sockaddr_in*)(&remoteAddr);
		DebugOutput::DebugDebug("Connected with remote address: %s", inet_ntoa(pAddr->sin_addr));
	}

	self->SetStatus(CS_Connected);

	// Use this thread to receive data.
	self->RecvFunc();

	return NULL;
}

void* Connection::ReceiveThreadFunc(void* pParam)
{
	Connection* self = static_cast<Connection*>(pParam);
	self->RecvFunc();
	return NULL;
}

void Connection::SetStatus(ConnectionStatus nNewStatus)
{
	m_nConnectionStatus = nNewStatus;

	if (m_pConnectionStatusInterface != NULL)
	{
		// TODO: This may not in work thread.
		m_pConnectionStatusInterface->OnConnectionStatusChanged(GetStatus());
	}
}

int Connection::RecvFunc(void)
{
	const int RECV_BUF_SIZE = 4096;
    char* pBuf = new char[RECV_BUF_SIZE + 1];

	while (true)
	{
		memset(pBuf, 0, RECV_BUF_SIZE+1);
		ssize_t ret = recv(m_socket, pBuf, RECV_BUF_SIZE, 0);
		if (ret == -1)
		{
			DebugOutput::DebugError("Receive from socket meet error: %d", errno);
			break;
		}
		else if (ret == 0)
		{
			// Connection closed.
			DebugOutput::DebugDebug("Receive meet connection close.");
			break;
		}
		else
		{
			if (m_pDataReceiverInterface != NULL)
			{
				// TODO: This was not in work thread.
				m_pDataReceiverInterface->OnDataReceived(pBuf, ret);
			}
		}
	}
	return 0;
}
