#pragma once
#include "../GNEngine_API.h"

#include <vector>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <stdexcept>
#include <queue>
#include <optional>
#include <typeindex>
#include <typeinfo>
#include <string>

#include "GNEngine/core/Entity.h"
#include "GNEngine/core/ComponentArray.h"
#include "GNEngine/component/CameraComponent.h"
#include "GNEngine/component/TextComponent.h"

#include <iostream>

/*
 * @class EntityManager
 * @brief ECS에서 엔티티와 컴포넌트의 관계를 관리하는 핵심 클래스임.
 *        엔티티 ID를 생성하고, 엔티티에 컴포넌트를 추가/제거/조회하는 기능을 제공함.
*/ 
class GNEngine_API EntityManager {
public:
    EntityManager() = default;
    ~EntityManager() = default;

    EntityManager(const EntityManager&) = delete;
    EntityManager& operator=(const EntityManager&) = delete;
    EntityManager(EntityManager&&) = delete;
    EntityManager& operator=(EntityManager&&) = delete;

    EntityID createEntity();
    void destroyEntity(EntityID entity);

    /* 
     * @brief 사용할 컴포넌트를 등록한다. 등록한 컴포넌트만 사용 가능. 
     * @tparam T 사용할 컴포넌트

    */
    template<typename T>
    void registerComponentType() {
        std::type_index type = typeid(T);
        if (componentTypes_.find(type) == componentTypes_.end()) {
            if (nextComponentType_ >= MAX_COMPONENTS) {
                throw std::runtime_error("EntityManager: Exceeded maximum number of component types.");
            }
            componentTypes_[type] = nextComponentType_++;
        }
    }

    /*
     * @brief 특정 엔티티에 컴포넌트를 추가함.
     *        SoA 컴포넌트일 경우, 반환값이 없음.
     *        AoS 컴포넌트일 경우, 추가된 컴포넌트의 참조를 반환함.
     * @param entity 컴포넌트를 추가할 Entity의 Id
     * @tparam T 추가할 컴포넌트
     * @tparam Args 컴포넌트 생성자의 파라미터
    */
    template<typename T, typename... Args>
    auto addComponent(EntityID entity, Args&&... args) -> std::conditional_t<std::is_same_v<T, TransformComponent> || std::is_same_v<T, RenderComponent> || std::is_same_v<T, AnimationComponent> || std::is_same_v<T, TextComponent> || std::is_same_v<T, CameraComponent> || std::is_same_v<T, VelocityComponent> || std::is_same_v<T, AccelerationComponent> || std::is_same_v<T, TextComponent>, void, T&>
    {
        std::type_index type = typeid(T);
        if (componentTypes_.find(type) == componentTypes_.end()) {
            throw std::runtime_error("EntityManager: Component type not registered. Call registerComponentType<T>() first." + std::string(type.name()));
        }

        auto componentArray = getComponentArray<T>();
        if (!componentArray) { /* If getComponentArray returned false */
            componentArrays_[type] = std::make_shared<ComponentArray<T>>();
            componentArray = static_cast<ComponentArray<T>*>(componentArrays_[type].get());
        }
        componentArray->addComponent(entity, T(std::forward<Args>(args)...));

        entitySignatures_[entity].set(componentTypes_[type]);

        // If T component type is SoA
        if constexpr (!std::is_same_v<T, TransformComponent> && !std::is_same_v<T, RenderComponent> && !std::is_same_v<T, AnimationComponent> && !std::is_same_v<T, TextComponent> && !std::is_same_v<T, CameraComponent> && !std::is_same_v<T, VelocityComponent> && !std::is_same_v<T, AccelerationComponent>) {
            return componentArray->getComponent(entity);
        }
    }

    /*
     * @brief 특정 엔티티의 컴포넌트를 값으로 조회함.z
     * @return 컴포넌트가 존재하면 std::optional<T>로 감싸진 컴포넌트 값을, 없으면 std::nullopt를 반환함.
    */
    template<typename T>
    std::optional<T> getComponent(EntityID entity) {
        std::type_index type = typeid(T);
        if (componentTypes_.find(type) == componentTypes_.end()) {
            return std::nullopt;
        }

        auto componentArray = getComponentArray<T>();
        if (componentArray && componentArray->hasComponent(entity)) {
            return componentArray->getComponent(entity);
        }
        return std::nullopt;
    }

    template<typename T>
    bool hasComponent(EntityID entity) {
        std::type_index type = typeid(T);
        if (componentTypes_.find(type) == componentTypes_.end()) {
            return false;
        }
        auto componentArray = getComponentArray<T>();
        return componentArray && componentArray->hasComponent(entity);
    }

    template<typename T>
    void removeComponent(EntityID entity) {
        std::type_index type = typeid(T);
        if (componentTypes_.find(type) == componentTypes_.end()) {
            return;
        }

        auto componentArray = getComponentArray<T>();
        if (componentArray) {
            componentArray->removeComponent(entity);
            entitySignatures_[entity].reset(componentTypes_[type]);
        }
    }

    /* 
     * @brief 주어진 컴포넌트들을 모두 갖고있는 Entity의 Id를 std::vector<EntityId> 형태로 반환.
     * @tparam Args 검색할 모든 컴포넌트들
    */
    template<typename... Args>
    std::vector<EntityID> getEntitiesWith() {
        Signature requiredSignature;
        (registerComponentType<Args>(), ...);
        ((requiredSignature.set(componentTypes_[typeid(Args)])), ...);

        std::vector<EntityID> matchingEntities;
        for (EntityID entity : activeEntities_) {
            if ((entitySignatures_[entity] & requiredSignature) == requiredSignature) {
                matchingEntities.push_back(entity);
            }
        }
        return matchingEntities;
    }

    std::vector<EntityID> getAllEntities() const;

    /*
     * @brief 시스템이 컴포넌트 배열에 직접 접근할 수 있도록 포인터를 반환함.
    */
    template<typename T>
    ComponentArray<T>* getComponentArray() {
        auto it = componentArrays_.find(typeid(T));
        if (it != componentArrays_.end()) { 
            return static_cast<ComponentArray<T>*>(it->second.get());
        }
        return nullptr; /* Yet there is no component to perform */
    }

private:
    EntityID nextEntityId_ = 1;
    std::vector<EntityID> activeEntities_;
    std::queue<EntityID> availableEntityIds_;
    std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> componentArrays_;
    std::unordered_map<std::type_index, size_t> componentTypes_;
    size_t nextComponentType_ = 0;
    std::unordered_map<EntityID, Signature> entitySignatures_;
};




