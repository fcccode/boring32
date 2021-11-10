module;

#include <string>
#include <stdexcept>
#include <Windows.h>
#include "include/Raii/Win32Handle.hpp"
#include "include/Error/Win32Error.hpp"

module boring32.async.filemapping;

namespace Boring32::Async
{
	FileMapping::~FileMapping()
	{
		Close();
	}

	FileMapping::FileMapping()
		: m_maxSize(0)
	{}

	FileMapping::FileMapping(const bool isInheritable, const size_t maxSize)
		: m_maxSize(maxSize)
	{
		CreateOrOpen(true, PAGE_READWRITE, isInheritable);
	}
	
	FileMapping::FileMapping(
		const bool isInheritable, 
		const std::wstring name, 
		const size_t maxSize
	)
	:	m_name(std::move(name)), 
		m_maxSize(maxSize)
	{
		if (m_name.empty())
			throw std::invalid_argument(__FUNCSIG__": name cannot be an empty string");
		if(m_maxSize == 0)
			throw std::invalid_argument(__FUNCSIG__": maxSize cannot be 0");
		CreateOrOpen(true, PAGE_READWRITE, isInheritable);
	}

	FileMapping::FileMapping(const FileMapping& other) : FileMapping() { Copy(other); }
	FileMapping::FileMapping(FileMapping&& other) noexcept : FileMapping() { Move(other); }

	FileMapping::FileMapping(
		const bool isInheritable, 
		const std::wstring name, 
		const size_t maxSize,
		const DWORD desiredAccess
	)
	:	m_name(std::move(name)),
		m_maxSize(maxSize)
	{
		if (m_name.empty())
			throw std::invalid_argument(__FUNCSIG__": name cannot be an empty string");
		if (m_maxSize == 0)
			throw std::invalid_argument(__FUNCSIG__": maxSize cannot be 0");
		CreateOrOpen(false, desiredAccess, isInheritable);
	}

	FileMapping& FileMapping::operator=(const FileMapping& other) { return Copy(other); }
	FileMapping& FileMapping::operator=(FileMapping&& other) noexcept { return Move(other); }

	void FileMapping::Close()
	{
		m_mapFile = nullptr;
		m_maxSize = 0;
		m_name.clear();
	}

	const std::wstring FileMapping::GetName() const noexcept
	{
		return m_name;
	}

	HANDLE FileMapping::GetNativeHandle() const noexcept
	{
		return m_mapFile.GetHandle();
	}

	const Raii::Win32Handle FileMapping::GetHandle() const noexcept
	{
		return m_mapFile;
	}
	
	size_t FileMapping::GetFileSize() const
	{
		return m_maxSize;
	}

	#define LODWORD(_qw)    ((DWORD)(_qw))
	#define HIDWORD(_qw)    ((DWORD)(((_qw) >> 32) & 0xffffffff))

	void FileMapping::CreateOrOpen(
		const bool create,
		const DWORD desiredAccess,
		const bool isInheritable
	)
	{
		if (create)
		{
			LARGE_INTEGER li{ .QuadPart = static_cast<long long>(m_maxSize) };
			const wchar_t* name = m_name.empty() ? nullptr : m_name.c_str();
			// https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-createfilemappingw
			m_mapFile = CreateFileMappingW(
				INVALID_HANDLE_VALUE,	// use paging file
				nullptr,				// default security
				PAGE_READWRITE,			// read/write access
				li.HighPart,			// maximum object size (high-order DWORD)
				li.LowPart,				// maximum object size (low-order DWORD)
				name					// m_name of mapping object
			);
			if (!m_mapFile)
				throw Error::Win32Error(__FUNCSIG__": CreateFileMappingW() failed", GetLastError());
			m_mapFile.SetInheritability(isInheritable);
		}
		else
		{
			if (m_name.empty())
				throw std::runtime_error(__FUNCSIG__": m_name cannot be empty when opening a file mapping");
			// https://docs.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-openfilemappingw
			m_mapFile = OpenFileMappingW(
				desiredAccess,	// read/write access
				isInheritable,	// Should the handle be inheritable
				m_name.c_str()	// name of mapping object
			);
			if (!m_mapFile)
				throw Error::Win32Error(__FUNCSIG__": OpenFileMappingW() failed", GetLastError());
		}
	}

	FileMapping& FileMapping::Copy(const FileMapping& other)
	{
		Close();
		m_mapFile = other.m_mapFile;
		m_name = other.m_name;
		m_maxSize = other.m_maxSize;
		return *this;
	}

	FileMapping& FileMapping::Move(FileMapping& other) noexcept
	{
		Close();
		m_mapFile = std::move(other.m_mapFile);
		m_name = std::move(other.m_name);
		m_maxSize = other.m_maxSize;
		other.Close();
		return *this;
	}
}