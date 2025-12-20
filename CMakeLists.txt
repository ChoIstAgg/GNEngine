cmake_minimum_required(VERSION 3.25)
project(GNEngine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-std=c++23")

# Set output directories for libraries and executables to keep the build directory clean.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)

# Set the installation directory to be inside the build folder.
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")


# Platform-specific configurations
if(WIN32)
# MinGW 런타임 경로 설정 (Windows)
    set(MINGW_BIN_DIR "C:/Development_Environment/code/MSYS2/ucrt64/bin")

# SDL 경로 설정 (Windows)
    set(SDL3_DIR "${CMAKE_SOURCE_DIR}/lib/SDL3-3.2.18/x86_64-w64-mingw32/lib/cmake/SDL3")
    set(SDL3_DLL_DIR "${CMAKE_SOURCE_DIR}/lib/SDL3-3.2.18/x86_64-w64-mingw32/bin")
    set(SDL3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/SDL3-3.2.18/x86_64-w64-mingw32/include")
    set(SDL3_image_DIR "${CMAKE_SOURCE_DIR}/lib/SDL3_image-3.2.4/x86_64-w64-mingw32/lib/cmake/SDL3_image")
    set(SDL3_image_DLL_DIR "${CMAKE_SOURCE_DIR}/lib/SDL3_image-3.2.4/x86_64-w64-mingw32/bin")
    set(SDL3_image_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/SDL3_image-3.2.4/x86_64-w64-mingw32/include")
    set(SDL3_ttf_DIR "${CMAKE_SOURCE_DIR}/lib/SDL3_ttf-3.2.2/x86_64-w64-mingw32/lib/cmake/SDL3_ttf")
    set(SDL3_ttf_DLL_DIR "${CMAKE_SOURCE_DIR}/lib/SDL3_ttf-3.2.2/x86_64-w64-mingw32/bin")
    set(SDL3_ttf_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/SDL3_ttf-3.2.2/x86_64-w64-mingw32/include")

    # OpenAL Soft 경로 설정 (Windows)
    set(OPENAL_DIR "${CMAKE_SOURCE_DIR}/lib/openal-soft-1.24.3-bin")



if(FLATBUFFERS_INCLUDE_PARENT)
    include_directories(SYSTEM "${FLATBUFFERS_INCLUDE_PARENT}")
    list(APPEND CMAKE_PREFIX_PATH "${FLATBUFFERS_INCLUDE_PARENT}")
    install(DIRECTORY "${FLATBUFFERS_INCLUDE_PARENT}/" DESTINATION include FILES_MATCHING PATTERN "*.h*")
endif()
    
    # SDL 패키지 찾기 (Windows)
    find_package(SDL3 CONFIG REQUIRED)
    find_package(SDL3_image CONFIG REQUIRED)
    find_package(SDL3_ttf CONFIG REQUIRED)

    # Copy dependency DLLs to the runtime output directory for development builds.
    file(COPY
        "${SDL3_DLL_DIR}/SDL3.dll"
        "${SDL3_image_DLL_DIR}/SDL3_image.dll"
        "${SDL3_ttf_DLL_DIR}/SDL3_ttf.dll"
        DESTINATION "${CMAKE_BINARY_DIR}/bin/Debug"
    )
    file(COPY "${OPENAL_DIR}/bin/Win64/soft_oal.dll" DESTINATION "${CMAKE_BINARY_DIR}/bin/Debug")
    file(RENAME "${CMAKE_BINARY_DIR}/bin/Debug/soft_oal.dll" "${CMAKE_BINARY_DIR}/bin/Debug/OpenAL32.dll")

    # Install dependency DLLs.
    install(FILES
        "${SDL3_DLL_DIR}/SDL3.dll"
        "${SDL3_image_DLL_DIR}/SDL3_image.dll"
        "${SDL3_ttf_DLL_DIR}/SDL3_ttf.dll"
        DESTINATION bin
    )
    install(FILES "${OPENAL_DIR}/bin/Win64/soft_oal.dll" DESTINATION bin RENAME "OpenAL32.dll")
    

elseif(UNIX)
    # SDL 경로 설정 (Linux) - 프로젝트 내부 라이브러리 사용 가정
    # Linux 환경에서는 .cmake 파일이 lib/SDL3-3.2.18/lib/cmake/SDL3 에 있을 것으로 가정합니다.
    set(SDL3_DIR "${CMAKE_SOURCE_DIR}/include/lib/SDL3-3.2.18/lib/cmake/SDL3")
    set(SDL3_image_DIR "${CMAKE_SOURCE_DIR}/include/lib/SDL3_image-3.2.4/lib/cmake/SDL3_image")
    set(SDL3_ttf_DIR "${CMAKE_SOURCE_DIR}/include/lib/SDL3_ttf-3.2.2/lib/cmake/SDL3_ttf")

    # CMake가 라이브러리를 찾을 경로를 지정합니다.
    list(APPEND CMAKE_PREFIX_PATH 
    "${CMAKE_SOURCE_DIR}/lib/SDL3-3.2.18/"
    "${CMAKE_SOURCE_DIR}/lib/SDL3_image-3.2.4/"
    "${CMAKE_SOURCE_DIR}/lib/SDL3_ttf-3.2.2/"
    "${CMAKE_SOURCE_DIR}/lib/openal-soft-1.24.3-bin/"
    )

    # OpenAL Soft 경로 설정 (Linux) - 프로젝트 내부 라이브러리 사용 가정
    set(OPENAL_DIR "${CMAKE_SOURCE_DIR}/lib/openal-soft-1.24.3-bin")

    # SDL 패키지 찾기 (Linux)
    find_package(SDL3 CONFIG REQUIRED)
    find_package(SDL3_image CONFIG REQUIRED)
    find_package(SDL3_ttf CONFIG REQUIRED)

    # OpenAL Soft (Linux)
    find_package(OpenAL CONFIG REQUIRED)
endif()

# FlatBuffers (cross-platform)
set(FLATBUFFERS_DIR "${CMAKE_SOURCE_DIR}/include")
if(EXISTS "${FLATBUFFERS_DIR}")
# Make headers available to all targets
include_directories(SYSTEM "${FLATBUFFERS_DIR}")
# Help CMake find packages that might look under this prefix
list(APPEND CMAKE_PREFIX_PATH "${FLATBUFFERS_DIR}")
# Install headers for `cmake --install`
install(DIRECTORY "${FLATBUFFERS_DIR}/" DESTINATION include FILES_MATCHING PATTERN "*.h*")
endif()

# Box2D (Cross-platform)
add_subdirectory(include/box2d-3.1.1)

# Create a directory for generated files to keep the build directory clean.
set(GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)

# Generate GNEngineRootPath.h from the .in template for use in the source code.
configure_file(
    "${CMAKE_SOURCE_DIR}/include/GNEngine/GNEngineRootPath.h.in"
    "${GENERATED_DIR}/GNEngineRootPath.h"
    @ONLY
    )
    
    # Add the GNEngine library from its own directory
    add_subdirectory(include)
    
    # Add the example project
    add_subdirectory(example)