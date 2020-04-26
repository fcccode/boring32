#include "pch.hpp"
#include "Objbase.h"
#include "include/Win32Utils.hpp"

namespace Win32Utils::Guid
{
	// Adapted from https://stackoverflow.com/a/19941516/7448661
	std::wstring GetGuidAsWString(const GUID& guid)
	{
		wchar_t rawGuid[64] = { 0 };
		StringFromGUID2(guid, rawGuid, 64);
		return std::wstring(rawGuid);
	}
}
