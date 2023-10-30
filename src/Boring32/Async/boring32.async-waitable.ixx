export module boring32.async:waitable;
import <chrono>;
import boring32.win32;
import boring32.error;
import :functions;
import :concepts;

export namespace Boring32::Async
{
	template<auto FOnSuccess = nullptr, auto FOnTimeout = nullptr, auto FOnFailure = nullptr>
	class Waitable final
	{
		public:
			Waitable(Win32::HANDLE handle)
				: m_handle(handle)
			{
				if (not m_handle)
					throw Error::Boring32Error("Waitable handle required");
			}

		public:
			operator bool() const noexcept 
			{
				return m_handle != nullptr;
			}

			operator HANDLE() const noexcept
			{
				return m_handle; 
			}

			void operator()(Duration auto duration, const bool alertable = false)
			{
				DoWaitAndGet(duration, alertable);
			}

		public:
			WaitResult DoWaitAndGet(Duration auto duration, const bool alertable = false)
			{
				m_lastWait = WaitFor(
					m_handle,
					static_cast<unsigned long>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()),
					alertable
				);
				if constexpr (FOnSuccess != nullptr)
				{
					static_assert(std::is_invocable_v<decltype(FOnSuccess)>, "FOnSuccess must be invocable");
					if (m_lastWait == WaitResult::Success)
						FOnSuccess();
				}
				if constexpr (FOnTimeout != nullptr)
				{
					static_assert(std::is_invocable_v<decltype(FOnTimeout)>, "FOnTimeout must be invocable");
					if (m_lastWait == WaitResult::Timeout)
						FOnTimeout();
				}
				if constexpr (FOnFailure != nullptr)
				{
					static_assert(std::is_invocable_v<decltype(FOnFailure)>, "FOnFailure must be invocable");
					if (m_lastWait == WaitResult::Failure)
						FOnFailure();
				}
				return m_lastWait;
			}

			Waitable& DoWait(Duration auto duration, const bool alertable = false)
			{
				DoWaitAndGet(duration, alertable);
				return *this;
			}

			void AssertSuccess()
			{
				if (m_lastWait != WaitResult::Success)
					throw Error::Boring32Error("Last wait was not successful");
			}

			Waitable& OnSuccess(auto&& func, auto&&...args)
			{
				if (m_lastWait == WaitResult::Success)
					func(std::forward<decltype(args)>(args)...);
				return *this;
			}

			Waitable& OnTimeout(auto&& func, auto&&...args)
			{
				if (m_lastWait == WaitResult::Timeout)
					func(std::forward<decltype(args)>(args)...);
				return *this;
			}

		private:
			WaitResult m_lastWait = WaitResult::Failed;
			Win32::HANDLE m_handle = nullptr;
	};
}