/*
 * taskqueue.h
 *
 *  Created on: Mar 25, 2015
 *      Author: jzhao
 */

#ifndef TASKQUEUE_H_
#define TASKQUEUE_H_

#include <pthread.h>
#include <queue>

class Task;

class TaskQueue
{
public:
	TaskQueue(void);
	virtual ~TaskQueue(void);

public:
	int Initialize(void);
	void ExecuteQueue(void);
	int AddTask(const Task* pTask);
	void Stop(void);

private:
	pthread_cond_t				m_threadCond;
	pthread_mutex_t				m_queueMutex;
	std::queue<const Task*>		m_taskQueue;
};

#endif /* TASKQUEUE_H_ */
