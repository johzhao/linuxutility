/*
 * task.cpp
 *
 *  Created on: Mar 25, 2015
 *      Author: jzhao
 */

#include "task.h"

Task::Task(void)
{
	Initialize();
}

Task::Task(const Task& other)
{
	Initialize();
	*this = other;
}

Task::~Task(void)
{
}

Task& Task::operator=(const Task& other)
{
	// TODO:
	return *this;
}

int Task::Initialize(void)
{
	return 0;
}
