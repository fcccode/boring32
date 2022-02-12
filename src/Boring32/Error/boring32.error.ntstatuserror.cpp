module;

#include <string>
#include <format>
#include <source_location>
#include <Windows.h>

module boring32.error.ntstatuserror;
import boring32.error.functions;

namespace Boring32::Error
{
	// Related: https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-erref/596a1078-e883-4972-9bbc-49e60bebca55
	NtStatusError::~NtStatusError() {}

	NtStatusError::NtStatusError(
		const std::source_location& location, 
		const std::string& msg
	)
		: std::runtime_error(msg),
		m_errorCode(0)
	{
		m_errorString = std::format(
			"NTSTATUS exception in function {}() in {}:{}:{}: {}",
			location.function_name(),
			location.file_name(),
			location.line(),
			location.column(),
			msg
		);
	}
	
	NtStatusError::NtStatusError(
		const std::source_location& location, 
		const std::string& msg, 
		const LONG errorCode
	)
		: std::runtime_error(msg),
		m_errorCode(errorCode)
	{
		m_errorString = Boring32::Error::GetNtStatusCode<std::string>(errorCode);
		m_errorString = std::format(
			"NTSTATUS exception in function {}() in {}:{}:{}: {} (NTSTATUS code: {}, {:#X}): {}",
			location.function_name(),
			location.file_name(),
			location.line(),
			location.column(),
			msg, 
			errorCode, 
			errorCode, 
			m_errorString
		);
	}

	LONG NtStatusError::GetErrorCode() const noexcept
	{
		return m_errorCode;
	}

	const char* NtStatusError::what() const noexcept
	{
		return m_errorString.c_str();
	}
}
