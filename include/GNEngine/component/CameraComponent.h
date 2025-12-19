#pragma once
#include "../GNEngine_API.h"

#include "GNEngine/core/Component.h"
#include "GNEngine/core/Entity.h"

/*
 * @brief 카메라 기능을 제공하는 컴포넌트임.
 * @param targetId(INVALID_ENTITY_ID) 카메라가 따라갈 엔티티 ID
 * @param zoom(1.0f) 카메라의 줌 레벨
 * @param x(0.0f) 카메라의 x 좌표
 * @param y(0.0f) 카메라의 y 좌표
*/
struct GNEngine_API CameraComponent : public Component {
    float x;
    float y;
    float zoom;
    EntityId targetEntityId; // 카메라가 따라갈 엔티티 ID

    CameraComponent(EntityId targetId = INVALID_ENTITY_ID, float zoom = 1.0f, float x = 0.0f, float y = 0.0f)
        : targetEntityId(targetId), zoom(zoom), x(x), y(y) {}
};




