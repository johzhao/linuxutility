/*
 * debugoutput.cpp
 *
 *  Created on: Mar 16, 2015
 *      Author: jzhao
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "debugoutput.h"

#define DEBUGOUTPUT

const int BufferSize = 8192;

DebugOutput::DebugOutput(void)
{
	m_pBuffer = new char[BufferSize];
	SetDebugLevel(DebugLevel_None);
}

DebugOutput::~DebugOutput(void)
{
	delete [] m_pBuffer;
}

void DebugOutput::DebugCritical(const char *fmt, ...)
{
	DebugOutput& self = DebugOutput::Instance();
	if (self.m_nLevel <= DebugLevel_Critical)
	{
		memset(self.m_pBuffer, 0, BufferSize);

		va_list argumentList;
		va_start(argumentList, fmt);
		vsnprintf(self.m_pBuffer, BufferSize-1, fmt, argumentList);
		va_end(argumentList);

		self.OutputDebug("Critical", self.m_pBuffer);
	}
}

void DebugOutput::DebugError(const char *fmt, ...)
{
	DebugOutput& self = DebugOutput::Instance();
	if (self.m_nLevel <= DebugLevel_Error)
	{
		memset(self.m_pBuffer, 0, BufferSize);

		va_list argumentList;
		va_start(argumentList, fmt);
		vsnprintf(self.m_pBuffer, BufferSize-1, fmt, argumentList);
		va_end(argumentList);

		self.OutputDebug("Error", self.m_pBuffer);
	}
}

void DebugOutput::DebugWarning(const char *fmt, ...)
{
	DebugOutput& self = DebugOutput::Instance();
	if (self.m_nLevel <= DebugLevel_Warning)
	{
		memset(self.m_pBuffer, 0, BufferSize);

		va_list argumentList;
		va_start(argumentList, fmt);
		vsnprintf(self.m_pBuffer, BufferSize-1, fmt, argumentList);
		va_end(argumentList);

		self.OutputDebug("Warning", self.m_pBuffer);
	}
}

void DebugOutput::DebugInfo(const char *fmt, ...)
{
	DebugOutput& self = DebugOutput::Instance();
	if (self.m_nLevel <= DebugLevel_Info)
	{
		memset(self.m_pBuffer, 0, BufferSize);

		va_list argumentList;
		va_start(argumentList, fmt);
		vsnprintf(self.m_pBuffer, BufferSize-1, fmt, argumentList);
		va_end(argumentList);

		self.OutputDebug("Info", self.m_pBuffer);
	}
}

void DebugOutput::DebugDebug(const char *fmt, ...)
{
	DebugOutput& self = DebugOutput::Instance();
	if (self.m_nLevel <= DebugLevel_Debug)
	{
		memset(self.m_pBuffer, 0, BufferSize);

		va_list argumentList;
		va_start(argumentList, fmt);
		vsnprintf(self.m_pBuffer, BufferSize-1, fmt, argumentList);
		va_end(argumentList);

		self.OutputDebug("Debug", self.m_pBuffer);
	}
}

const int SingleLineLength = 16;

int OutputBinaryLine(const char* pSource, int nLength, char* pDestinate)
{
    int nOffset = 0;
    char* pBuffer = pDestinate;
    for (int i=0; i<nLength; ++i)
    {
        unsigned char c = *(pSource + i);
        sprintf(pBuffer + nOffset, "%02X ", c);
        nOffset += 3;
    }

    if (nLength < SingleLineLength)
    {
        int nDiff = (SingleLineLength - nLength) * 3;
        sprintf(pBuffer + nOffset, "%*s", nDiff, " ");
        nOffset += nDiff;
    }

    sprintf(pBuffer + nOffset, "\t");
    nOffset += 1;

    for (int i=0; i<nLength; ++i)
    {
        char c = *(pSource + i);
    	if (c == '\0' || c == '\r' || c == '\n' || c == '\t' || c == '\v' || c == '\f' || c == '\b')
    	{
    		// Display space for control characters
			sprintf(pBuffer + nOffset, ".");
    	}
    	else
    	{
			sprintf(pBuffer + nOffset, "%c", c);
    	}
        nOffset += 1;
    }

    sprintf(pBuffer + nOffset, "\r\n");
    nOffset += 2;

    return nOffset;
}

void DebugOutput::DebugBinary(const char* pBuf, int nSize)
{
	DebugOutput& self = DebugOutput::Instance();
	if (self.m_nLevel <= DebugLevel_Debug)
	{
		int nLength = (nSize / SingleLineLength + 2) * 67;
		char* outputBuffer = new char[nLength];
		memset(outputBuffer, nLength, 0);
		char* buf = outputBuffer;
		for (int i=0; i<nSize; i+=SingleLineLength)
		{
			int lineLength = SingleLineLength;
			if (i + lineLength > nSize)
			{
				lineLength = nSize - i;
			}
			buf += OutputBinaryLine(pBuf + i, lineLength, buf);
		}

		fprintf(stderr, "[Binary Begin]\r\n%s", outputBuffer);
		fprintf(stderr, "[Binary End]\r\n");
		delete [] outputBuffer;
	}
}

DebugOutput& DebugOutput::Instance(void)
{
	static DebugOutput self;

#ifdef DEBUGOUTPUT
	self.SetDebugLevel(DebugLevel_Debug);
#else
	self.SetDebugLevel(DebugLevel_None);
#endif

	return self;
}

void DebugOutput::SetDebugLevel(DebugLevel nLevel)
{
	m_nLevel = nLevel;
}

void DebugOutput::OutputDebug(const char* pType, const char* pContent)
{
	fprintf(stderr, "[%s]\t%s", pType, pContent);
	fprintf(stderr, "\r\n");
}
