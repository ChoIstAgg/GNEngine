#pragma once
#include "../GNEngine_API.h"

#include <functional>
#include <unordered_map>
#include <list>
#include <typeindex>
#include <any>
#include <utility>
#include <iostream>

#include "GNEngine/core/EventInterface.h"

/**
 * @class EventManager
 * @brief 이벤트 시스템을 관리하는 매니저 클래스.
 * 
 * 이벤트 구독, 발행 및 구독 해지를 처리함.
 * 이벤트 타입에 따라 구독자 목록을 관리하고, 이벤트 발생 시 해당 이벤트를 등록한 EventListenerComponent의 콜백 함수를 호출함.
 */
class GNEngine_API EventManager {
public:
    using SubscriptionId = int32_t;
    using Callback = std::function<void(const std::any&)>; /* 범용 함수 타입. (void 반환형) */

private:
    struct Subscriber {
        SubscriptionId id;
        Callback callback; /* 이벤트 발생시 실행 할 함수*/
    };
    
    /* key= 이벤트의 타입(type_index. typeid()로 사용하기.) : value= 구독자 목록(list) */
    std::unordered_map<std::type_index, std::list<Subscriber>> subscribers_; 
    SubscriptionId nextId_ = 0; /* For allocation of a next subscripttion id. */

public:
    EventManager() 
        : nextId_(0) {
        subscribers_.clear();
    };
    
    ~EventManager() {
        if(!subscribers_.empty()) { subscribers_.clear(); }
        std::cerr << "EventManager " << this << " is successfully destroyed and all subscriptions cleared.\n";
    }

    /* 이벤트 구독하기 */
    template<typename T_Event>
    SubscriptionId subscribe (std::function<void(const T_Event&)> callback) {
        /* 래퍼 람다 객체로 타입을 소거함. ->  하나의 컨테이너(list)에 저장, 유연성 증가. */
        auto wrapper = [cb = std::move(callback)](const std::any& eventData) {
            if (const T_Event* event = std::any_cast<T_Event>(&eventData)) {
                cb(*event);
            }
        };
        SubscriptionId id = nextId_++; /* Assign next id into corrunt id and increase next id by 1 */
        subscribers_[typeid(T_Event)].push_back({id, wrapper});

        return id; /* 현재 이벤트 타입 id 반환. */
    }

    /* 구독 해지하기*/
    void unsubscribe (const std::type_index& eventType, SubscriptionId id) {
        if (subscribers_.count(eventType)) { /* 만약 이벤트를 구독한 리스너가 존재한다면 */
            auto& subscribers = subscribers_.at(eventType); 
            /* 현재 이벤트 id와 구독자의 이벤트 id가 일치하면 구독자를 목록에서 제거*/
            subscribers.remove_if( [&id](const auto& subscriber) { return (subscriber.id == id); });
        }
    }

    /* 이벤트 발생시 각 리스너의 함수 호출 */
    template<typename T_Event>
    void dispatch (const T_Event& event) {
        std::type_index eventType = typeid(T_Event);
        if (subscribers_.count(eventType)) { /* 해당 이벤트를 구독한 리스너가 존재한다면 */
            auto& subscribers = subscribers_.at(eventType); /* 구독자 list 자체를 참조 */
            for (const auto& sub : subscribers) {
                sub.callback(event); /* 리스너가 콜백 함수 실행 */
            }
        }
    }
    
};