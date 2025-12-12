# GNEngine Project Configuration for Gemini

## 1. 프로젝트 개요 (Project Overview)

- **프로젝트명:** GNEngine
- **목표:** Modern C++과 SDL3를 이용한 2D 게임 엔진 프레임워크 개발.
- **사용 패러다임:** DOD(Data Oriented Design) + ECS(Entity Component System)
- 현재 DOD 구조를 사용하고 있다는 것을 꼭 기억해줘.

## 2. 빌드 및 실행 (Build & Run)
- 현재 운영체재는 Windows 11 Home이고 현재 GEMINI CLI는 VSCODE의 터미널에서 실행중이야.

이 프로젝트는 CMake와 `CMakePresets.json`을 사용하여 빌드합니다. g++ 컴파일러와 Ninja 빌드 시스템을 사용합니다.
- 현재 환경: vscode

1.  **CMake 설정 (Configure):**
    `CMakePresets.json`에 정의된 "Custom configure preset"을 사용합니다.
    ```bash
    cmake --preset custom-preset
    ```

2.  **빌드 (Build):**
    ```bash
    cmake --build out/build/custom-preset
    ```

3.  **실행 (Run):**
    빌드가 완료되면 실행 파일은 `out/build/custom-preset/Debug/` 디렉터리에 생성됩니다.
    ```bash
    out/build/custom-preset/Debug/{실행할항목}.exe
    ```

## 3. 의존성 (Dependencies)

- SDL3(3.2.18)
- SDL3_image
- SDL3_ttf
- OpenAL soft
- dr_wav
- dr_mp3
- dr_flac
- stb_vorbis
- nlohmann/json
- flatBuffers
- Box2d(3.1.1)

의존성 라이브러리들은 `include/lib/` 디렉터리에서 관리됩니다.

## 4. 디렉터리 구조 (Directory Structure)
- 각 파일의 자세한 역할은 ./Engine Struct.md 파일 참조..

- `include/GNEngine/`: GNEngine 라이브러리 헤더.
  - `manager/`
  - `component/`
  - `system/`
  - `resource/` : 엔진의 범용 에셋. 
    - `image/`: 이미지가 헤더파일로 존재함. TextManager::loadTextureEmbedded로 로딩하기. 
- `out/build/`: 빌드 결과물이 저장되는 디렉터리
- `example/`: 예제 프로젝트들. 
  - `top_view_game`
  - `snake_game`

## 5. 소스 파일들의 역할과 구조
- System : 모든 직접적인 로직을 담당함.
- Manager : 리소스 관리와 초기화 등 유틸리티 기능을 담당하는 관리자.
- Component : 각 Entity가 기능을 사용하기 위해 갖는 객체.
- Entity : EntityManager를 통해 Component를 등록할 수 있음. ID만 저장 및 사용.
- prefab : Entity를 생성하는 빌드 템플릿.

## 6. 코딩 스타일 및 규칙 (Coding Style & Conventions)

- 프로젝트 전반에 걸쳐 일관된 코딩 스타일을 유지합니다.
- 새로운 코드를 작성할 때는 주변 코드의 명명 규칙(naming convention), 포맷팅 등을 따라주세요.

### 명명 규칙:
- 변수 명: Camel Case 
- 함수 명: Camel Case
- 클래스: Pascal Case
- 타입 명: Pascal Case
- 상수: Screaming Snake Case

### 그 외 코드 스타일 규칙
- 중괄호 배치는 K&R을 사용합니다.
- 다만 한 줄이 너무 길다고 판단될때에는 Allman 스타일을 사용합니다.

- lambda 객체는 간단하게 한 줄로 작성할 수 있다고 판단될 때(대부분의 경우)에는 아래의 형식대로 작성합니다. (람다 전후로 띄어쓰기에 집중해줘!!)
    function( []() { return 1; } )
- 한 줄로 작성하기 힘들다고 판단될 때에는에는 아래의 형식대로 작성합니다.
    function( []() {
        /* 
         * 무수한 코드들...
        */
    }; )


### 주석
- 주석 표기는 대부분 /* */를 사용해줘.
- 주석은 되도록이면 .h 헤더 파일 말고도 .cpp 구현 파일에도 작성해줘.
- 한글 주석을 사용할 때에는 "입니다." 대신 "." "~임.", "~함.", "~하기"를 사용해줘
- 
- 모든 클래스의 주석에는 생성자의 파라미터를 같이 작성한다. 
  - 작성 양식은 "@param 변수이름(변수 기본값) 설명"
  - 변수 기본값은 있을 경우에만 작성한다.
  - 변수 기본값이 컨테이너 형식으로 되어있을 경우 쉼표 대신 하이픈을 사용한다.
  - example:  
    - @class RenderComponent
    - @brief 주어진 텍스처를 렌더링하기 위한 데이터.
    - @param Texture 렌더링할 텍스처 데이터.
    - @param srcRect({0-0-0-0}) 텍스처의그럼 흠 비 사각형 영역.
    - @param flipX(false) x축으로 뒤집을지 여부.

## 7. 그 밖의 규칙
- 항상 필요한 것만 바꿔줘. WriteFile 명령어는 항상 새 파일을 생성하고 작성할 때에만 사용해줘. 기존 파일에 WriteFile 명령어를 사용하면 기존 내용이 파괴되어버려. 
- 나는 Modern C++(23까지)와 게임 엔진을 배우고자 하는 열렬한 학습자야.
- 나는 코드의 작동 원리를 알고싶어. 반드시 코드를 작성해준 후 코드의 작동 원리를 세세하게 명시해줘.
- 조금 시간이 더 걸리더라도 정확한 정보를 가져오고, 더 효율적인 코드를 작성해줘.
  
- 나는 항상 컴파일 성능 보다는 '런타임 성능'을 중시해. 되도록이면 컴파일 타임 연산과 상수를 사용했으면 좋겠어. (하지만 런타임으로 돌아가는 어플리케이션이므로 런타임 연산이 필요할 때에는 런타임 연산을 해야해.)
- 실시간 어플리케이션인 만큼 불필요한 연산과 오버헤드를 최대한 줄이고 싶어.
- DOD 구조를 사용중이니 캐시 친화성이 매우 중요해. 특별한 상황이 아니라면 std::vector이나 std::array, std::deque를 사용해줘. 

- 아부는 하지 말아줘. 나는 매우 현실적인 사람이야.

## 고려해야할 사항
- 추후 멀티스레드를 도입할거야. 멀티스레드 충돌을 방지하기 위해 구조를 미리 잘 작성해야해. 
