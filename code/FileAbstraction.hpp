#ifndef _FILE_ABSTRACTION_HPP_
#define _FILE_ABSTRACTION_HPP_

#if defined(_WIN32)
//Windows

#include <Windows.h>

#else
//Unix

#endif

#include <iostream>

class File
{
public:
	struct ModifiedDate
	{
		unsigned int day;
		unsigned int month;
		unsigned int year;
		unsigned int hour;
		unsigned int minute;
		unsigned int second;
		unsigned int milliseconds;
	};

	File(const std::string &path);
	~File();
	bool IsOpem() const {return m_isOpen;} 
	bool IsValid() const {return m_isValid;}
	bool IsModified();
	ModifiedDate GetModifiedTime();
private:
	bool m_isOpen;
	bool m_isValid;
	#if defined(_WIN32)
		HANDLE m_file;
		FILETIME m_createTime;
		FILETIME m_lastAccessTime;
		FILETIME m_writeTime;
		SYSTEMTIME m_writeTime_system;
	#else
		int m_file;
	#endif
	std::string m_path;
};

#endif //_FILE_ABSTRACTION_HPP_