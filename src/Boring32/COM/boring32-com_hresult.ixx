export module boring32:com_hresult;
import boring32.shared;
import :error;
import :com_functions;

export namespace Boring32::COM
{
	// See https://learn.microsoft.com/en-us/windows/win32/com/error-handling-in-com
	class HResult final
	{
		// The six
		public:
			~HResult() noexcept = default;
			HResult() = default;
			HResult(const HResult&) = default;
			HResult& operator=(const HResult&) = default;
			HResult(HResult&&) noexcept = default;
			HResult& operator=(HResult&&) = default;

		public:
			HResult(const Win32::HRESULT hr) noexcept
				: m_hr(hr)
			{ }

			HResult(
				const long severity, 
				const long facility, 
				const long code
			) noexcept
			{
				m_hr = Win32::MakeHResult(severity, facility, code);
			}

		public:
			operator Win32::HRESULT() const noexcept
			{
				return m_hr;
			}

			HResult& operator=(const Win32::HRESULT hr) noexcept
			{
				m_hr = hr;
				return *this;
			}

			operator bool() const noexcept
			{
				return Succeeded(m_hr);
			}

			bool operator==(const Win32::HRESULT hr) const noexcept
			{
				return m_hr == hr;
			}

			bool operator==(const HResult& hr) const noexcept
			{
				return m_hr == hr.m_hr;
			}

		// See https://learn.microsoft.com/en-us/windows/win32/com/using-macros-for-error-handling
		public:
			HRESULT Get() const noexcept
			{
				return m_hr;
			}

			long Facility() const noexcept
			{
				return Win32::Facility(m_hr);
			}

			long Code() const noexcept
			{
				return Win32::Code(m_hr);
			}

			long Severity() const noexcept
			{
				return Win32::Severity(m_hr);
			}

			void ThrowIfFailed(
				const char* msg,
				const std::source_location& loc = std::source_location::current()
			) const
			{
				if (Succeeded(m_hr))
					return;
				if (msg)
					throw Error::COMError(msg, m_hr, loc);
				throw Error::COMError("HRESULT check failed", m_hr, loc);
			}

		private:
			Win32::HRESULT m_hr = 0x0;
	};
}
