#pragma once
#include "../GNEngine_API.h"

#include "GNEngine/core/Component.h"

/**
 * @brief TransformComponent는 게임 오브젝트의 2D 공간에서의 위치, 회전, 크기를 나타내는 데이터임
 * @warning PhysicsSystem에서 매 물리 프레임마다 업데이트함.
 * @param positionX(0.0f) 위치 X
 * @param positionY(0.0f) 위치 Y
 * @param scaleX(1.0f) 크기 배율 (너비)
 * @param scaleY(1.0f) 크기 배율 (높이)
 * @param rotatedAngle(0.0f) 회전된 각도
 */ 
class GNEngine_API TransformComponent : public Component {
public:
    /**
     * @brief TransformComponent의 생성자.
     * @param positionX, positionY 위치 X, Y
     * @param scaleX, scaleY 크기 배율 (너비, 높이)
     * @param rotatedAngle 회전된 각도
    */
    TransformComponent(float positionX = 0.0f, float positionY = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f, float rotatedAngle = 0.0f) 
        : positionX_(positionX), positionY_(positionY), scaleX_(scaleX), scaleY_(scaleY), angle_(rotatedAngle) {}
        
    /* 위치 */
    float positionX_;
    float positionY_;

    /* 크기. (배수) */
    float scaleX_; /* x축 크기 배율 */
    float scaleY_; /* y축 크기 배율 */

    /* 0 ~ 360도(degree) 값 */
    float angle_; /* 회전된 각도 */
};