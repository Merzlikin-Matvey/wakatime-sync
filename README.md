# wakatime-sync

A proxy server for redirecting WakaTime API requests to an alternative backend (e.g., Hackatime API). Allows you to use standard WakaTime clients with a different backend.


## Build

Requirements:
- CMake 3.15+
- MSVC
- Conan

First install dependencies using Conan:

```shell
conan install . --build=missing
```

Then build the project using CMake:
```shell
cd build
cmake .. -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE="generators/conan_toolchain.cmake"
cmake --build . --config Release
```

## Run

The server listens on `0.0.0.0:59178` and redirects requests to the Hackatime API.


## License

MIT 
