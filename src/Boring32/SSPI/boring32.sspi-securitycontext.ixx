export module boring32.sspi:securitycontext;
import <vector>;
import <win32.hpp>;

export namespace Boring32::SSPI
{
	enum class BufferType
	{
		Token = SECBUFFER_TOKEN,
		Data = SECBUFFER_DATA
	};

	// https://learn.microsoft.com/en-us/windows/win32/secauthn/initializesecuritycontext--general
	// https://learn.microsoft.com/en-us/windows/win32/api/sspi/nf-sspi-initializesecuritycontextw
	// https://learn.microsoft.com/en-us/windows/win32/secauthn/initializesecuritycontext--schannel
	// Should this be SchannelSecurityContext, or should we subclass it?
	class SecurityContext
	{
		public:
			virtual ~SecurityContext() = default;
			SecurityContext() = default;
			SecurityContext(const SecurityContext&) = delete;
			SecurityContext(SecurityContext&&) noexcept = delete;

		public:
			virtual void Init();
			virtual void AddInBuffer(
				const BufferType type,
				const unsigned size
			);
			virtual void AddOutBuffer(
				const BufferType type,
				const unsigned size
			);

		protected:
			SecHandle m_context = { 0 };
			bool m_initialised = false;
			std::vector<SecBuffer> m_inBuffers;
			std::vector<SecBuffer> m_outBuffers;
	};
}