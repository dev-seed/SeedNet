#pragma once
#define _LOCK(LOCK)		if( SeedLocker _LOCKER=LOCK ){}else

class SeedLock
{
public :
	SeedLock();
	virtual ~SeedLock();

	void	Lock();
	void	Unlock();

protected:
	CRITICAL_SECTION	m_CriticalSection;
};

class SeedLocker
{
public :
	SeedLocker(SeedLock &seedLock);
	virtual ~SeedLocker();

	operator bool()
	{
		return false;
	}

protected:
	SeedLock			*m_pSeedLock;
};