#include "pch.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

import boring32.error;

namespace Error
{
	TEST_CLASS(Win32Error)
	{
		std::string m_errorMessage = "Some error message";

		public:
			TEST_METHOD(TestConstructor)
			{
				Boring32::Error::Win32Error basicError(m_errorMessage);
				Assert::IsTrue(std::string(basicError.what()).contains(m_errorMessage));
			}

			TEST_METHOD(TestCopyConstructor)
			{
				Boring32::Error::Win32Error error1(m_errorMessage, 0x5);
				Boring32::Error::Win32Error error2(error1);
				Assert::IsTrue(std::string(error1.what()) == std::string(error2.what()));
				Assert::IsTrue(error1.GetErrorCode() == error2.GetErrorCode());
			}
	};
}