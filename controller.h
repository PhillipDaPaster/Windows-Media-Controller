#pragma once

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Media.Control.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>

#pragma comment(lib, "windowsapp")
#pragma comment(lib, "Ole32.lib")

using namespace winrt;
using namespace Windows::Media::Control;
using namespace Windows::Storage::Streams;

class MediaController {
public:
    MediaController() {
        try {
            init_apartment();

            HRESULT hr;

            Enumerator = winrt::com_ptr<IMMDeviceEnumerator>();
            Device = winrt::com_ptr<IMMDevice>();
            EndpointVolume = winrt::com_ptr<IAudioEndpointVolume>();

            hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(Enumerator.put()));
            if (FAILED(hr)) {
                return;
            }

            hr = Enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, Device.put());
            if (FAILED(hr)) {
                return;
            }

            hr = Device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(EndpointVolume.put()));
            if (FAILED(hr)) {
                EndpointVolume = nullptr;
            }

            Manager = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        }
        catch (...) {
            Manager = nullptr;
        }
    }

    float GetVolume() const {
        float vol = 0.0f;

        if (EndpointVolume) {
            if (SUCCEEDED(EndpointVolume->GetMasterVolumeLevelScalar(&vol))) {
                return vol * 100.0f;
            }
        }

        return 0.0f;
    }

    void SetVolume(float volume) const {
        if (EndpointVolume) {
            float clamped = std::clamp(volume, 0.0f, 100.0f);
            EndpointVolume->SetMasterVolumeLevelScalar(clamped / 100.0f, nullptr);
        }
    }

    void Play() const {
        SendMediaCommand(APPCOMMAND_MEDIA_PLAY);
    }

    void Pause() const {
        SendMediaCommand(APPCOMMAND_MEDIA_PAUSE);
    }

    void Stop() const {
        SendMediaCommand(APPCOMMAND_MEDIA_STOP);
    }

    void Next() const {
        SendMediaCommand(APPCOMMAND_MEDIA_NEXTTRACK);
    }

    void Previous() const {
        SendMediaCommand(APPCOMMAND_MEDIA_PREVIOUSTRACK);
    }

    std::string GetTitle() const {
        return GetStringProperty([](GlobalSystemMediaTransportControlsSessionMediaProperties const& props) {
            return props.Title();
        });
    }

    std::string GetArtist() const {
        return GetStringProperty([](GlobalSystemMediaTransportControlsSessionMediaProperties const& props) {
            return props.Artist();
        });
    }

    std::string GetAlbumTitle() const {
        return GetStringProperty([](GlobalSystemMediaTransportControlsSessionMediaProperties const& props) {
            return props.AlbumTitle();
        });
    }

    std::vector<uint8_t> GetAlbumArt() const {
        std::vector<uint8_t> data;

        try {
            GlobalSystemMediaTransportControlsSession session = GetCurrentSession();
            if (!session) {
                return data;
            }

            GlobalSystemMediaTransportControlsSessionMediaProperties props = session.TryGetMediaPropertiesAsync().get();
            IRandomAccessStreamReference thumbnail = props.Thumbnail();

            if (!thumbnail) {
                return data;
            }

            IRandomAccessStream stream = thumbnail.OpenReadAsync().get();
            if (!stream) {
                return data;
            }

            uint64_t size = stream.Size();
            if (size == 0 || size > (1 << 24)) {
                return data;
            }

            stream.Seek(0);
            Buffer buffer(static_cast<uint32_t>(size));
            stream.ReadAsync(buffer, static_cast<uint32_t>(size), InputStreamOptions::None).get();

            DataReader reader = DataReader::FromBuffer(buffer);
            data.resize(static_cast<size_t>(size));
            reader.ReadBytes(winrt::array_view<uint8_t>(data));
        }
        catch (...) {
            data.clear();
        }

        return data;
    }

    std::string GetPlaybackStatus() const {
        try {
            GlobalSystemMediaTransportControlsSession session = GetCurrentSession();
            if (!session) {
                return "Unknown";
            }

            GlobalSystemMediaTransportControlsSessionPlaybackInfo info = session.GetPlaybackInfo();

            switch (info.PlaybackStatus()) {
            case GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing:
                return "Playing";
            case GlobalSystemMediaTransportControlsSessionPlaybackStatus::Paused:
                return "Paused";
            case GlobalSystemMediaTransportControlsSessionPlaybackStatus::Stopped:
                return "Stopped";
            case GlobalSystemMediaTransportControlsSessionPlaybackStatus::Changing:
                return "Changing";
            default:
                return "Unknown";
            }
        }
        catch (...) {
            return "Unknown";
        }
    }

    bool IsPaused() const {
        try {
            GlobalSystemMediaTransportControlsSession session = GetCurrentSession();

            if (session) {
                GlobalSystemMediaTransportControlsSessionPlaybackInfo info = session.GetPlaybackInfo();
                return info.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Paused;
            }
        }
        catch (...) {}

        return false;
    }

    double GetPosition() const {
        try {
            GlobalSystemMediaTransportControlsSession session = GetCurrentSession();

            if (session) {
                return session.GetTimelineProperties().Position().count() / 1e7;
            }
        }
        catch (...) {}

        return 0.0;
    }

    double GetDuration() const {
        try {
            GlobalSystemMediaTransportControlsSession session = GetCurrentSession();

            if (session) {
                return session.GetTimelineProperties().EndTime().count() / 1e7;
            }
        }
        catch (...) {}

        return 0.0;
    }

    std::string GetFormattedTime() const {
        double pos = GetPosition();
        double dur = GetDuration();

        return FormatTime(pos) + " / " + FormatTime(dur);
    }

private:
    GlobalSystemMediaTransportControlsSessionManager Manager{ nullptr };
    winrt::com_ptr<IAudioEndpointVolume> EndpointVolume;
    winrt::com_ptr<IMMDeviceEnumerator> Enumerator;
    winrt::com_ptr<IMMDevice> Device;

    void SendMediaCommand(int command) const {
        HWND hwnd = GetShellWindow();

        if (hwnd != nullptr) {
            PostMessage(hwnd, WM_APPCOMMAND, reinterpret_cast<WPARAM>(hwnd), MAKELPARAM(0, command));
        }
    }

    GlobalSystemMediaTransportControlsSession GetCurrentSession() const {
        if (Manager != nullptr) {
            return Manager.GetCurrentSession();
        }

        return nullptr;
    }

    template<typename Getter>
    std::string GetStringProperty(Getter getter) const {
        try {
            GlobalSystemMediaTransportControlsSession session = GetCurrentSession();

            if (!session) {
                return "N/A";
            }

            GlobalSystemMediaTransportControlsSessionMediaProperties props = session.TryGetMediaPropertiesAsync().get();
            hstring hstr = getter(props);

            return winrt::to_string(hstr);
        }
        catch (...) {
            return "N/A";
        }
    }

    static std::string FormatTime(double seconds) {
        if (seconds < 0.0) {
            seconds = 0.0;
        }

        int total = static_cast<int>(seconds);
        int minutes = total / 60;
        int secs = total % 60;

        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << minutes << ":"
            << std::setw(2) << std::setfill('0') << secs;

        return oss.str();
    }
};
