#pragma once
#include "../GNEngine_API.h"

#include "GNEngine/core/Component.h"

/*
 * @brief 오브젝트의 가속도를 나타내는 컴포넌트.
 * @param ax(0.0f) x축 가속도
 * @param ay(0.0f) y축 가속도
*/       
struct GNEngine_API AccelerationComponent : public Component {
    float ax = 0.0f;
    float ay = 0.0f;

    AccelerationComponent(float ax = 0.0f, float ay = 0.0f)
        : ax(ax), ay(ay) {}
};