#include "GNEngine/system/PlayerAnimationControlSystem.h"

#include <iostream>
#include <filesystem>
#include <cmath>

#include "GNEngine/core/RenderLayer.h"
#include "GNEngine/component/AccelerationComponent.h"
#include "GNEngine/component/RenderComponent.h"
#include "GNEngine/component/TransformComponent.h"

PlayerAnimationControlSystem::PlayerAnimationControlSystem(
    AnimationManager& animationManager,
    TextureManager& textureManager,
    RenderManager& renderManager
)
    : animationManager_(animationManager),
      textureManager_(textureManager),
      renderManager_(renderManager)
{}

void PlayerAnimationControlSystem::update(EntityManager& entityManager, float deltaTime) {
    //SDL_Log("PlayerAnimationControlSystem: update called. DeltaTime: %.4f", deltaTime);
    for (EntityId entity : entityManager.getEntitiesWith<PlayerAnimationControllerComponent, AnimationComponent, VelocityComponent, TransformComponent, AccelerationComponent>()) {
        auto animController_opt = entityManager.getComponent<PlayerAnimationControllerComponent>(entity);
        auto animation_opt = entityManager.getComponent<AnimationComponent>(entity);
        auto velocity_opt = entityManager.getComponent<VelocityComponent>(entity);
        auto acceleration_opt = entityManager.getComponent<AccelerationComponent>(entity);

        if (!animController_opt || !animation_opt || !velocity_opt || !acceleration_opt) { continue; }

        auto& animController = animController_opt.value();
        auto& animation = animation_opt.value();
        auto& velocity = velocity_opt.value();
        auto& acceleration = acceleration_opt.value();

        //SDL_Log("PlayerAnimationControlSystem: Entity %u - Vel(%.2f, %.2f)", entity, velocity.vx, velocity.vy);

        // 이동 상태에 따른 애니메이션 전환 로직
        if (std::abs(velocity.vx) > 0.1f || std::abs(velocity.vy) > 0.1f) {
            // 움직이고 있다면 wal 애니메이션 재생
            if (animation.getAnimation() != animController.walkAnimationData_) {
                //SDL_Log("PlayerAnimationControlSystem: Switching to walk animation for entity %u.", entity);
                setCurrentAnimation(entityManager, entity, animController.walkAnimationData_);
            }
            if (!animation.isPlaying()) {
                //SDL_Log("PlayerAnimationControlSystem: Playing walk animation for entity %u.", entity);
                // animation.play(); // SoA 컴포넌트의 복사본에 대한 호출은 실제 데이터에 영향 없음
            }
        } else {
            // 멈춰 있다면 idle 애니메이션 재생
            if (animation.getAnimation() != animController.idleAnimationData_) {
                //SDL_Log("PlayerAnimationControlSystem: Switching to idle animation for entity %u.", entity);
                setCurrentAnimation(entityManager, entity, animController.idleAnimationData_);
            }
            if (!animation.isPlaying()) {
                //SDL_Log("PlayerAnimationControlSystem: Playing idle animation for entity %u.", entity);
                // animation.play(); // SoA 컴포넌트의 복사본에 대한 호출은 실제 데이터에 영향 없음
            }
        }

        // 방향에 따른 좌우 반전
        auto renderArray = entityManager.getComponentArray<RenderComponent>();
        if (renderArray && renderArray->hasComponent(entity)) {
            const size_t i = renderArray->getEntityToIndexMap().at(entity);

            if (acceleration.ax < 0) { // 왼쪽으로 이동 (기본 방향이 왼쪽이므로 반전 없음)
                if (renderArray->flipX[i] != false) {
                    renderArray->flipX[i] = false;
                }
            } else if (acceleration.ax > 0) { // 오른쪽으로 이동 (오른쪽을 바라보도록 반전)
                if (renderArray->flipX[i] != true) {
                    renderArray->flipX[i] = true;
                }
            }
        }
    }
}

void PlayerAnimationControlSystem::playWalkAnimation(EntityManager& entityManager, EntityId entityId, std::shared_ptr<Animation> walkAnimationData) {
    setCurrentAnimation(entityManager, entityId, walkAnimationData);
}

void PlayerAnimationControlSystem::playJumpAnimation(EntityManager& entityManager, EntityId entityId, std::shared_ptr<Animation> jumpAnimationData) {
    setCurrentAnimation(entityManager, entityId, jumpAnimationData);
}

bool PlayerAnimationControlSystem::isJumpAnimationActive(AnimationComponent* animation, std::shared_ptr<Animation> jumpAnimationData) const {
    return animation && animation->animation_ == jumpAnimationData;
}

void PlayerAnimationControlSystem::setCurrentAnimation(EntityManager& entityManager, EntityId entityId, std::shared_ptr<Animation> newAnimation) {
    if (!newAnimation) {
        std::cerr << "Error: Attempted to set a null animation for entity " << entityId << std::endl;
        return;
    }

    // Update or add AnimationComponent
    auto animArray = entityManager.getComponentArray<AnimationComponent>();
    if (animArray && animArray->hasComponent(entityId)) {
        const size_t i = animArray->getEntityToIndexMap().at(entityId);
        animArray->animations[i] = newAnimation;
        animArray->currentFrames[i] = 0;
        animArray->frameTimers[i] = 0.0f;
        animArray->arePlaying[i] = true;
        animArray->areFinished[i] = false;
    } else {
        entityManager.addComponent<AnimationComponent>(entityId, newAnimation);
    }

    // Get the texture for the new animation. TextureManager will provide a default texture if it fails.
    Texture* newAnimTexture = textureManager_.getTexture(newAnimation->getTexturePath());

    // Update or add RenderComponent
    auto renderArray = entityManager.getComponentArray<RenderComponent>();
    if (renderArray && renderArray->hasComponent(entityId)) {
        const size_t i = renderArray->getEntityToIndexMap().at(entityId);
        renderArray->sdlTextures[i] = newAnimTexture->sdlTexture_;
        const SDL_Rect& firstFrameRect = newAnimation->getFrame(0);
        renderArray->srcRectX[i] = firstFrameRect.x;
        renderArray->srcRectY[i] = firstFrameRect.y;
        renderArray->srcRectW[i] = firstFrameRect.w;
        renderArray->srcRectH[i] = firstFrameRect.h;
        renderArray->hasAnimations[i] = true;
    } else {
        const SDL_Rect& firstFrameRect = newAnimation->getFrame(0);
        entityManager.addComponent<RenderComponent>(entityId, newAnimTexture->sdlTexture_, RenderLayer::GAME_OBJECT, false, true, firstFrameRect.w, firstFrameRect.h, firstFrameRect, false, false);
    }
}