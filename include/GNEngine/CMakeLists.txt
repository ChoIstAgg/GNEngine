# GNEngine 라이브러리 정의
if(EMSCRIPTEN)
    add_library(GNEngine STATIC
        # Include All source files.
        ${PROJECT_SOURCE_DIR}/src/GNEngine/core/ComponentArray.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/core/Animation.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/core/Sound.cpp

        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/FileManager.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/EntityManager.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/SystemManager.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/RenderManager.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/InputManager.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/SceneManager.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/SoundManager.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/AnimationManager.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/TextManager.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/TextureManager.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/FadeManager.cpp
        
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/RenderSystem.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/SoundSystem.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/InputSystem.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/CameraSystem.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/AnimationSystem.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/TextSystem.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/MovementSystem.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/FadeSystem.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/InputToAccelerationSystem.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/PlayerAnimationControlSystem.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/AccelerationResetSystem.cpp

        ${PROJECT_SOURCE_DIR}/src/GNEngine/component/AnimationComponent.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/component/SoundComponent.cpp
    )
else()
    add_library(GNEngine SHARED
        # Include All source files.
        ${PROJECT_SOURCE_DIR}/src/GNEngine/core/ComponentArray.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/core/Animation.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/core/Sound.cpp

        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/FileManager.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/EntityManager.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/SystemManager.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/RenderManager.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/InputManager.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/SceneManager.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/SoundManager.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/AnimationManager.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/TextManager.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/TextureManager.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/manager/FadeManager.cpp
        
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/RenderSystem.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/SoundSystem.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/InputSystem.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/CameraSystem.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/AnimationSystem.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/TextSystem.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/MovementSystem.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/FadeSystem.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/InputToAccelerationSystem.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/PlayerAnimationControlSystem.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/system/AccelerationResetSystem.cpp

        ${PROJECT_SOURCE_DIR}/src/GNEngine/component/AnimationComponent.cpp
        ${PROJECT_SOURCE_DIR}/src/GNEngine/component/SoundComponent.cpp
    )
endif()

# GNEngine 라이브러리 인클루드 디렉터리 설정
target_include_directories(GNEngine PUBLIC
    "${GENERATED_DIR}"
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/lib"
    "${PROJECT_SOURCE_DIR}/lib/dr_libs"
    "${PROJECT_SOURCE_DIR}/lib/nlohmann"
    "${PROJECT_SOURCE_DIR}/lib/box2d-3.1.1/include"
    
    "${PROJECT_SOURCE_DIR}/include/GNEngine/flatbuffers_generated"
    "${PROJECT_SOURCE_DIR}/src/GNEngine/component"
    "${PROJECT_SOURCE_DIR}/src/GNEngine/core"
    "${PROJECT_SOURCE_DIR}/src/GNEngine/manager"
    "${PROJECT_SOURCE_DIR}/src/GNEngine/prefab"
    "${PROJECT_SOURCE_DIR}/src/GNEngine/resource"
    "${PROJECT_SOURCE_DIR}/src/GNEngine/system"
)

# SDL3, SDL3_image, SDL3_ttf 인클루드 경로 추가 (target_link_libraries가 자동으로 처리하지 못할 경우를 대비)
if(WIN32)
    target_include_directories(GNEngine PUBLIC
        "${SDL3_INCLUDE_DIR}"
        "${SDL3_image_INCLUDE_DIR}"
        "${SDL3_ttf_INCLUDE_DIR}"
    )
endif()

# GNEngine 라이브러리 링크 설정
target_link_libraries(GNEngine PUBLIC
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    OpenAL::OpenAL
    box2d
)

if(NOT MSVC AND NOT EMSCRIPTEN)
    target_link_libraries(GNEngine PUBLIC stdc++fs)
endif()

# GNEngine_EXPORTS 정의 (DLL 빌드 시 필요)
target_compile_definitions(GNEngine PRIVATE GNEngine_EXPORTS)

# Force include <cstring> for compatibility with some libraries
target_compile_options(GNEngine PRIVATE -include cstring)

# Install the GNEngine library target and its headers.
install(TARGETS GNEngine
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/"
    DESTINATION include/GNEngine
    FILES_MATCHING PATTERN "*.h"
)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/lib/nlohmann" DESTINATION include)
# Install flatbuffers headers individually
file(GLOB FLATBUFFERS_HEADERS "${PROJECT_SOURCE_DIR}/lib/flatbuffers/include/flatbuffers/*.h")
install(FILES ${FLATBUFFERS_HEADERS} DESTINATION include/flatbuffers)
