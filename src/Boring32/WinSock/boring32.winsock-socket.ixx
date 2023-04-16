export module boring32.winsock:socket;
import "win32.hpp";

export namespace Boring32::WinSock
{
	class Socket final
	{
		public:
			~Socket();
			Socket() = default;
			Socket(const Socket&) = delete;
			Socket(Socket&&) noexcept;
			Socket(const SOCKET socket);

		public:
			Socket& operator=(Socket&&) noexcept;
			Socket& operator=(const Socket&) = delete;

		public:
			void Close();

		private:
			Socket& Move(Socket& other);

		private:
			SOCKET m_socket = INVALID_SOCKET;
	};
}
