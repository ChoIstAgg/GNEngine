#include "GNEngine/manager/PhysicsManager.h"
#include "GNEngine/component/RigidBodyComponent.h"
#include "GNEngine/component/TransformComponent.h"
#include <iostream>

PhysicsManager::PhysicsManager() {
    // Constructor
}

PhysicsManager::~PhysicsManager() {
    shutdown();
}

void PhysicsManager::init() {
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = gravity_;
    worldId_ = b2CreateWorld(&worldDef);

    if (b2World_IsValid(worldId_) == false) {
        std::cerr << "Failed to create Box2D world!" << std::endl;
    } else {
        std::cout << "PhysicsManager initialized physics world.\n";
    }
}

void PhysicsManager::shutdown() {
    if (b2World_IsValid(worldId_)) {
        b2DestroyWorld(worldId_);
        worldId_ = b2_nullWorldId;
        std::cout << "PhysicsManager shutdown physics world.\n";
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
    if (!b2World_IsValid(worldId_)) return b2_nullBodyId;
    return b2CreateBody(worldId_, &def);
}

void PhysicsManager::destroyBody(b2BodyId bodyId) {
    if (b2Body_IsValid(bodyId)) {
        b2DestroyBody(bodyId);
    }
}

b2ShapeId PhysicsManager::createPolygonShape(b2BodyId bodyId, const b2ShapeDef& shapeDef, const b2Polygon& polygon) {
    if (!b2Body_IsValid(bodyId)) return b2_nullShapeId;
    return b2CreatePolygonShape(bodyId, &shapeDef, &polygon);
}

b2ShapeId PhysicsManager::createCircleShape(b2BodyId bodyId, const b2ShapeDef& shapeDef, const b2Circle& circle) {
    if (!b2Body_IsValid(bodyId)) return b2_nullShapeId;
    return b2CreateCircleShape(bodyId, &shapeDef, &circle);
}

b2ShapeId PhysicsManager::createCapsuleShape(b2BodyId bodyId, const b2ShapeDef& shapeDef, const b2Capsule& capsule) {
    if (!b2Body_IsValid(bodyId)) return b2_nullShapeId;
    return b2CreateCapsuleShape(bodyId, &shapeDef, &capsule);
}

b2ShapeId PhysicsManager::createSegmentShape(b2BodyId bodyId, const b2ShapeDef& shapeDef, const b2Segment& segment) {
    if (!b2Body_IsValid(bodyId)) return b2_nullShapeId;
    return b2CreateSegmentShape(bodyId, &shapeDef, &segment);
}

void PhysicsManager::updateRigidBodyComponent(RigidBodyComponent& rb) {
    if (!b2Body_IsValid(rb.bodyId)) return;

    rb.position = b2Body_GetPosition(rb.bodyId);
    rb.angle = b2Rot_GetAngle(b2Body_GetRotation(rb.bodyId));
    rb.linearVelocity = b2Body_GetLinearVelocity(rb.bodyId);
    rb.angularVelocity = b2Body_GetAngularVelocity(rb.bodyId);
    rb.linearDamping = b2Body_GetLinearDamping(rb.bodyId);
    rb.angularDamping = b2Body_GetAngularDamping(rb.bodyId);
    rb.gravityScale = b2Body_GetGravityScale(rb.bodyId);
    rb.bodyType = b2Body_GetType(rb.bodyId);
    
    rb.isAwake = b2Body_IsAwake(rb.bodyId);
    rb.isEnabled = b2Body_IsEnabled(rb.bodyId);
    rb.isFixedRotation = b2Body_IsFixedRotation(rb.bodyId);
    // rb.allowSleep = b2Body_IsSleepAllowed(rb.bodyId); // Box2D v3 has this? assuming yes or similar
}
