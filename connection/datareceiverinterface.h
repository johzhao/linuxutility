/*
 * datareceiverinterface.h
 *
 *  Created on: Mar 26, 2015
 *      Author: jzhao
 */

#ifndef DATARECEIVERINTERFACE_H_
#define DATARECEIVERINTERFACE_H_

class DataReceiverInterface
{
public:
	virtual ~DataReceiverInterface(void) {}

	virtual void OnDataReceived(const char* pBuffer, int nSize) = 0;
};

#endif /* DATARECEIVERINTERFACE_H_ */
