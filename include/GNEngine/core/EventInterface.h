#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include <typeinfo>
#include "GNEngine/core/Entity.h"

/**
 * @brief 모든 이벤트의 기반이 되는 기본 구조체.
 * 
 * 'handled' 플래그를 포함함.
 * 리스너는 이벤트를 처리한 후 이 값을 true로 설정하면 동일한 이벤트를 구독하는 다른 리스너들이 해당 이벤트를 더 이상 처리하지 않도록 하게 할 수 있음.
 * @note 이 'handled' 플래그가 올바르게 동작하려면 EventManager와 EventListenerComponent가
 * 이벤트를 수정 가능한 참조(non-const reference)로 전달해야함.
 *
 */
struct Event {
    bool handled = false;
};

struct TestEvent : public Event {
    std::string message;
    TestEvent(const std::string_view message) : message(message) {}
};


// --- 창 관련 이벤트 ---

/**
 * @brief 창이 닫힐 때 발생하는 이벤트.
 */
struct WindowCloseEvent : public Event {};

/**
 * @brief 창 크기가 조절될 때 발생하는 이벤트.
 */
struct WindowResizeEvent : public Event {
    unsigned int width, height; /* 바뀐 너비, 높이*/

    WindowResizeEvent(unsigned int newWidth, unsigned int newHeight)
        : width(newWidth), height(newHeight) {} 
};


// --- 키보드 관련 이벤트 ---

/**
 * @brief 키보드 이벤트의 기반이 되는 구조체.
 */
struct KeyEvent : public Event {
    SDL_Scancode keyCode;
protected:
    KeyEvent(SDL_Scancode keycode) : keyCode(keycode) {}
};

/**
 * @brief 모든 키의 현재 상태를 반환하는 이벤트.
 */
struct ActiveKeyEvent {
    bool& keyState;
    ActiveKeyEvent(bool& state) : keyState(state) {}
};

struct KeyPressedEvent : public KeyEvent {

    KeyPressedEvent(SDL_Scancode keycode)
        : KeyEvent(keycode) {}
};

/**
 * @brief 키가 계속 눌려있는 상태일 때 발생하는 이벤트.
 */
struct KeyRepeatEvent : public KeyEvent {
    int repeatTime; // 반복 시간 (ms 단위)

    KeyRepeatEvent(SDL_Scancode keycode, int time)
        : KeyEvent(keycode), repeatTime(time) {}
};

/**
 * @brief 키에서 손을 뗐을 때 발생하는 이벤트.
 */
struct KeyReleasedEvent : public KeyEvent {
    KeyReleasedEvent(SDL_Scancode keycode) : KeyEvent(keycode) {}
};

/**
 * @brief 단일 키에 대한 정보(scancode, 눌린 시간)를 포함하는 구조체.
 */
struct KeyHeldInfo {
    SDL_Scancode scancode;
    uint32_t durationMs; // 키가 눌린 시간 (밀리초)
};

/**
 * @brief 현재 프레임에서 눌려있는 모든 키들의 목록과 눌린 시간을 전달하는 이벤트.
 * 동시 입력을 이벤트 기반으로 처리하기 위해 사용됨.
 */
struct KeysHeldEvent : public Event {
    std::vector<KeyHeldInfo> heldKeys;

    KeysHeldEvent(const std::vector<KeyHeldInfo>& keys) : heldKeys(keys) {}
};



// --- 마우스 관련 이벤트 ---

/**
 * @brief 마우스 커서가 움직일 때 발생하는 이벤트.
 */
struct MouseMovedEvent : public Event {
    float mouseX, mouseY;

    MouseMovedEvent(float x, float y) : mouseX(x), mouseY(y) {}
};

/**
 * @brief 마우스 휠을 스크롤할 때 발생하는 이벤트.
 */
struct MouseScrolledEvent : public Event {
    float xOffset, yOffset;

    MouseScrolledEvent(float x, float y) : xOffset(x), yOffset(y) {}
};

/**
 * @brief 마우스 버튼 이벤트의 기반이 되는 구조체.
 */
struct MouseButtonEvent : public Event {
    int button;

protected:
    MouseButtonEvent(int btn) : button(btn) {}
};

/**
 * @brief 마우스 버튼이 눌렸을 때 발생하는 이벤트.
 */
struct MouseButtonPressedEvent : public MouseButtonEvent {
    MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}
};

/**
 * @brief 마우스 버튼에서 손을 뗐을 때 발생하는 이벤트.
 */
struct MouseButtonReleasedEvent : public MouseButtonEvent {
    MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}
};


// --- 게임 로직 관련 이벤트 ---

/**
 * @brief 키 입력 등 사용자 액션에 의해 발생하는 게임 내 논리적 이벤트를 나타냄.
 * 
 * InputSystem이 키 입력을 감지하고 InputControlComponent의 매핑을 기반으로 이 이벤트를 생성함.
 * 다른 게임 시스템(예: MovementSystem, AbilitySystem)이 이 이벤트를 구독하여
 * 실제 게임 로직을 처리함.
 * 
 * @field targetEntityId 액션의 대상이 되는 엔티티의 ID.
 * @field actionName 액션의 종류를 나타내는 문자열 (예: "move_forward", "jump", "fire").
 */
struct ActionEvent : public Event {
    EntityId targetEntityId;
    std::string actionName;

    ActionEvent(EntityId entityId, const std::string& action)
        : targetEntityId(entityId), actionName(action) {}
};

// --- ECS 관련 이벤트 ---

/**
 * @brief 엔티티에 컴포넌트가 추가될 때 발생하는 이벤트.
 */
struct ComponentAddedEvent : public Event {
    EntityId entityId;
    const std::type_info& componentType;

    ComponentAddedEvent(EntityId entity, const std::type_info& type)
        : entityId(entity), componentType(type) {}
};

/**
 * @brief 엔티티에서 컴포넌트가 제거될 때 발생하는 이벤트.
 */
struct ComponentRemovedEvent : public Event {
    EntityId entityId;
    const std::type_info& componentType;

    ComponentRemovedEvent(EntityId entity, const std::type_info& type)
        : entityId(entity), componentType(type) {}
};