#include "stdafx.h"

SeedThread::SeedThread()
	: m_hThread(NULL),
	m_ThreadID(0)
{

}

SeedThread::~SeedThread()
{

}

bool SeedThread::Start( unsigned (__stdcall* pFunc)(void*), void* pArg )
{
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, pFunc, pArg, 0, &m_ThreadID);
	if (m_hThread == NULL)
	{
		return false;
	}
	return true;
}

void SeedThread::Finish()
{
	if (m_hThread != NULL)
	{
		CloseHandle(m_hThread);	// 여기서 _endthreadex 호출
		m_hThread = NULL;
	}
	m_ThreadID = 0;
}

DWORD SeedThread::Wait( DWORD Timeout )
{
	return WaitForSingleObject(m_hThread, Timeout);
}