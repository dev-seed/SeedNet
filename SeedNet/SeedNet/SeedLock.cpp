#include "stdafx.h"
#include "SeedLock.h"

SeedLock::SeedLock()
{
	InitializeCriticalSection(&m_CriticalSection);
}

SeedLock::~SeedLock()
{
	DeleteCriticalSection(&m_CriticalSection);
}

void SeedLock::Lock()
{
	EnterCriticalSection(&m_CriticalSection);
}

void SeedLock::Unlock()
{
	LeaveCriticalSection(&m_CriticalSection);
}


SeedLocker::SeedLocker( SeedLock &seedLock )
{
	m_pSeedLock = &seedLock;
	m_pSeedLock->Lock();
}

SeedLocker::~SeedLocker()
{
	m_pSeedLock->Unlock();
}