/*
 * taskqueue.cpp
 *
 *  Created on: Mar 25, 2015
 *      Author: jzhao
 */

#include <iostream>
#include "taskqueue.h"
#include "task.h"

const int EndTaskQueueCommand = -1;

TaskQueue::TaskQueue(void)
{
}

TaskQueue::~TaskQueue(void)
{
	pthread_mutex_destroy(&m_queueMutex);
	pthread_cond_destroy(&m_threadCond);
}

int TaskQueue::Initialize(void)
{
	int result = 0;
	result = (result == 0) ? pthread_mutex_init(&m_queueMutex, NULL) : result;
	result = (result == 0) ? pthread_cond_init(&m_threadCond, NULL) : result;

	return result;
}

void TaskQueue::ExecuteQueue(void)
{
	while (true)
	{
		pthread_mutex_lock(&m_queueMutex);
		if (m_taskQueue.size() == 0)
		{
			pthread_cond_wait(&m_threadCond, &m_queueMutex);
		}

		const Task* pTask = m_taskQueue.front();
		m_taskQueue.pop();
		delete pTask;

		pthread_mutex_unlock(&m_queueMutex);
	}
}

int TaskQueue::AddTask(const Task* pTask)
{
	pthread_mutex_lock(&m_queueMutex);
	m_taskQueue.push(pTask);
	if (m_taskQueue.size() == 1)
	{
		pthread_cond_signal(&m_threadCond);
	}
	pthread_mutex_unlock(&m_queueMutex);

	return 0;
}

void TaskQueue::Stop(void)
{
	// TODO:
}
