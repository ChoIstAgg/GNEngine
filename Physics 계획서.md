- 목표: 이 GNEngine의 컴포넌트와 box2d를 통합하여 GNEngine만으로도 물리를 쉽게 사용하게 함. 최소한의 성능을 요구함.


- 주의할 점
  - 현재 아직 렌더링 프레임과 연산 프레임이 분리되지 않아 한 프레임에 렌더링과 물리 연산을 같이 하지만 추후 프레임을 분리할 예정임.


### PhysicsManager
전역 물리 설정을 함.
- 중력가속도(9.8m/s)
- 픽셀 미터 매칭

### PhysicsSystem
물리 로직을 업데이트.
b2World::Step()로 물리 연산을 수행하고
물리의 영향을 받으면 TransformComponent의 position을 업데이트함.
그밖의 추가적인 물리 연산도 수행. (진공 우주 등 기능 수행 가능.)



## 강체 컴포넌트
### RigidBodyComponent
강체 바디의 속성 데이터
PhysicsManager::updateRigidBodyState()로 업데이트함 (b2BodyId를 받아 해당 Id의 정보를 이 컴포넌트의 데이터에 덮어씌움.)

- b2BodyId
- b2bodyDef는 바디 생성시 필요한 초기 정보임. 대신 아래의 변수들을 갖고있음
	- b2vec2 position
	- float angle
  	- b2vec2 linearVelocity
	- float angularVelocity
	- float linearDamping
	- float angularDamping
	- bool allowSleep
	- bool awake
	- bool fixedRotation
	- bool bullet
	- bool enabled
	- b2BodyUserData userData
	- float gravityScale

### FixtureComponent

- b2Fixture : 물리적 속성. PhysicsManager::CreateFixture로 생성.
	- shape
	- dense
	- friction
	- restitution
	- category
	- proxyCount
	- filter
	- isSensor
	- userData

## 유체
유체 렌더링은 추후 지원 예정. 현재는 강체에 집중하자. 

### FluidBodyComponent
유체
- particle count 입자수 : 많을 수록 성능 저하.
- b2ParticleSystemDef 입자의 특성
	- radius 입자 반경 : 작으면 세세하지만 많은 입자가 필요. 
	- density 밀도 : 높을수록 무겁고 강체에 작용시키는 힘이 증가.
	- gravityScale 중력 스케일 : 유체에 적용되는 중력의 배율
	- dampingStrength 감쇠 강도 : 유체 입자의 속도를 감쇠하는 배율. 높을수록 빨리 안정됨. 
	- powderStrength 분말
	- tensileStrength 인장력
	- viscousStrength 점성 
	- surfaceTensionStrength 표면 장력
	- 등등

##### 유체 렌더링 방법
- 유체의 입자에 SDL로 원이나 사각형 
- 원형 텍스처를 미리 렌더링하여 유체의 입자에 매칭시킴. 
  렌더링 비용 상대적으로 저렴.
- 메타볼 : 매우 사실적인 유체 구현. 하지만 매우 큰 성능 저하 예상.
  SDL_RenderGeometry로 렌더링함.