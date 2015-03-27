/*
 * debugoutput.h
 *
 *  Created on: Mar 16, 2015
 *      Author: jzhao
 */

#ifndef DEBUGOUTPUT_H_
#define DEBUGOUTPUT_H_

class DebugOutput
{
public:
	DebugOutput(void);
	~DebugOutput(void);

public:
	enum DebugLevel
	{
		DebugLevel_Debug = 0,
		DebugLevel_Info,
		DebugLevel_Warning,
		DebugLevel_Error,
		DebugLevel_Critical,
		DebugLevel_None,
	};

public:
	static void DebugCritical(const char *fmt, ...);
	static void DebugError(const char *fmt, ...);
	static void DebugWarning(const char *fmt, ...);
	static void DebugInfo(const char *fmt, ...);
	static void DebugDebug(const char *fmt, ...);
	static void DebugBinary(const char* pBuf, int nSize);

private:
	static DebugOutput& Instance(void);

private:
	void SetDebugLevel(DebugLevel nLevel);
	void OutputDebug(const char* pType, const char* pContent);

private:
	char*			m_pBuffer;
	DebugLevel		m_nLevel;
};

#endif /* DEBUGOUTPUT_H_ */
