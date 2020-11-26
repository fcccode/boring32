#pragma once
#include <string>
#include <Windows.h>
#include "Win32Error.hpp"
#include "ComError.hpp"

namespace Boring32::Error
{
	void GetErrorCodeString(const DWORD errorCode, std::string& stringToHoldMessage) noexcept;

	void GetErrorCodeString(const DWORD errorCode, std::wstring& stringToHoldMessage) noexcept;

	std::wstring CreateErrorStringFromCode(const std::wstring msg, const DWORD errorCode) noexcept;

	std::string CreateErrorStringFromCode(const std::string msg, const DWORD errorCode) noexcept;

	std::wstring GetErrorFromHResult(const std::wstring& msg, const HRESULT hr) noexcept;

	std::string GetErrorFromHResult(const std::string& msg, const HRESULT hr) noexcept;

	template<typename T, typename S>
	inline bool TryCatchLogToWCerr(const T& function, const S* string) noexcept
	{
		try
		{
			function();
			return true;
		}
		catch (const std::exception& ex)
		{
			std::wcerr
				<< string
				<< L" "
				<< ex.what()
				<< std::endl;
			return false;
		}
	}

	template<typename T, typename S>
	inline bool TryCatchLogToWCerr(const T& function, const S& string) noexcept
	{
		return TryCatchLogToWCerr(function, string.c_str());
	}
}
