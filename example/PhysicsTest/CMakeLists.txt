cmake_minimum_required(VERSION 3.16)

set(EXECUTABLE_NAME PhysicsTest)
project(${EXECUTABLE_NAME} CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GNEngine 라이브러리의 경로 (상대 경로)


# 실행 파일에 포함될 소스 파일 목록
set(SOURCES
    main.cpp
    Application.cpp
    TestScene.cpp
)

# 실행 파일 생성
add_executable(${EXECUTABLE_NAME} ${SOURCES})

# p_RootPath.h.in으로 p_RootPath.h 생성. 소스에서 프로젝트 경로 지정용.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/p_RootPath.h.in ${CMAKE_BINARY_DIR}/generated/p_RootPath.h @ONLY)

# GNEngine 라이브러리 링크
target_link_libraries(${EXECUTABLE_NAME} PRIVATE GNEngine)

# GNEngine에 대한 명시적 의존성 추가
add_dependencies(${EXECUTABLE_NAME} GNEngine)

# GNEngine 헤더 파일 인클루드
target_include_directories(${EXECUTABLE_NAME} PRIVATE 
    ../../include
)
