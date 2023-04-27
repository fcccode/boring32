export module boring32.async:processinfo;
import <win32.hpp>;
import boring32.raii;

export namespace Boring32::Async
{
	class ProcessInfo
	{
		public:
			virtual ~ProcessInfo()
			{
				Close();
			}

			ProcessInfo() = default;

			ProcessInfo(const ProcessInfo& other)
				: m_processInfo{ 0 }
			{
				Copy(other);
			}

			ProcessInfo(ProcessInfo&& other) noexcept
				: m_processInfo{ 0 }
			{
				Move(other);
			}

		public:
			virtual ProcessInfo& operator=(const ProcessInfo& other)
			{
				return Copy(other);
			}

			virtual ProcessInfo& operator=(ProcessInfo&& other) noexcept
			{
				return Move(other);
			}
		
		public:
			virtual void Close()
			{
				if (m_processInfo.hProcess)
				{
					CloseHandle(m_processInfo.hProcess);
					m_processInfo.hProcess = nullptr;
				}
				if (m_processInfo.hThread)
				{
					CloseHandle(m_processInfo.hThread);
					m_processInfo.hThread = nullptr;
				}
			}

			virtual PROCESS_INFORMATION& GetProcessInfo() noexcept
			{
				return m_processInfo;
			}

			virtual const PROCESS_INFORMATION& GetProcessInfo() const noexcept
			{
				return m_processInfo;
			}

			virtual PROCESS_INFORMATION* operator&() noexcept
			{
				return &m_processInfo;
			}

			virtual HANDLE GetProcessHandle() const noexcept
			{
				return m_processInfo.hProcess;
			}

			virtual HANDLE GetThreadHandle() const noexcept
			{
				return m_processInfo.hThread;
			}

		protected:
			virtual ProcessInfo& Copy(const ProcessInfo& other)
			{
				if (this == &other)
					return *this;
				Close();
				m_processInfo.dwProcessId = other.m_processInfo.dwProcessId;
				m_processInfo.dwThreadId = other.m_processInfo.dwThreadId;
				m_processInfo.hProcess = RAII::Win32Handle::DuplicatePassedHandle(other.m_processInfo.hProcess, false);
				m_processInfo.hThread = RAII::Win32Handle::DuplicatePassedHandle(other.m_processInfo.hThread, false);
				return *this;
			}

			virtual ProcessInfo& Move(ProcessInfo& other) noexcept
			{
				Close();
				m_processInfo.dwProcessId = other.m_processInfo.dwProcessId;
				m_processInfo.dwThreadId = other.m_processInfo.dwThreadId;
				m_processInfo.hProcess = other.m_processInfo.hProcess;
				m_processInfo.hThread = other.m_processInfo.hThread;
				other.m_processInfo.hProcess = nullptr;
				other.m_processInfo.hThread = nullptr;
				return *this;
			}

		protected:
			PROCESS_INFORMATION m_processInfo{ 0 };
	};
}