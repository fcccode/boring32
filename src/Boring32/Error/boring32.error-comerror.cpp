module;

#include <format>
#include <string>
#include <source_location>
#include <Windows.h>

module boring32.error:comerror;
import :functions;

namespace Boring32::Error
{
	ComError::~ComError() {}

	ComError::ComError(const ComError& other)					= default;
	ComError::ComError(ComError&& other) noexcept				= default;
	ComError& ComError::operator=(const ComError& other)		= default;
	ComError& ComError::operator=(ComError&& other) noexcept	= default;

	ComError::ComError(
		const std::string& msg, 
		const HRESULT hr,
		const std::source_location location
	)
		: std::runtime_error(""),
		m_hresult(hr)
	{
		m_errorString = Boring32::Error::TranslateErrorCode<std::string>(hr);
		m_errorString = Error::FormatErrorMessage("COM", location, msg, hr, m_errorString);
	}

	HRESULT ComError::GetHResult() const noexcept
	{
		return m_hresult;
	}

	const char* ComError::what() const noexcept
	{
		return m_errorString.c_str();
	}
}
