#include "GNEngine/manager/PhysicsManager.h"
#include <box2d/box2d.h>

PhysicsManager::PhysicsManager(EntityManager& entityManager, b2Vec2 gravity, float pixelsPerMeter, float timeStep)
    : entityManager_(entityManager),gravity_(gravity), pixelsPerMeter_(pixelsPerMeter), timeStep_(timeStep), worldId_(b2_nullWorldId) {}

PhysicsManager::~PhysicsManager() {
    // shutdownPhysics()에서 월드 파괴를 처리하므로 여기서는 추가 작업 없음
    std::cerr << "PhysicsManager " << this << " is successfully destroyed and inited physics settings. \n";
}

void PhysicsManager::initPhysics() {
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = gravity_;
    worldId_ = b2CreateWorld(&worldDef);
    std::cerr << "PhysicsManager initialized physics world.\n";
}

void PhysicsManager::shutdownPhysics() {
    if (b2World_IsValid(worldId_)) {
        b2DestroyWorld(worldId_);
        worldId_ = b2_nullWorldId;
        std::cerr << "PhysicsManager shutdown physics world.\n";
    }
}

// void PhysicsManager::destroyBody(ComponentRemovedEvent& event) {
//     if(event == )
// }