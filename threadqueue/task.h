/*
 * task.h
 *
 *  Created on: Mar 25, 2015
 *      Author: jzhao
 */

#ifndef TASK_H_
#define TASK_H_

class Task
{
public:
	Task(void);
	Task(const Task& other);
	virtual ~Task(void);
	Task& operator=(const Task& other);

private:
	int Initialize(void);
};

#endif /* TASK_H_ */
