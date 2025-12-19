#pragma once
#include "../GNEngine_API.h"

#include <vector>
#include <memory>
#include <iostream>
#include <filesystem>

#include "GNEngine/manager/AnimationManager.h"
#include "GNEngine/manager/TextureManager.h"
#include "GNEngine/manager/RenderManager.h"
#include "GNEngine/manager/EntityManager.h"
#include "GNEngine/component/PlayerAnimationControllerComponent.h"
#include "GNEngine/component/AnimationComponent.h"
#include "GNEngine/component/VelocityComponent.h"
#include "GNEngine/component/RenderComponent.h"


/*
 * @class PlayerAnimationControlSystem
 * @brief PlayerAnimationControllerComponent를 가진 엔티티의 애니메이션 상태를 제어하는 시스템임.
 *        주로 이동 상태에 따라 걷기, 점프 애니메이션을 전환함.
*/ 
class GNEngine_API PlayerAnimationControlSystem {
public:
    PlayerAnimationControlSystem(
        AnimationManager& animationManager,
        TextureManager& textureManager,
        RenderManager& renderManager
    );

    /*
     * @brief 모든 플레이어 애니메이션 컨트롤러를 업데이트함.
     * @param entityManager - 엔티티와 컴포넌트를 관리하는 EntityManager.
     * @param deltaTime - 이전 프레임으로부터 경과된 시간 (초).
     */
    void update(EntityManager& entityManager, float deltaTime);

private:
    AnimationManager& animationManager_;
    TextureManager& textureManager_;
    RenderManager& renderManager_;

    /*
     * @brief 현재 애니메이션을 설정하고 RenderComponent를 업데이트함.
     * @param entityManager - 엔티티와 컴포넌트를 관리하는 EntityManager.
     * @param entityId - 애니메이션을 제어할 엔티티의 ID.
     * @param newAnimation - 새로 설정할 애니메이션 데이터.
     */
    void setCurrentAnimation(EntityManager& entityManager, EntityId entityId, std::shared_ptr<Animation> newAnimation);

    /*
     * @brief 현재 애니메이션이 점프 애니메이션인지 확인.
     */
    bool isJumpAnimationActive(AnimationComponent* animation, std::shared_ptr<Animation> jumpAnimationData) const;

    /*
     * @brief 걷기 애니메이션으로 전환함.
     */
    void playWalkAnimation(EntityManager& entityManager, EntityId entityId, std::shared_ptr<Animation> walkAnimationData);

    /*
     * @brief 점프 애니메이션으로 전환함.
     */
    void playJumpAnimation(EntityManager& entityManager, EntityId entityId, std::shared_ptr<Animation> jumpAnimationData);
};



