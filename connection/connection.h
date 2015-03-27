/*
 * connection.h
 *
 *  Created on: Mar 25, 2015
 *      Author: jzhao
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <pthread.h>
#include "connectionconstants.h"

class ConnectionStatusInterface;
class DataReceiverInterface;

class Connection
{
public:
	Connection(void);
	virtual ~Connection(void);

public:
	int Initialize(void);

	void SetConnectionStatusInterface(ConnectionStatusInterface* pConnectionStatusInterface);
	void SetDataReceiverInterface(DataReceiverInterface* pDataReceiverInterface);

	int StartListen(int nPort);
	int StartConnect(const char* pAddress, int nPort);

	int Close(void);

	int SendMessage(const char* pData, int nSize);
	ConnectionStatus GetStatus(void) const;

protected:
	static void* ListenThreadFunc(void* pParam);
	static void* ReceiveThreadFunc(void* pParam);

protected:
	void SetStatus(ConnectionStatus nNewStatus);
	int RecvFunc(void);

protected:
	ConnectionStatusInterface*		m_pConnectionStatusInterface;
	DataReceiverInterface*			m_pDataReceiverInterface;
	pthread_t						m_WorkThreadId;
	ConnectionStatus				m_nConnectionStatus;
	int								m_nPort;
	int								m_listenSocket;
	int								m_socket;
};

#endif /* CONNECTION_H_ */
