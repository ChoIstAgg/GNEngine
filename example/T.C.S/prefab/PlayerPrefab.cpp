#include "PlayerPrefab.h"

#include "AppRootPath.h"

#include <SDL3/SDL_render.h>

#include "GNEngine/manager/EntityManager.h"
#include "GNEngine/manager/EventManager.h"
#include "GNEngine/manager/TextureManager.h"
#include "GNEngine/manager/RenderManager.h"
#include "GNEngine/manager/SoundManager.h"
#include "GNEngine/manager/AnimationManager.h"

#include "GNEngine/component/TransformComponent.h"
#include "GNEngine/component/VelocityComponent.h"
#include "GNEngine/component/AccelerationComponent.h"
#include "GNEngine/component/RenderComponent.h"
#include "GNEngine/component/SoundComponent.h"
#include "GNEngine/component/PlayerAnimationControllerComponent.h"
#include "GNEngine/component/PlayerMovementComponent.h"
#include "GNEngine/component/InputControlComponent.h"

#include <iostream>
#include <filesystem>

/*
* @brief 플레이어 엔티티를 생성하고 필요한 모든 컴포넌트를 추가함.
* @return 생성된 플레이어의 EntityId
*/
EntityId PlayerPrefab::create(EntityManager& entityManager, EventManager& eventManager, TextureManager& textureManager, RenderManager& renderManager, SoundManager& soundManager, AnimationManager& animationManager) {
    EntityId entityId = entityManager.createEntity();

    // 엔티티 생성 및 컴포넌트 추가
    entityManager.addComponent<TransformComponent>(entityId, 0.0f, 0.0f);
    entityManager.addComponent<VelocityComponent>(entityId, 0.0f, 0.0f);
    entityManager.addComponent<AccelerationComponent>(entityId, 0.0f, 0.0f);

    // PlayerAnimationControllerComponent 추가
    // 애니메이션 데이터 로드
    // T.C.json 파일 하나로 모든 애니메이션 로드
    std::filesystem::path tcAnimationJsonPath = std::filesystem::path(ANIMATION_SHEET_ASSET_ROOT_PATH) / "T.C/" / "T.C.json";
    if (!animationManager.loadAnimation(tcAnimationJsonPath)) {
        std::cerr << "Error: Failed to load animation JSON: " << tcAnimationJsonPath << std::endl;
    }
    
    std::shared_ptr<Animation> tcIdleAnimationData = animationManager.getAnimation("idle");
    std::shared_ptr<Animation> tcWalkAnimationData = animationManager.getAnimation("walk");
    std::shared_ptr<Animation> tcJumpAnimationData = animationManager.getAnimation("jump");
    animationManager.setScaleModeOfAnimation("idle", SDL_SCALEMODE_NEAREST);
    animationManager.setScaleModeOfAnimation("walk", SDL_SCALEMODE_NEAREST);
    animationManager.setScaleModeOfAnimation("jump", SDL_SCALEMODE_NEAREST);

    // RenderComponent 추가
    // 애니메이션 데이터에서 텍스처 경로를 가져와 TextureManager를 통해 로드
    if (tcWalkAnimationData) {
        std::filesystem::path texturePath = tcWalkAnimationData->getTexturePath();
        if (!textureManager.loadTexture(texturePath)) {
            std::cerr << "Error: Failed to load texture for animation: " << texturePath << std::endl;
        }
        Texture* animationTexture = textureManager.getTexture(texturePath);
        if (animationTexture) {
            SDL_Rect frame = tcWalkAnimationData->getFrame(0);
            entityManager.addComponent<RenderComponent>(entityId, animationTexture->sdlTexture_, RenderLayer::CRUCIAL_GAME_OBJECT, false, true, frame.w, frame.h, frame, false, false);
        } else {
            std::cerr << "Error: Animation texture is null for: " << texturePath << std::endl;
        }
    } else {
        std::cerr << "Error: Walk animation data is null. Cannot add RenderComponent." << std::endl;
    }

    entityManager.addComponent<PlayerAnimationControllerComponent>(entityId, tcWalkAnimationData, tcJumpAnimationData, tcIdleAnimationData);

    // AnimationComponent 추가: PlayerAnimationControllerComponent에서 사용할 기본 애니메이션으로 초기화
    entityManager.addComponent<AnimationComponent>(entityId, tcWalkAnimationData);

    // SoundComponent 추가
    std::filesystem::path soundPath = std::filesystem::path(SOUND_EFFECT_ASSET_ROOT_PATH) / "hit01.flac";
    std::shared_ptr<Sound> hitSound = soundManager.getSound(soundPath);
    if (hitSound) {
        auto& soundComp = entityManager.addComponent<SoundComponent>(entityId); // addComponent가 참조를 반환
        soundComp.addSound("hit", hitSound, false, 1.0f); // addSound 호출
    }

    // PlayerMovementComponent 제거 (PlayerInputComponent와 MovementSystem이 대체)
    // entityManager.removeComponent<PlayerMovementComponent>(entityId); // 이전에 추가된 경우 제거

    // InputControlComponent 추가 및 키 매핑 설정
    auto& inputControl = entityManager.addComponent<InputControlComponent>(entityId);
    inputControl.keyActions[SDL_SCANCODE_W] = "move_forward";
    inputControl.keyActions[SDL_SCANCODE_S] = "move_backward";
    inputControl.keyActions[SDL_SCANCODE_A] = "move_left";
    inputControl.keyActions[SDL_SCANCODE_D] = "move_right";

    return entityId;
}