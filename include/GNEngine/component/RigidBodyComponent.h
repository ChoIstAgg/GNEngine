#pragma once
#include "GNEngine/core/Component.h"
#include <box2d/box2d.h>

/**
 * @struct RigidBodyComponent
 * @brief 엔티티의 물리적 속성을 정의하는 컴포넌트.
 *        Box2D의 b2BodyId를 관리하며, 물리 시뮬레이션 결과(위치, 속도 등)를 반영함.
 *        PhysicsSystem이 초기화 시 이 데이터를 기반으로 b2Body를 생성함.
 * @param size 박스 크기(width, height) - 미터 단위
 * @param type b2BodyType (b2_staticBody, b2_kinematicBody, b2_dynamicBody)
 * @param density 밀도(질량 계산에 사용)
 * @param friction 마찰 계수
 * @param restitution 반발 계수
 * @param isFixedRotation 회전 고정 여부(true면 회전이 제한됨)
 * @date 2025-12-20
 */
struct RigidBodyComponent : public Component {
    // --- Runtime Handle ---
    b2BodyId bodyId = b2_nullBodyId;

    // --- Configuration & State ---
    // (초기 설정 및 런타임 상태 미러링)
    
    b2Vec2 position = {0.0f, 0.0f}; // 물리 월드상의 위치 (미터 단위)
    float angle = 0.0f;             // 물리 월드상의 회전 (라디안)
    
    b2Vec2 linearVelocity = {0.0f, 0.0f};
    float angularVelocity = 0.0f;

    float linearDamping = 0.0f;
    float angularDamping = 0.0f;
    float gravityScale = 1.0f;

    b2BodyType bodyType = b2_dynamicBody;

    bool isFixedRotation = false;
    bool isBullet = false;
    bool isAwake = true;
    bool isEnabled = true;
    bool isSleepingAllowed = true;

    // --- Shape/Fixture Properties (Simplified) ---
    // (추후 FixtureComponent로 분리될 수 있으나, 편의를 위해 여기에 유지)
    b2Vec2 size = {1.0f, 1.0f}; // 박스 형태일 경우 크기 (미터 단위, Width-Height)
    float density = 1.0f;
    float friction = 0.3f;
    float restitution = 0.0f;

    // --- Constructors ---
    
    RigidBodyComponent() = default;
    /**
     * @brief 기본 박스 형태 RigidBody 생성자.
     *        컴포넌트의 물리 특성(크기, 타입, 밀도 등)
     * @param size 박스 크기(width, height) - 미터 단위
     * @param type(b2_staticBody) b2BodyType (b2_staticBody, b2_kinematicBody, b2_dynamicBody)
     * @param density(1.0f) 밀도(질량 계산에 사용)
     * @param friction(0.3f) 마찰 계수
     * @param restitution(0.0f) 반발 계수
     * @param isFixedRotation(false) 회전 고정 여부(true면 회전이 제한됨)
     * @date 2025-12-20
     */
    RigidBodyComponent(b2Vec2 size, b2BodyType type = b2_staticBody, float density = 1.0f, float friction = 0.3f, float restitution = 0.0f, bool isFixedRotation = false)
        : size(size), bodyType(type), density(density), friction(friction), restitution(restitution), isFixedRotation(isFixedRotation) {}
};
