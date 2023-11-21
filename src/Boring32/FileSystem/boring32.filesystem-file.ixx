export module boring32.filesystem:file;
import std;
import std.compat;
import boring32.raii;
import boring32.error;
import boring32.win32;

export namespace Boring32::FileSystem
{
	class File final
	{
		public:
			~File() = default;
			File() = default;
			File(const File&) = default;
			File(File&&) noexcept = default;
			File& operator=(const File&) = default;
			File& operator=(File&&) noexcept = default;

		public:
			File(std::wstring fileName)
				: m_fileName(std::move(fileName))
			{
				InternalOpen();
			}

		public:
			void Close()
			{
				m_fileHandle = nullptr;
			}

			Win32::HANDLE GetHandle() const noexcept
			{
				return *m_fileHandle;
			}

		private:
			void InternalOpen()
			{
				if (m_fileName.empty())
					throw Error::Boring32Error("Filename must be specified");

				m_fileHandle = Win32::CreateFileW(
					m_fileName.c_str(),				// lpFileName
					Win32::GenericRead | Win32::GenericWrite,	// dwDesiredAccess
					// https://learn.microsoft.com/en-us/windows/win32/secauthz/generic-access-rights
					// https://learn.microsoft.com/en-us/windows/win32/fileio/file-security-and-access-rights
					// https://learn.microsoft.com/en-us/windows/win32/fileio/file-access-rights-constants
					0,								// dwShareMode
					nullptr,						// lpSecurityAttributes
					Win32::OpenAlways,					// dwCreationDisposition
					Win32::FileAttributeNormal,			// dwFlagsAndAttributes
					nullptr							// hTemplateFile
				);
				if (m_fileHandle == Win32::InvalidHandleValue)
				{
					const auto lastError = Win32::GetLastError();
					throw Error::Win32Error("CreateFileW() failed", lastError);
				}
			}

		private:
			std::wstring m_fileName;
			RAII::Win32Handle m_fileHandle;
	};
}