#include "GNEngine/system/PhysicsSystem.h"
#include "GNEngine/component/RigidBodyComponent.h"
#include "GNEngine/component/TransformComponent.h"

PhysicsSystem::PhysicsSystem(EntityManager& entityManager, PhysicsManager& physicsManager, EventManager& eventManager)
    : entityManager_(entityManager), physicsManager_(physicsManager), eventManager_(eventManager) {}

PhysicsSystem::~PhysicsSystem() {
}

void PhysicsSystem::init() {
    std::cerr << "PhysicsSystem initialized.\n";

    b2WorldId worldId = physicsManager_.getWorldId();
    if (!b2World_IsValid(worldId)) {
        std::cerr << "PhysicsSystem::init - Invalid physics world ID from PhysicsManager.\n";
        return;
    }

    // 기존 엔티티에 대한 b2Body 생성 로직은 onComponentAdded 이벤트 핸들러로 이동
    // init 시점에 이미 존재하는 RigidBodyComponent에 대해 b2Body를 생성합니다.
    for (auto entity : entityManager_.getEntitiesWith<RigidBodyComponent, TransformComponent>()) {
        ComponentAddedEvent event(entity, typeid(RigidBodyComponent));
        onComponentAdded(event);
    }

    componentAddedSubId_ = eventManager_.subscribe<ComponentAddedEvent>(
        std::bind(&PhysicsSystem::onComponentAdded, this, std::placeholders::_1)
    );
    componentRemovedSubId_ = eventManager_.subscribe<ComponentRemovedEvent>(
        std::bind(&PhysicsSystem::onComponentRemoved, this, std::placeholders::_1)
    );
}

void PhysicsSystem::update(EntityManager& entityManager, float deltaTime) {
    b2WorldId worldId = physicsManager_.getWorldId();
    if (!b2World_IsValid(worldId)) return;

    b2World_Step(worldId, physicsManager_.getTimeStep(), 8); // Corrected argument count

    // RigidBodyComponent 업데이트 
    for (auto entity : entityManager_.getEntitiesWith<RigidBodyComponent, TransformComponent>()) {
        auto rigidBodyRef = entityManager_.getComponent<RigidBodyComponent>(entity);
        if (!rigidBodyRef.has_value()) continue; // Should not happen if getEntitiesWith is correct
        auto& rigidBody = rigidBodyRef;

        if (rigidBody.value().bodyType == b2_dynamicBody) {
            auto transformRef = entityManager_.getComponent<TransformComponent>(entity);
            if (!transformRef.has_value()) continue; // Should not happen
            auto& transform = transformRef;

            b2Vec2 position = b2Body_GetPosition(rigidBody.value().runtimeBody);
            b2Rot rotation = b2Body_GetRotation(rigidBody.value().runtimeBody);
            transform.value().positionX_ = position.x * physicsManager_.getPixelsPerMeter();
            transform.value().positionY_ = position.y * physicsManager_.getPixelsPerMeter();
            transform.value().rotatedAngle_ = atan2f(rotation.s, rotation.c);
        }
    }
}

void PhysicsSystem::shutdown() {
    std::cerr << "PhysicsSystem shutdown.\n";

    eventManager_.unsubscribe(typeid(ComponentAddedEvent), componentAddedSubId_);
    eventManager_.unsubscribe(typeid(ComponentRemovedEvent), componentRemovedSubId_);

    b2WorldId worldId = physicsManager_.getWorldId();
    if (!b2World_IsValid(worldId)) {
        std::cerr << "PhysicsSystem::shutdown - Invalid physics world ID from PhysicsManager.\n";
        return;
    }

    for (auto entity : entityManager_.getEntitiesWith<RigidBodyComponent>()) {
        auto rigidBodyOpt = entityManager_.getComponent<RigidBodyComponent>(entity);
        if (!rigidBodyOpt.has_value()) continue; // Should not happen

        auto& rigidBody = rigidBodyOpt.value();

        if (b2Body_IsValid(rigidBody.runtimeBody)) { /* Destroy successfully. */
            b2DestroyBody(rigidBody.runtimeBody);
            rigidBody.runtimeBody = b2_nullBodyId;
        }
    }
}

void PhysicsSystem::onComponentAdded(const ComponentAddedEvent& event) {
    if (event.componentType == typeid(RigidBodyComponent)) {
        EntityID entity = event.entityId;
        auto rigidBodyRef = entityManager_.getComponent<RigidBodyComponent>(entity);
        auto transformRef = entityManager_.getComponent<TransformComponent>(entity);

        if (!rigidBodyRef.has_value() || !transformRef.has_value()) {
            std::cerr << "PhysicsSystem::onComponentAdded - Entity " << entity << " missing RigidBodyComponent or TransformComponent.\n";
            return;
        }

        auto& rigidBody = rigidBodyRef.value(); 
        auto& transform = transformRef.value(); 

        b2WorldId worldId = physicsManager_.getWorldId();
        if (!b2World_IsValid(worldId)) {
            std::cerr << "PhysicsSystem::onComponentAdded - Invalid physics world ID from PhysicsManager.\n";
            return;
        }

        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = rigidBody.bodyType;
        bodyDef.position = b2Vec2(transform.positionX_ / physicsManager_.getPixelsPerMeter(), transform.positionY_ / physicsManager_.getPixelsPerMeter());
        bodyDef.rotation = b2Rot(transform.rotatedAngle_);
        bodyDef.fixedRotation = rigidBody.isFixedRotation;

        b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
        rigidBody.runtimeBody = bodyId;

        b2Polygon box = b2MakeBox(rigidBody.size.x / 2.0f / physicsManager_.getPixelsPerMeter(), rigidBody.size.y / 2.0f / physicsManager_.getPixelsPerMeter());

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = rigidBody.density;

        b2ShapeId shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &box);
        b2Shape_SetFriction(shapeId, rigidBody.friction);
        b2Shape_SetRestitution(shapeId, rigidBody.restitution);
    }
}

void PhysicsSystem::onComponentRemoved(const ComponentRemovedEvent& event) {
    if (event.componentType == typeid(RigidBodyComponent)) {
        EntityID entity = event.entityId;
        auto rigidBodyRef = entityManager_.getComponent<RigidBodyComponent>(entity);

        if (!rigidBodyRef.has_value()) {
            std::cerr << "PhysicsSystem::onComponentRemoved - Entity " << entity << " missing RigidBodyComponent.\n";
            return;
        }
        auto& rigidBody = rigidBodyRef.value();

        if (b2Body_IsValid(rigidBody.runtimeBody)) {
            b2DestroyBody(rigidBody.runtimeBody);
            rigidBody.runtimeBody = b2_nullBodyId;
        }
    }
}
