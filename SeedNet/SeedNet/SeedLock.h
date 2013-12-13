#pragma once
#define _LOCK(LOCK)		if( SeedLocker _LOCKER=LOCK ){}else

// Normal Critical Section
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

// Used for #define 
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



// Spin lock
class SeedSpinLock
{
public :
	SeedSpinLock() : m_State(Unlocked) {}
	void lock()
	{
		while (m_State.exchange(Locked, boost::memory_order_acquire) == Locked)
		{
			// busy-wait
		}
	}

	void unlock()
	{
		m_State.store(Unlocked, boost::memory_order_release);
	}

private :
	typedef enum {Locked, Unlocked} LockState;
	boost::atomic<LockState>	m_State;
};
