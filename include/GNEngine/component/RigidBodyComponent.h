#pragma once
#include "GNEngine/core/Component.h"
#include <box2d/box2d.h>

/*
 * @class RigidBodyComponent
 * @brief 엔티티의 물리적 속성을 정의하는 컴포넌트.
 *        Box2D의 b2Body에 대한 핸들(runtimeBody_)과 초기 설정값을 포함함.
 * @param size 물리적 몸체의 크기 (너비- 높이).
 * @param bodyType(b2_staticBody) b2_staticBody(고정)- b2_kinematicBody(코드 제어)- b2_dynamicBody(완전한 물리 제어) 중 하나.
 * @param density(1.0f) 밀도.
 * @param friction(0.3f) 마찰 계수.
 * @param restitution(0.0f) 반발 계수.
 * @param fixedRotation(false) 회전 여부.
*/
class RigidBodyComponent : public Component {
public:
    RigidBodyComponent(b2Vec2 size, b2BodyType bodyType = b2_staticBody, float density = 1.0f, float friction = 0.3f, float restitution = 0.0f, bool isFixedRotation = false)
        : size(size), bodyType(bodyType), density(density), friction(friction), restitution(restitution), isFixedRotation(isFixedRotation), runtimeBody(b2_nullBodyId) {}

    b2Vec2 size;
    b2BodyType bodyType = b2_staticBody; // b2_staticBody(고정)- b2_kinematicBody(코드 제어)- b2_dynamicBody(완전한 물리 제어) 중 하나.
    float density = 1.0f; // 밀도
    float friction = 0.3f; // 마찰 계수
    float restitution = 0.0f; // 반발 계수
    bool isFixedRotation = false; // 회전 여부
    
    b2BodyId runtimeBody; // Box2D의 실제 물리 객체에 대한 핸들. 초기값은 b2_nullBodyId. PhysicsSystem이 물리 객체 생성후 값을 할당함. 
};