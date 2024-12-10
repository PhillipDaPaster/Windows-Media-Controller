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
        CoInitialize(NULL);
        CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&deviceEnumerator));
        deviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &defaultDevice);
        defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (void**)&endpointVolume);
    }

    ~MediaController() {
        if (endpointVolume) endpointVolume->Release();
        if (defaultDevice) defaultDevice->Release();
        if (deviceEnumerator) deviceEnumerator->Release();
        CoUninitialize();
    }

    float get_volume() {
        float volume = 0.0f;
        return (endpointVolume && SUCCEEDED(endpointVolume->GetMasterVolumeLevelScalar(&volume))) ? volume * 100.0f : 0.0f;
    }

    void set_volume(float volume) {
        if (endpointVolume) endpointVolume->SetMasterVolumeLevelScalar(volume / 100.0f, NULL);
    }

    void play() {
        mediacommand(APPCOMMAND_MEDIA_PLAY);
    }

    void pause() {
        mediacommand(APPCOMMAND_MEDIA_PAUSE);
    }

    void stop() {
        mediacommand(APPCOMMAND_MEDIA_STOP);
    }

    void next() {
        mediacommand(APPCOMMAND_MEDIA_NEXTTRACK);
    }

    void previous() {
        mediacommand(APPCOMMAND_MEDIA_PREVIOUSTRACK);
    }

private:
    IAudioEndpointVolume* endpointVolume = nullptr;
    IMMDeviceEnumerator* deviceEnumerator = nullptr;
    IMMDevice* defaultDevice = nullptr;

    void mediacommand(int command) {
        HWND hwnd = GetForegroundWindow();
        if (hwnd) {
            PostMessage(hwnd, WM_APPCOMMAND, (WPARAM)hwnd, MAKELPARAM(0, command));
        }
    }
};
