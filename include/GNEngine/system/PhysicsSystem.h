#pragma once
#include "GNEngine/GNEngine_API.h"
#include "GNEngine/manager/PhysicsManager.h"
#include "GNEngine/manager/EntityManager.h"
#include "GNEngine/manager/EventManager.h"
#include "GNEngine/core/EventInterface.h"
#include <box2d/box2d.h>

class GNEngine_API PhysicsSystem {
public:
    PhysicsSystem(EntityManager& entityManager, PhysicsManager& physicsManager, EventManager& eventManager);
    ~PhysicsSystem();

    void init();
    void update(EntityManager& entityManager, float deltaTime);
    void shutdown();
    void onComponentAdded(const ComponentAddedEvent& event);
    void onComponentRemoved(const ComponentRemovedEvent& event);

private:
    PhysicsManager& physicsManager_;
    EntityManager& entityManager_;
    EventManager& eventManager_;
    EventManager::SubscriptionId componentAddedSubId_;
    EventManager::SubscriptionId componentRemovedSubId_;
};
