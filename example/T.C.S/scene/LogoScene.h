#pragma once
#include "GNEngine/GNEngine_API.h"
#include "GNEngine/core/Scene.h"

// 필요한 Manager들의 전방 선언
class EntityManager;
class EventManager;
class RenderManager;
class TextureManager;
class SoundManager;
class AnimationManager;
class FadeManager;
class SceneManager;

#include <SDL3/SDL_render.h>

#include "GNEngine/core/Entity.h"
#include "GNEngine/component/TransformComponent.h"
#include "GNEngine/component/TextComponent.h"
#include "GNEngine/component/RenderComponent.h"

/*
 * @brief LogoScene
 * 
 * 이 생성자는 로고 씬을 초기화하며, 엔티티, 이벤트, 렌더링, 사운드, 텍스처, 애니메이션, 페이드 관련 매니저들의 참조를 받아
 * 씬에서 사용할 수 있도록 저장함.
 * 
 * @param entityManager 엔티티 관리 매니저 참조
 * @param eventManager 이벤트 관리 매니저 참조
 * @param renderManager 렌더링 관리 매니저 참조
 * @param soundManager 사운드 관리 매니저 참조
 * @param textureManager 텍스처 관리 매니저 참조
 * @param animationManager 애니메이션 관리 매니저 참조
 * @param fadeManager 페이드 효과 관리 매니저 참조
*/
class LogoScene : public Scene {
public: 
    LogoScene(EntityManager& entityManager,
            SceneManager& sceneManager,
            EventManager& eventManager, 
            RenderManager& renderManager, 
            SoundManager& soundManager, 
            TextureManager& textureManager, 
            AnimationManager& animationManager,
            FadeManager& fadeManager);
    ~LogoScene() override = default;

    void onEnter() override;
    void onExit() override;
    bool loadScene() override;
    void update(float deltaTime) override;
    

private:
std::vector<EntityID> sceneEntityIDs_;
    EntityID logoEntity_;

    float sceneTimer_ = 0.0f; /* 페이드인이 끝난 직후부터 시간을 기록함. */

    /* --- 표시 시간 --- */
    float screenTime_ = 2.0f;
    float fadeInTime_ = 2.0f;
    float fadeOutTime_ = 5.0f;

    enum class LogoSceneState {
        FADING_IN,
        DISPLAYING,
        FADING_OUT
    };
    LogoSceneState currentState_ = LogoSceneState::FADING_IN;

    EntityManager& entityManager_;
    SceneManager& sceneManager_;
    EventManager& eventManager_;
    RenderManager& renderManager_;
    TextureManager& textureManager_;
    SoundManager& soundManager_;
    AnimationManager& animationManager_;
    FadeManager& fadeManager_;
};


