cmake_minimum_required(VERSION 3.25)

# 애플리케이션 소스 파일
set(T.C.S_SOURCES
    main.cpp
    Application.cpp

    # -- Scene --
    scene/LogoScene.cpp
    scene/MainMenuScene.cpp
    scene/InGame.cpp

    # -- Prefab --
    prefab/PlayerPrefab.cpp
)

# 애플리케이션 실행 파일 정의
add_executable(T.C.S ${T.C.S_SOURCES})

# 애플리케이션 인클루드 디렉터리 설정
target_include_directories(T.C.S PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# AppRootPath.h.in으로 AppRootPath.h 생성. 소스에서 프로젝트 경로 지정용.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/AppRootPath.h.in ${CMAKE_BINARY_DIR}/generated/AppRootPath.h @ONLY)

# 애플리케이션 링크 라이브러리 설정
target_link_libraries(T.C.S PRIVATE GNEngine)

if(EMSCRIPTEN)
    target_link_options(T.C.S PRIVATE
        "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/asset@/asset"
        "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data@/data"
        "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/config.bin@/config.bin"
        "-sEXIT_RUNTIME=1"
    )
endif()

# AppRootPath.h가 생성되는 디렉토리를 T.C.S의 include 경로에 추가
target_include_directories(T.C.S PRIVATE ${CMAKE_BINARY_DIR}/generated)
