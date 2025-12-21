#include "GNEngine/manager/PhysicsManager.h"
#include "GNEngine/component/RigidBodyComponent.h"
#include "GNEngine/component/TransformComponent.h"
#include <iostream>
#include <algorithm> // for std::remove_if, std::erase
#include <box2d/base.h> // for b2_pi
#include <box2d/types.h> // for b2BodyId.index and b2WorldId.index

PhysicsManager::PhysicsManager() {
    // 생성자에서는 물리 세계를 초기화하지 않음. init() 호출 시 초기화됨.
}

PhysicsManager::~PhysicsManager() {
    shutdown();
}

void PhysicsManager::init() {
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = gravity_;
    worldId_ = b2CreateWorld(&worldDef);

    if (!b2World_IsValid(worldId_)) {
        std::cerr << "PhysicsManager: Failed to create Box2D world!" << std::endl;
    } else {
        std::cout << "PhysicsManager: Physics world initialized successfully.\n";
    }
}

void PhysicsManager::shutdown() {
    if (b2World_IsValid(worldId_)) {
        // 모든 바디를 파괴
        for (b2BodyId bodyId : bodyIds_) {
            if (b2Body_IsValid(bodyId)) {
                b2DestroyBody(bodyId);
            }
        }
        bodyIds_.clear(); // ID 목록 비우기

        b2DestroyWorld(worldId_);
        worldId_ = b2_nullWorldId;
        std::cout << "PhysicsManager: Physics world shutdown successfully.\n";
    }
}

void PhysicsManager::step(float timeStep, int subStepCount) {
    if (b2World_IsValid(worldId_)) {
        b2World_Step(worldId_, timeStep, subStepCount);
    }
}

void PhysicsManager::setGravity(b2Vec2 gravity) {
    gravity_ = gravity;
    if (b2World_IsValid(worldId_)) {
        b2World_SetGravity(worldId_, gravity_);
    }
}

b2BodyId PhysicsManager::createBody(const b2BodyDef& def) {
    if (!b2World_IsValid(worldId_)) {
        std::cerr << "PhysicsManager: Cannot create body, world is not initialized." << std::endl;
        return b2_nullBodyId;
    }
    b2BodyId newBodyId = b2CreateBody(worldId_, &def);
    if (b2Body_IsValid(newBodyId)) {
        bodyIds_.push_back(newBodyId);
    } else {
        std::cerr << "PhysicsManager: Failed to create body." << std::endl;
    }
    return newBodyId;
}

void PhysicsManager::destroyBody(b2BodyId bodyId) {
    if (!b2Body_IsValid(bodyId)) return;

    if (b2World_IsValid(worldId_)) {
        b2DestroyBody(bodyId);
        removeBodyId(bodyId); // 관리 목록에서 제거
    } else {
        std::cerr << "PhysicsManager: Cannot destroy body, world is not initialized." << std::endl;
    }
}

b2ShapeId PhysicsManager::createPolygonShape(b2BodyId bodyId, const b2ShapeDef& shapeDef, const b2Polygon& polygon) {
    if (!b2Body_IsValid(bodyId)) {
        std::cerr << "PhysicsManager: Cannot create polygon shape, body ID is invalid." << std::endl;
        return b2_nullShapeId;
    }
    return b2CreatePolygonShape(bodyId, &shapeDef, &polygon);
}

b2ShapeId PhysicsManager::createCircleShape(b2BodyId bodyId, const b2ShapeDef& shapeDef, const b2Circle& circle) {
    if (!b2Body_IsValid(bodyId)) {
        std::cerr << "PhysicsManager: Cannot create circle shape, body ID is invalid." << std::endl;
        return b2_nullShapeId;
    }
    return b2CreateCircleShape(bodyId, &shapeDef, &circle);
}

b2ShapeId PhysicsManager::createCapsuleShape(b2BodyId bodyId, const b2ShapeDef& shapeDef, const b2Capsule& capsule) {
    if (!b2Body_IsValid(bodyId)) {
        std::cerr << "PhysicsManager: Cannot create capsule shape, body ID is invalid." << std::endl;
        return b2_nullShapeId;
    }
    return b2CreateCapsuleShape(bodyId, &shapeDef, &capsule);
}

b2ShapeId PhysicsManager::createSegmentShape(b2BodyId bodyId, const b2ShapeDef& shapeDef, const b2Segment& segment) {
    if (!b2Body_IsValid(bodyId)) {
        std::cerr << "PhysicsManager: Cannot create segment shape, body ID is invalid." << std::endl;
        return b2_nullShapeId;
    }
    return b2CreateSegmentShape(bodyId, &shapeDef, &segment);
}

