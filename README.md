# Memory-Module
Simple windows mutli-media controller 

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white">
  <img src="https://img.shields.io/badge/Visual_Studio-5C2D91?style=for-the-badge&logo=visual%20studio&logoColor=white">
  <img src="https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white">
  <img src="https://img.shields.io/badge/license-GPL_3.0-blue?style=for-the-badge&logo=&logoColor=whit">
</p>

## code examples

### Creating an instance of `MediaController`
```cpp
MediaController media;
```

### Getting the current system volume
```cpp
float Volume = media.get_volume();
```

### Setting the system volume
```cpp
media.set_volume(50.f);
```

## Controlling media playback

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


