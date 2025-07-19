
<h1 align="center">Windows Multi Media Controller</h1>

### A modern, safe, and lightweight C++ wrapper around Windows' system media transport and volume control APIs.

### Supports playback management, media metadata retrieval (title, artist, album, artwork), and system volume control — all via native WinRT and Core Audio interfaces.

---
<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white">
  <img src="https://img.shields.io/badge/Visual_Studio-5C2D91?style=for-the-badge&logo=visual%20studio&logoColor=white">
  <img src="https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white">
  <img src="https://img.shields.io/badge/license-GPL_3.0-blue?style=for-the-badge&logo=&logoColor=whit">
</p>


## Example Usage

### Create a MediaController Instance
```cpp
MediaController media;
```

### Get Current System Volume
```cpp
float volume = media.GetVolume();
```

### Setting System Volume
```cpp
media.set_volume(50.f);
```

### Retrieve Current Song Info
```cpp
std::string title  = media.GetTitle();
std::string artist = media.GetArtist();
std::string album  = media.GetAlbumTitle();
```

### Retrieve Album Art
```cpp
std::vector<uint8_t> art = media.GetAlbumArt(); 
```

### Get Playback Status
```cpp
std::string status = media.GetPlaybackStatus();  // "Playing", "Paused", "Stopped", "Changing".
```

### Get Timeline Info
```cpp
double position = media.GetPosition();   // seconds
double duration = media.GetDuration();   // seconds
std::string time = media.GetFormattedTime(); // MM:SS / MM:SS

```

## Media Playback Controls

### Play Media
```cpp
media.play();
```

### Pause Media
```cpp
media.pause();
```

### Stop Current Media
```cpp
media.stop();
```
### Skip Track
```cpp
media.next();
```
### Previous Track
```cpp
media.previous();
```

## Dependencies 
> <mark>**[WinRT](https://learn.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/intro-to-using-cpp-with-winrt).**</mark><br>
> Required for accessing Windows Runtime APIs like `Windows.Media.Control` and `Windows.Storage.Streams`.  
> <mark>**[Core Audio API](https://learn.microsoft.com/en-us/windows/win32/coreaudio/core-audio-portal).**</mark><br>
> Includes headers like `mmdeviceapi.h` and `endpointvolume.h` for audio control.  
> <mark>**[Windows SDK](https://developer.microsoft.com/en-us/windows/downloads/sdk-archive/).**</mark><br>
> Provides the necessary libraries and tools to build applications for Windows.  

## License
This project is licensed under the **GNU General Public License**.  
See the [LICENSE](LICENSE) file for more details.

<p align="center">
  <i>Windows Multi Media Controller</i>
</p>
