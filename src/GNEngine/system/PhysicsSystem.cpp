#include "GNEngine/system/PhysicsSystem.h"
#include "GNEngine/component/RigidBodyComponent.h"
#include "GNEngine/component/TransformComponent.h"
#include <iostream>

PhysicsSystem::PhysicsSystem(EntityManager& entityManager, PhysicsManager& physicsManager, EventManager& eventManager)
    : entityManager_(entityManager), physicsManager_(physicsManager), eventManager_(eventManager) {}

PhysicsSystem::~PhysicsSystem() {
    shutdown();
}

void PhysicsSystem::init() {
    // Subscribe to events
    componentAddedSubId_ = eventManager_.subscribe<ComponentAddedEvent>(
        std::bind(&PhysicsSystem::onComponentAdded, this, std::placeholders::_1)
    );
    componentRemovedSubId_ = eventManager_.subscribe<ComponentRemovedEvent>(
        std::bind(&PhysicsSystem::onComponentRemoved, this, std::placeholders::_1)
    );

    // Initialize existing bodies
    for (auto entity : entityManager_.getEntitiesWith<RigidBodyComponent, TransformComponent>()) {
        ComponentAddedEvent event(entity, typeid(RigidBodyComponent));
        onComponentAdded(event);
    }
}

void PhysicsSystem::update(EntityManager& entityManager, float deltaTime) {
    // 1. Step the physics world
    // Using a fixed time step is recommended for physics stability.
    // For now, we use deltaTime but capped or fixed step in Manager is better.
    // PhysicsManager::step takes (timeStep, subStepCount)
    physicsManager_.step(deltaTime, 4); 

    // 2. Sync Box2D simulation results back to Components
    for (auto entity : entityManager_.getEntitiesWith<RigidBodyComponent, TransformComponent>()) {
        auto rbOpt = entityManager.getComponent<RigidBodyComponent>(entity);
        auto trOpt = entityManager.getComponent<TransformComponent>(entity);

        if (!rbOpt || !trOpt) continue;

        RigidBodyComponent& rb = rbOpt.value();
        TransformComponent& tr = trOpt.value();

        if (b2Body_IsValid(rb.bodyId)) {
            // Update RigidBodyComponent data from Box2D
            physicsManager_.updateRigidBodyComponent(rb);

            // Update TransformComponent from RigidBodyComponent (which now holds fresh Box2D data)
            // Convert Meters (Box2D) -> Pixels (Game World)
            b2Vec2 posMeters = rb.position;
            b2Vec2 posPixels = physicsManager_.toPixels(posMeters);
            
            tr.positionX_ = posPixels.x;
            tr.positionY_ = posPixels.y;
            tr.angle_ = rb.angle; // Box2D angle is radians, Transform might need conversion if it expects degrees? 
                                  // Assuming Transform uses Radians as per common std. If Degrees, need conversion.
                                  // Looking at previous code, it used atan2, implying radians.
        }
    }
}

void PhysicsSystem::shutdown() {
    eventManager_.unsubscribe(typeid(ComponentAddedEvent), componentAddedSubId_);
    eventManager_.unsubscribe(typeid(ComponentRemovedEvent), componentRemovedSubId_);

    // Destroy all bodies managed by this system
    for (auto entity : entityManager_.getEntitiesWith<RigidBodyComponent>()) {
        auto rbOpt = entityManager_.getComponent<RigidBodyComponent>(entity);
        if (rbOpt) {
            physicsManager_.destroyBody(rbOpt.value().bodyId);
            rbOpt.value().bodyId = b2_nullBodyId;
        }
    }
}

void PhysicsSystem::onComponentAdded(const ComponentAddedEvent& event) {
    if (event.componentType != typeid(RigidBodyComponent)) return;

    EntityId entity = event.entityId;
    auto rbOpt = entityManager_.getComponent<RigidBodyComponent>(entity);
    auto trOpt = entityManager_.getComponent<TransformComponent>(entity);

    if (!rbOpt || !trOpt) { return; }

    RigidBodyComponent& rb = rbOpt.value();
    TransformComponent& tr = trOpt.value();

    if (b2Body_IsValid(rb.bodyId)) { return; }// Already exists

    // Create Body Definition
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = rb.bodyType;
    
    // Convert Pixels -> Meters
    b2Vec2 posPixels = {tr.positionX_, tr.positionY_};
    bodyDef.position = physicsManager_.toMeters(posPixels);
    bodyDef.rotation = b2MakeRot(tr.angle_);
    bodyDef.fixedRotation = rb.isFixedRotation;
    bodyDef.linearVelocity = rb.linearVelocity;
    bodyDef.angularVelocity = rb.angularVelocity;
    bodyDef.gravityScale = rb.gravityScale;
    bodyDef.enableSleep = rb.isSleepingAllowed;
    bodyDef.isAwake = rb.isAwake;
    bodyDef.isEnabled = rb.isEnabled;

    // Create Body
    rb.bodyId = physicsManager_.createBody(bodyDef);

    // Create Shape (Box for now)
    // TODO: Support other shapes via ShapeComponent or similar
    b2Polygon box = b2MakeBox(
        physicsManager_.toMeters(rb.size.x) * 0.5f, 
        physicsManager_.toMeters(rb.size.y) * 0.5f
    );

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = rb.density;
    
    b2ShapeId shapeId = physicsManager_.createPolygonShape(rb.bodyId, shapeDef, box);
    
    // Set material properties after creation
    b2Shape_SetFriction(shapeId, rb.friction);
    b2Shape_SetRestitution(shapeId, rb.restitution);
}

void PhysicsSystem::onComponentRemoved(const ComponentRemovedEvent& event) {
    if (event.componentType != typeid(RigidBodyComponent)) return;

    EntityId entity = event.entityId;
    auto rbOpt = entityManager_.getComponent<RigidBodyComponent>(entity);
    if (rbOpt && b2Body_IsValid(rbOpt.value().bodyId)) {
        physicsManager_.destroyBody(rbOpt.value().bodyId);
        rbOpt.value().bodyId = b2_nullBodyId;
    }
}
