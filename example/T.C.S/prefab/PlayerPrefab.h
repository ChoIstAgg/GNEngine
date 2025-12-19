#pragma once
#include "../GNEngine_API.h"

#include "GNEngine/core/Entity.h"

// 순환참조 방지용 전방선언
class EntityManager;
class EventManager;
class TextureManager;
class RenderManager;
class SoundManager;
class AnimationManager;

/*
* @class PlayerFactory
* @brief 플레이어 엔티티 생성을 위한 정적 팩토리 클래스임.
*
* 이 클래스는 플레이어 캐릭터에 필요한 모든 컴포넌트를 조합하여
* 하나의 완전한 엔티티를 생성하는 역할을 함.
* 객체를 생성하지 않고 정적 함수 create()를 통해 플레이어를 생성함.
*/
class PlayerPrefab {
public:
    /*
    * @brief 플레이어 엔티티를 생성하고 필요한 모든 컴포넌트를 추가함.
    * @param entityManager
    * @param eventManager
    * @param textureManager
    * @param renderManager
    * @param soundManager
    * @param animationManager
    * @return 생성된 플레이어의 EntityId
    */
    static EntityId create(
        EntityManager& entityManager, 
        EventManager& eventManager, 
        TextureManager& textureManager, 
        RenderManager& renderManager, 
        SoundManager& soundManager, 
        AnimationManager& animationManager
    );
};




