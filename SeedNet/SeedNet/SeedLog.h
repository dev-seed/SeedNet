#pragma once

typedef WStringQueue	LogQueue;

class SeedLog
{
public :
	SeedLog(LogType logType = LogType::Managed, std::wstring fileName = std::wstring(L"DefaultLog"), std::wstring directoryName = std::wstring(L"DefaultLog"));
	virtual ~SeedLog();

	bool				Create();
	void				Close();

	void				Write(std::wstring statement);
	void				Flush();
	void				ClearQueue(LogQueue &logQueue);

public :
	inline bool			Empty()
	{
		bool tempEmpty;
		_LOCK(m_Lock)
		{
			tempEmpty =	m_LogQueue.empty();
		}
		return tempEmpty;
	}

	inline bool			Managed()
	{
		return (m_LogType == LogType::Managed);
	}

private :
	static	DWORD		reference;
	LogType				m_LogType;
	std::wofstream		m_LogFileStream;
	std::wstring		m_LogFileName;
	std::wstring		m_LogDirectoryName;
	LogQueue			m_LogQueue;
	SeedLock			m_Lock;
	tm					m_LogCreatedDate;
};

typedef std::list<SeedLog*>	LogList;

class SeedLogger : public SeedThread
{
public :
	virtual ~SeedLogger();

	bool	Create();
	void	Close();
	
	bool	Manage(SeedLog* pSeedLog);
	void	Unmanage(SeedLog* pSeedLog);

public :
	static SeedLogger	&Instance();
	static unsigned __stdcall Run(void *pArg);

	bool	Process();

private :
	DWORD			m_LastProcessedTime;
	HANDLE			m_hEnd;

	LogList			m_LogList;
	SeedLock		m_Lock;
private :
	SeedLogger();
};