module boring32.filesystem:win32;
import boring32.error;

namespace Boring32::FileSystem::Win32
{
	HANDLE CreateFile(
		const std::wstring& fileName,
		const DWORD desiredAccess,
		const DWORD shareMode,
		SECURITY_ATTRIBUTES* securityAttributes,
		const DWORD creationDisposition,
		const DWORD flagsAndAttributes,
		const HANDLE templateFile,
		const std::source_location& location
	)
	{
		if (fileName.empty())
			throw Error::Boring32Error("Filename must be specified");

		// https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew
		const HANDLE fileHandle = CreateFileW(
			fileName.c_str(),				// lpFileName
			desiredAccess,	// dwDesiredAccess
			// https://learn.microsoft.com/en-us/windows/win32/secauthz/generic-access-rights
			// https://learn.microsoft.com/en-us/windows/win32/fileio/file-security-and-access-rights
			// https://learn.microsoft.com/en-us/windows/win32/fileio/file-access-rights-constants
			shareMode,								// dwShareMode
			securityAttributes,						// lpSecurityAttributes
			creationDisposition,					// dwCreationDisposition
			flagsAndAttributes,			// dwFlagsAndAttributes
			templateFile							// hTemplateFile
		);
		if (fileHandle == INVALID_HANDLE_VALUE)
		{
			const auto lastError = GetLastError();
			throw Error::Win32Error("CreateFileW() failed", lastError, location);
		}
		return fileHandle;
	}

	DWORD WriteFile(
		HANDLE file,
		void* lpBuffer,
		const DWORD numberOfBytesToWrite,
		const std::source_location& location
	)
	{
		if (!file)
			throw Error::Boring32Error("File handle cannot be null", location);
		if (file == INVALID_HANDLE_VALUE)
			throw Error::Boring32Error("File handle cannot be INVALID_HANDLE_VALUE", location);
		if (numberOfBytesToWrite == 0)
			return 0;
		if (!lpBuffer)
			throw Error::Boring32Error("Buffer cannot be null", location);

		DWORD numberOfBytesWritten;
		const bool success = ::WriteFile(
			file,
			lpBuffer,
			numberOfBytesToWrite,
			&numberOfBytesWritten,
			nullptr
		);
		if (!success)
		{
			const auto lastError = GetLastError();
			throw Error::Win32Error(
				"WriteFile() failed", 
				lastError, 
				location
			);
		}

		return numberOfBytesWritten;
	}
}