#include "GNEngine/system/InputSystem.h"
#include "GNEngineRootPath.h"
#include "GNEngine/component/InputControlComponent.h"
#include "GNEngine/core/Entity.h" 

InputSystem::InputSystem(EventManager& eventManager, EntityManager& entityManager) // entityManager를 인자로 받음
    : eventManager_(eventManager)
{
    keysHeldSubId_ = eventManager_.subscribe<KeysHeldEvent>(
        [this, &entityManager](const KeysHeldEvent& event) { 
            onKeysHeld(event, entityManager); 
        }
    );
}

InputSystem::~InputSystem() {
    eventManager_.unsubscribe(typeid(KeysHeldEvent), keysHeldSubId_);
}

void InputSystem::update(EntityManager& entityManager, float deltaTime) {
    // 이벤트 기반으로 작동하므로 이 함수는 비워둠.
}

void InputSystem::onKeysHeld(const KeysHeldEvent& event, EntityManager& entityManager) {
    if (event.heldKeys.empty()) { return; }
    // SDL_Log("InputSystem: KeysHeldEvent received with %zu keys.", event.heldKeys.size());

    // InputControlComponent를 가진 모든 엔티티를 순회
    for (EntityID entity : entityManager.getEntitiesWith<InputControlComponent>()) {
        auto inputControl_opt = entityManager.getComponent<InputControlComponent>(entity);
        if (!inputControl_opt) continue;
        const auto& inputControl = inputControl_opt.value();

        // 현재 눌린 키가 엔티티의 keyActions 맵에 있는지 확인
        for (const auto& keyInfo : event.heldKeys) {
            auto it = inputControl.keyActions.find(keyInfo.scancode);
            if (it != inputControl.keyActions.end()) {
                // 매핑된 액션을 찾았다면 ActionEvent를 생성하여 dispatch
                // std::cerr << "[DEBUG] InputSystem - Firing ActionEvent " << it->second << " for entity(" << entity << ") \n";
                eventManager_.dispatch(ActionEvent(entity, it->second));
            }
        }
    }
}


