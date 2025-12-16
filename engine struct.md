- 미안해. 아직 다 작성하지 않았어. 직접 디렉터리 구조를 파악해줘.

- core/ : 앱의 핵심 데이터
  - ComponentArray: SoA 설계를 구현하기 위해 컴포넌트.
  각 컴포넌트 타입마다 템플릿 특수화로 SoA를 구현해야함.
- system/: 직접적인 연산을 수행하는 노동자.
  - InputSystem : 모든 InputControlComponent를 순회하여 매핑된 이벤트를 발생시킴.
  - SoundSystem : 모든 SoundComponent를 순회하며 재생 큐에 있는 
  소리를 재생하고 스피커 위치를 업데이트함.
  - MovementSystem : 
- manager/: System, Entity, 각 요소들을 모두 관리하는 총 책임자. 싱글톤 패턴이 적용된 GNManager로 인스턴스를 받아 사용함.
  - SystemManager : 각 System의 연산을 PRE_UPDATE, LOGIC_UPDATE, PHYSICS_UPDATE, POST_UPDATE, RENDER 5가지로 나눠 순차적으로 수행함. 
  - FileManager : ※따로 독자적으로 Singleton pattern 사용중.※ flatBuffer 라이브러리를 활용하여 모든 소스에서 로그 관리와 바이너리 읽기, 쓰기 작업을 수행함.
  - SoundManager : OpenAL과 다른 디코딩 라이브러리를 활용하여 
- component/: System이 연산을 수행하기 위해 필요한 데이터.
- prefab/: Entity를 만드는 하나의 프리셋.
- resource/: Sound, Text 등 리소스 자원 클래스. 직접적인 SDL 변수 대신  리소스를 사용할 때 모두 이 클래스를 사용함.
- RootPath.h.in: Asset 등 외부 파일을 사용하기 위해 프로젝트의 절대 경로를 정의함. CMake가 자동으로 사용자의 프로젝트 절대경로를 사용함. 변경할 필요 없음. 
- 