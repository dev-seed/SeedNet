#pragma once

class SeedThread
{
public :
	SeedThread();
	virtual ~SeedThread();

	bool	Start(unsigned (__stdcall* pFunc)(void*), void* pArg);
	void	Finish();
	DWORD	Wait(DWORD Timeout);

protected :
	HANDLE			m_hThread;
	unsigned		m_ThreadID;
};