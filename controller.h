#include <winrt/base.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <windows.h>
#include <mmdeviceapi.h>
#include <mmsystem.h>
#include <endpointvolume.h>

using namespace winrt;
using namespace Windows::Media::Control;
using namespace Windows::Storage::Streams;

#pragma comment(lib, "windowsapp")
#pragma comment(lib, "winmm.lib")

class MediaController {
public:
	MediaController() {
		winrt::init_apartment();
		CoInitialize(NULL);
		CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&Enumerator));
		Enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &Device);
		Device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (void**)&EndpointVolume);
		Manager = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
	}

	~MediaController() {
		if (EndpointVolume)
			EndpointVolume->Release();
		if (Device)
			Device->Release();
		if (Enumerator)
			Enumerator->Release();
		CoUninitialize();
	}

	float GetVolume() {
		float volume = 0.0f;
		return (EndpointVolume && SUCCEEDED(EndpointVolume->GetMasterVolumeLevelScalar(&volume))) ? volume * 100.0f : 0.0f;
	}

	void SetVolume(float volume) {
		if (EndpointVolume)
			EndpointVolume->SetMasterVolumeLevelScalar(volume / 100.0f, NULL);
	}

	void Play() {
		MediaCommand(APPCOMMAND_MEDIA_PLAY);
	}

	void Pause() {
		MediaCommand(APPCOMMAND_MEDIA_PAUSE);
	}

	void Stop() {
		MediaCommand(APPCOMMAND_MEDIA_STOP);
	}

	void Nect() {
		MediaCommand(APPCOMMAND_MEDIA_NEXTTRACK);
	}

	void Previous() {
		MediaCommand(APPCOMMAND_MEDIA_PREVIOUSTRACK);
	}

	std::string GetTitle() {
		auto Session = Manager.GetCurrentSession();
		return Session ? winrt::to_string(Session.TryGetMediaPropertiesAsync().get().Title()) : "N/A";
	}

	std::string GetArtist() {
		auto Session = Manager.GetCurrentSession();
		return Session ? winrt::to_string(Session.TryGetMediaPropertiesAsync().get().Artist()) : "N/A";
	}

private:
	GlobalSystemMediaTransportControlsSessionManager Manager{ nullptr };
	IAudioEndpointVolume* EndpointVolume = nullptr;
	IMMDeviceEnumerator* Enumerator = nullptr;
	IMMDevice* Device = nullptr;

	void MediaCommand(int command) {
		HWND Hwnd = GetForegroundWindow();
		if (Hwnd) {
			PostMessage(Hwnd, WM_APPCOMMAND, (WPARAM)Hwnd, MAKELPARAM(0, command));
		}
	}
};
