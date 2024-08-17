export module boring32:xaudio2_sourcevoice;
import boring32.shared;
import :error;
import :xaudio2_xaudio2error;
import :xaudio2_voice;

export namespace Boring32::XAudio2
{
	// https://docs.microsoft.com/en-us/windows/win32/api/xaudio2/nn-xaudio2-ixaudio2sourcevoice
	class SourceVoice : public Voice
	{
		public:
			virtual ~SourceVoice()
			{
				Close();
			}

			SourceVoice(const SourceVoice&) = delete;
			SourceVoice(SourceVoice&&) noexcept = default;
			SourceVoice(Win32::IXAudio2SourceVoice* voice)
				: m_voice(voice)
			{
				if (!voice)
					throw XAudio2Error("voice cannot be nullptr");
			}

		public:
			virtual SourceVoice& operator=(const SourceVoice&) = delete;
			virtual SourceVoice& operator=(SourceVoice&&) noexcept = default;

		public:
			virtual void Close()
			{
				if (m_voice)
				{
					m_voice->DestroyVoice();
					m_voice = nullptr;
				}
			}

		protected:
			Win32::IXAudio2SourceVoice* m_voice;
	};
}