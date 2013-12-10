#include "stdafx.h"

SeedLog::SeedLog( LogType logType /*= Managed*/, std::wstring fileName /*= L"DefaultLog"*/, std::wstring directoryName /*= L"DefaultLog"*/ )
	: m_LogType(logType),
	m_LogFileName(fileName),
	m_LogDirectoryName(directoryName)
{
	std::wstring tempDirectoryName = L"./FileLog";
	if (!m_LogDirectoryName.compare(L"DefaultLog"))
	{
		CreateDirectoryW(tempDirectoryName.c_str(), NULL);
	}
	else
	{
		tempDirectoryName += L"/";
		tempDirectoryName += m_LogDirectoryName;
		if (CreateDirectoryW(tempDirectoryName.c_str(), NULL) == FALSE)
		{
			DWORD lastError = GetLastError();
			if (lastError == ERROR_PATH_NOT_FOUND)
			{
				CreateDirectoryW(L"./FileLog", NULL);
			}
			else
			{
				std::wcout<<L"SeedLog()::CreateDirectoryW Failed with "<<lastError<<std::endl;
				return;
			}
		}
	}
	tempDirectoryName += L"/";
	tempDirectoryName += m_LogFileName;
	m_LogFileName = tempDirectoryName;

	ZeroMemory(&m_LogCreatedDate, sizeof(tm));

	if (Managed())
	{
		if(!SeedLogger::Instance().Manage(this))
		{
			m_LogType = LogType::Unmanaged;
		}
	}

	reference++;
}
SeedLog::~SeedLog()
{
	if (Managed())
	{
		SeedLogger::Instance().Unmanage(this);
	}
	ClearQueue(m_LogQueue);
	Close();

	reference--;
}

bool SeedLog::Create()
{
	time_t	current;
	tm		currentTm;

	time(&current);
	localtime_s(&currentTm, &current);

	if (m_LogCreatedDate.tm_year != currentTm.tm_year
	||	m_LogCreatedDate.tm_mon	 != currentTm.tm_mon
	||	m_LogCreatedDate.tm_mday != currentTm.tm_mday
		)
	{
		WCHAR	date[LENGTH_DEFAULTTINYSTUFF]={0,};
		WCHAR	dateFileName[LENGTH_DEFAULTFILENAME]={0,};

		Close();

		wcsftime(date, LENGTH_DEFAULTTINYSTUFF, L"%y%m%d%H", &currentTm);
		swprintf_s(dateFileName, LENGTH_DEFAULTFILENAME, L"%s_%s.log", m_LogFileName.c_str(), date);

		m_LogFileStream.open(dateFileName, std::ios::out | std::ios::app);
		if (m_LogFileStream.bad())
		{
			return false;
		}

		CopyMemory(&m_LogCreatedDate, &currentTm, sizeof(tm));
	}

	return true;
}

void SeedLog::Close()
{
	if (m_LogFileStream.is_open())
	{
		m_LogFileStream.flush();
		m_LogFileStream.close();
	}
}

void SeedLog::Write( std::wstring statement )
{
	WCHAR	date[LENGTH_DEFAULTTINYSTUFF]={0,};
	WCHAR	time[LENGTH_DEFAULTTINYSTUFF]={0,};

	_wstrdate_s(date);
	_wstrtime_s(time);
	std::wstring logStatement(date);
	logStatement += L"  ";
	logStatement += time;
	logStatement += L"\t";
	logStatement += statement;

	if (Managed())
	{
		_LOCK(m_Lock)
		{
			m_LogQueue.push(logStatement);
		}
	}
	else
	{
		if (Create())
		{
			m_LogFileStream.write(logStatement.c_str(), logStatement.length());
			m_LogFileStream.flush();
		}
	}
}

void SeedLog::Flush()
{
	LogQueue	flushQueue;
	_LOCK(m_Lock)
	{
		if (!Empty())
		{
			std::swap(flushQueue, m_LogQueue);
		}
	}
	ClearQueue(flushQueue);
}

void SeedLog::ClearQueue( LogQueue &logQueue )
{
	while (!logQueue.empty())
	{
		std::wstring statement = logQueue.front();
		logQueue.pop();

		if(Create())
		{
			m_LogFileStream.write(statement.c_str(), statement.length());
			m_LogFileStream.flush();
		}
	}
}

SeedLogger::~SeedLogger()
{
	Close();
}

bool SeedLogger::Create()
{
	m_hEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!m_hEnd)
	{
		return false;
	}

	if (!Start(Run, this))
	{
		return false;
	}
	return true;
}

void SeedLogger::Close()
{
	_LOCK(m_Lock)
	{
		m_LogList.clear();
	}

	if (m_ThreadID)
	{
		SetEvent(m_hEnd);
		Wait(INFINITE);
		Finish();

		CloseHandle(m_hEnd);
	}
}

bool SeedLogger::Manage( SeedLog* pSeedLog )
{
	if (pSeedLog != NULL)
	{
		_LOCK(m_Lock)
		{
			m_LogList.push_back(pSeedLog);
		}
		return true;
	}
	else
	{
		return false;
	}
}

void SeedLogger::Unmanage( SeedLog* pSeedLog )
{
	if (pSeedLog != NULL)
	{
		_LOCK(m_Lock)
		{
			m_LogList.remove(pSeedLog);
		}
	}
}

SeedLogger	& SeedLogger::Instance()
{
	static SeedLogger _instance;
	return _instance;
}

unsigned __stdcall SeedLogger::Run( void *pArg )
{
	((SeedLogger*)pArg)->Process();

	return 0;
}

bool SeedLogger::Process()
{
	while (true)
	{
		if (WaitForSingleObject(m_hEnd, CYCLE_DEFAULT) == WAIT_OBJECT_0)
		{
			break;
		}
		m_LastProcessedTime = timeGetTime();

		_LOCK(m_Lock)
		{
			if (!m_LogList.empty())
			{
				for_each(m_LogList.begin(), m_LogList.end(), [](SeedLog* pSeedLog){ pSeedLog->Flush(); });
			}
		}
	}

	return true;
}

SeedLogger::SeedLogger()
	: m_LastProcessedTime(0),
	m_hEnd(NULL)
{
	Create();
}