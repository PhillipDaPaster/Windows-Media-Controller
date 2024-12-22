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
        CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&enumerator));
        enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &device);
        device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (void**)&enpoint_volume);
        manager = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
    }

    ~MediaController() {
        if (enpoint_volume) enpoint_volume->Release();
        if (device) device->Release();
        if (enumerator) enumerator->Release();
        CoUninitialize();
    }

    float get_volume() {
        float volume = 0.0f;
        return (enpoint_volume && SUCCEEDED(enpoint_volume->GetMasterVolumeLevelScalar(&volume))) ? volume * 100.0f : 0.0f;
    }

    void set_volume(float volume) {
        if (enpoint_volume) enpoint_volume->SetMasterVolumeLevelScalar(volume / 100.0f, NULL);
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

    std::string get_title() {
        auto session = manager.GetCurrentSession();
        return session ? winrt::to_string(session.TryGetMediaPropertiesAsync().get().Title()) : "N/A";
    }

    std::string get_artist() {
        auto session = manager.GetCurrentSession();
        return session ? winrt::to_string(session.TryGetMediaPropertiesAsync().get().Artist()) : "N/A";
    }

private:
    GlobalSystemMediaTransportControlsSessionManager manager{ nullptr };
    IAudioEndpointVolume* enpoint_volume = nullptr;
    IMMDeviceEnumerator* enumerator = nullptr;
    IMMDevice* device = nullptr; 

    void mediacommand(int command) {
        HWND hwnd = GetForegroundWindow();
        if (hwnd) {
            PostMessage(hwnd, WM_APPCOMMAND, (WPARAM)hwnd, MAKELPARAM(0, command));
        }
    }
};
