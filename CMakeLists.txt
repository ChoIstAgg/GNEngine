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


set(THIRD_PARTY_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib") 

# SDL 및 OpenAL 패키지 찾기
if(EMSCRIPTEN)
    message(STATUS "Building for WebAssembly (Emscripten)...")

    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

    # Provide dummy SDL3 targets to skip SDL3_image's find_package(SDL3)
    if(NOT TARGET SDL3::SDL3)
        add_library(SDL3::SDL3 INTERFACE IMPORTED)
        set_target_properties(SDL3::SDL3 PROPERTIES SDL_FULL_VERSION "3.4.2")
    endif()
    if(NOT TARGET SDL3::Headers)
        add_library(SDL3::Headers INTERFACE IMPORTED)
    endif()

    # SDL3_image build from source (since port is missing)
    set(SDL3IMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
    set(SDL3IMAGE_TESTS OFF CACHE BOOL "" FORCE)
    add_subdirectory(include/SDL_image EXCLUDE_FROM_ALL)

    # Emscripten 내장 포트 플래그 설정 (SDL3 계열 및 OpenAL)
    add_compile_options(
        "-sUSE_SDL=3" 
        "-sUSE_SDL_TTF=3"
    )
    add_link_options(
        "-sUSE_SDL=3" 
        "-sUSE_SDL_TTF=3"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sMAX_WEBGL_VERSION=2"
    )

    # SDL3_image::SDL3_image is provided by add_subdirectory(include/SDL_image)
    if(NOT TARGET SDL3_ttf::SDL3_ttf)
        add_library(SDL3_ttf::SDL3_ttf INTERFACE IMPORTED)
    endif()
    if(NOT TARGET OpenAL::OpenAL)
        add_library(OpenAL::OpenAL INTERFACE IMPORTED)
        target_link_libraries(OpenAL::OpenAL INTERFACE openal)
    endif()
elseif(WIN32)
	# SDL 경로 설정 
	set(SDL3_DIR "${CMAKE_SOURCE_DIR}/lib/SDL3-3.2.18/x86_64-w64-mingw32/lib/cmake/SDL3")
	set(SDL3_DLL_DIR "${CMAKE_SOURCE_DIR}/lib/SDL3-3.2.18/x86_64-w64-mingw32/bin")
	set(SDL3_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/SDL3-3.2.18/x86_64-w64-mingw32/include")
	set(SDL3_image_DIR "${CMAKE_SOURCE_DIR}/lib/SDL3_image-3.2.4/x86_64-w64-mingw32/lib/cmake/SDL3_image")
	set(SDL3_image_DLL_DIR "${CMAKE_SOURCE_DIR}/lib/SDL3_image-3.2.4/x86_64-w64-mingw32/bin")
	set(SDL3_image_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/SDL3_image-3.2.4/x86_64-w64-mingw32/include")
	set(SDL3_ttf_DIR "${CMAKE_SOURCE_DIR}/lib/SDL3_ttf-3.2.2/x86_64-w64-mingw32/lib/cmake/SDL3_ttf")
	set(SDL3_ttf_DLL_DIR "${CMAKE_SOURCE_DIR}/lib/SDL3_ttf-3.2.2/x86_64-w64-mingw32/bin")
	set(SDL3_ttf_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/SDL3_ttf-3.2.2/x86_64-w64-mingw32/include")
	
	find_package(SDL3 CONFIG REQUIRED)
	find_package(SDL3_image CONFIG REQUIRED)
	find_package(SDL3_ttf CONFIG REQUIRED)

    # OpenAL Soft 경로 설정 (Windows)
    set(OPENAL_DIR "${CMAKE_SOURCE_DIR}/lib/openal-soft-1.24.3-bin")
    set(OPENAL_INCLUDE_DIR "${OPENAL_DIR}/include")
    set(OPENAL_LIBRARY "${OPENAL_DIR}/libs/Win64/OpenAL32.lib")
    
    add_library(OpenAL::OpenAL UNKNOWN IMPORTED)
    set_target_properties(OpenAL::OpenAL PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${OPENAL_INCLUDE_DIR}"
        IMPORTED_LOCATION "${OPENAL_LIBRARY}"
    )
else() # Linux or other Unix-like
	find_package(SDL3 REQUIRED)
	find_package(SDL3_image REQUIRED)
	find_package(SDL3_ttf REQUIRED)
    find_package(OpenAL REQUIRED)
endif()

# MinGW 런타임 경로 설정
set(MINGW_BIN_DIR "C:/Development_Environment/code/MSYS2/ucrt64/bin")


# Box2D
add_subdirectory(lib/box2d-3.1.1)

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

if(NOT EMSCRIPTEN)
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
endif()
