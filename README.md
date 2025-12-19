# GNEngine

GNEngine is a 2D game engine developed using Modern C++23 and SDL3.

※This project is personal toy project. Not completed.※

## Features

- **ECS**: Effecient system.
- **Hybrid DOD System**: Can use both AoS and SoA Data. (Required SoA specialization in ComponentArray.h. Customization support planned lator)
- **Event-Driven System**: Flexibly handles various events
- **2D Rendering**: Renders textures and animation to the screen.
- **Sound**: Play and manage WAV, MP3, and OGG format sounds.
Can use stereo sound to 3D sound with HRTF.
- **Scene Management**: Easily switch and manage game scenes.
- **Multi platform**: Not yet... Currently only supports Windows and Linux. Android will be supported later (maybe..)


## Build

This project is built using CMake and `CMakePresets.json`. I used GNU g++ (v15.1.0) and ninja (v1.13.0).

1.  **Configure CMake:**
- available Platform : windows, linux

    ```bash
    cmake --preset "Platform"-preset
    ```

1.  **Build:**
    ```bash
    cmake --build out/build/"Platform"-preset
    ```

## Example
- Available example
  - T.C.S

### Buile

```bash
cmake --build out/build/"Platform"-preset --target "A folder name of example"
```

### Run

```bash
out/build/custom-preset/Debug/"A folder name of example".exe
```

## Directory Structure

- `src/GNEngine/`: All source codes of GNEngine headers.
- `include/engine/`: All header files of GNEngine. 
  - `core`: core files of engine.
  - `system/`: Perform engine logics
  - `manager/`: Manage resources and system and component and etc.
  - `component`: Defines the base component and other concrete components like TransformComponent.
  - `resurce/`: Embedded purpose resources (images, fonts, sounds, etc.)
- `lib/`: External libraries used.

- `example/` : Example projects of GNEngine.
  - `top_view_game`: Unfinished.
  - `snake_game`: Unfinished.

## Used libraries

- SDL3 (v3.2.18): [https://github.com/libsdl-org/SDL](https://github.com/libsdl-org/SDL)
- SDL3_image (v3.2.4): [https://github.com/libsdl-org/SDL_image](https://github.com/libsdl-org/SDL_image)
- SDL3_ttf (v3.2.2): [https://github.com/libsdl-org/SDL_ttf](https://github.com/libsdl-org/SDL_ttf)
- OpenAL Soft (v1.24.3): [https://github.com/kcat/openal-soft](https://github.com/kcat/openal-soft)
- for decoding sound files :
    - dr_wav(v0.14.0): [https://github.com/mackron/dr_libs](https://github.com/mackron/dr_libs) 
    - dr_mp3(v0.7.1): [https://github.com/mackron/dr_libs](https://github.com/mackron/dr_libs)
    - dr_flac(v0.13.0): [https://github.com/mackron/dr_libs](https://github.com/nothings/dr_libs)
    - stb_vorbis(v1.22): [https://github.com/nothings/stb](https://github.com/nothings/stb)
- nlohmann/json(3.12.0): [https://github.com/nlohmann/json](https://github.com/nlohmann/json)
- google/flatBuffer(): [https://github.com/google/flatbuffers](https://github.com/google/flatbuffers)
- box2D(3.1.1): [https://github.com/erincatto/box2d](https://github.com/erincatto/box2d)