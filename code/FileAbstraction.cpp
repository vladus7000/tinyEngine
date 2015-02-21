#include "FileAbstraction.hpp"

File::File(const std::string &path)
	: m_isOpen(false)
	, m_isValid(false)
	, m_file(0)
	, m_path(path)
{
	HANDLE m_file = CreateFile(m_path.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (m_file)
	{
		m_isValid = true;
		BOOL ok = GetFileTime(m_file, &m_createTime, &m_lastAccessTime, &m_writeTime); 
		CloseHandle(m_file);
		SYSTEMTIME m_writeTime_system;
		FileTimeToSystemTime( &m_writeTime, &m_writeTime_system );
	}
}

File::~File()
{
	if (m_isOpen && m_isValid)
	{
		#if defined(_WIN32)
		CloseHandle(m_file);
		#else
		//unix 
		#endif
	}
}

bool File::IsModified()
{
	HANDLE hFile = CreateFile(m_path.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile)
	{
		FILETIME m_newWriteTime;
		BOOL ok = GetFileTime(hFile, &m_createTime, &m_lastAccessTime, &m_newWriteTime);
		CloseHandle(hFile);
		if (CompareFileTime(&m_newWriteTime, &m_writeTime) > 0)
		{
			m_writeTime = m_newWriteTime;
			return true;
		}
	}
	else
	{
		m_isValid = false;
		m_isOpen = false;
	}
	return false;
}

File::ModifiedDate File::GetModifiedTime()
{
	if (m_isValid)
	{
		File::ModifiedDate res;
		res.day = m_writeTime_system.wDay;
		res.hour = m_writeTime_system.wHour;
		res.milliseconds = m_writeTime_system.wMilliseconds;
		res.minute = m_writeTime_system.wMinute;
		res.month = m_writeTime_system.wMonth;
		res.second = m_writeTime_system.wSecond;
		res.year = m_writeTime_system.wYear;
		return res;
	}
}