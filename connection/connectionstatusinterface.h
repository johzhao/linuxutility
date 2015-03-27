/*
 * connectionstatusinterface.h
 *
 *  Created on: Mar 26, 2015
 *      Author: jzhao
 */

#ifndef CONNECTIONSTATUSINTERFACE_H_
#define CONNECTIONSTATUSINTERFACE_H_

#include "connectionconstants.h"

class ConnectionStatusInterface
{
public:
	virtual ~ConnectionStatusInterface(void) {}

	virtual void OnConnectionStatusChanged(ConnectionStatus nNewState) = 0;
};

#endif /* CONNECTIONSTATUSINTERFACE_H_ */