void PhysicsManager::updateRigidBodyComponent(RigidBodyComponent& rb) {
    if (!b2Body_IsValid(rb.bodyId)) {
        // std::cerr << "PhysicsManager: Warning - Attempted to update an invalid RigidBodyComponent." << std::endl;
        return;
    }

    rb.position = toPixels(b2Body_GetPosition(rb.bodyId));
    rb.angle = b2Rot_GetAngle(b2Body_GetRotation(rb.bodyId));
    rb.linearVelocity = toPixels(b2Body_GetLinearVelocity(rb.bodyId));
    rb.angularVelocity = b2Body_GetAngularVelocity(rb.bodyId);
    rb.linearDamping = b2Body_GetLinearDamping(rb.bodyId);
    rb.angularDamping = b2Body_GetAngularDamping(rb.bodyId);
    rb.gravityScale = b2Body_GetGravityScale(rb.bodyId);
    rb.bodyType = b2Body_GetType(rb.bodyId);
    
    rb.isAwake = b2Body_IsAwake(rb.bodyId);
    rb.isEnabled = b2Body_IsEnabled(rb.bodyId);
    rb.isFixedRotation = b2Body_IsFixedRotation(rb.bodyId);
    rb.isBullet = b2Body_IsBullet(rb.bodyId);
    rb.isSleepingAllowed = b2Body_IsSleepEnabled(rb.bodyId); // Box2D 3.1.1 C API (corrected function name)
}

void PhysicsManager::updateTransformComponent(TransformComponent& tc) {
    // TransformComponent의 구조 및 역할이 명확해지면 다시 구현 예정
    // if (!b2Body_IsValid(tc.bodyId)) {
    //     // std::cerr << "PhysicsManager: Warning - Attempted to update transform for an invalid body ID." << std::endl;
    //     return;
    // }

    // // Box2D 바디의 위치와 각도 업데이트
    // b2Body_SetTransform(tc.bodyId, toMeters(tc.position), tc.angle_);
}

void PhysicsManager::debugPrintPhysicsState() {
    if (!b2World_IsValid(worldId_)) {
        std::cerr << "PhysicsManager: Cannot print physics state, world is not initialized." << std::endl;
        return;
    }

    std::cout << "--- Physics State (World ID: " << worldId_.index << ") ---" << std::endl;
    std::cout << "Gravity: (" << gravity_.x << ", " << gravity_.y << ") | "
              << "Pixels Per Meter: " << pixelsPerMeter_ << std::endl;
    std::cout << "Total tracked Body IDs: " << bodyIds_.size() << std::endl;

    for (b2BodyId bodyId : bodyIds_) {
        if (!b2Body_IsValid(bodyId)) {
            std::cerr << "PhysicsManager: Warning - Encountered invalid body ID in tracked list: " << bodyId.index << std::endl;
            continue;
        }

        b2Vec2 position = b2Body_GetPosition(bodyId);
        float angle = b2Rot_GetAngle(b2Body_GetRotation(bodyId));
        b2Vec2 linearVelocity = b2Body_GetLinearVelocity(bodyId);
        float angularVelocity = b2Body_GetAngularVelocity(bodyId);
        b2BodyType bodyType = b2Body_GetType(bodyId);

        // Meter 단위를 Pixel 단위로 변환하여 출력
        b2Vec2 pixelPosition = toPixels(position);
        b2Vec2 pixelLinearVelocity = toPixels(linearVelocity);

        std::cout << "  Body ID: " << bodyId.index
                  << " | Type: " << (bodyType == b2_staticBody ? "Static" : (bodyType == b2_kinematicBody ? "Kinematic" : "Dynamic"))
                  << " | Pos(px): (" << pixelPosition.x << ", " << pixelPosition.y << ")"
                  << " | Angle(deg): " << angle * 180.0f / b2_pi // 라디안을 도로 변환
                  << " | LinVel(px/s): (" << pixelLinearVelocity.x << ", " << pixelLinearVelocity.y << ")"
                  << " | AngVel(rad/s): " << angularVelocity << std::endl;
    }
    std::cout << "---------------------" << std::endl;
}

void PhysicsManager::removeBodyId(b2BodyId id) {
    std::erase_if(bodyIds_, [&](b2BodyId bodyId) { return b2BodyId_Equal(bodyId, id); });
}
