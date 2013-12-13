#include "stdafx.h"

SeedThread::SeedThread()
	: m_hThread(nullptr),
	m_ThreadID(0)
{

}

SeedThread::~SeedThread()
{

}

bool SeedThread::Start( unsigned (__stdcall* pFunc)(void*), void* pArg )
{
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, pFunc, pArg, 0, &m_ThreadID);
	if (m_hThread == nullptr)
	{
		return false;
	}
	return true;
}

void SeedThread::Finish()
{
	if (m_hThread != nullptr)
	{
		CloseHandle(m_hThread);	// 여기서 _endthreadex 호출
		m_hThread = nullptr;
	}
	m_ThreadID = 0;
}

DWORD SeedThread::Wait( DWORD Timeout )
{
	return WaitForSingleObject(m_hThread, Timeout);
}