#pragma once

#include "../GNEngine_API.h"
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include "GNEngine/manager/EntityManager.h"

// 시스템 실행 단계를 정의하는 열거형
enum class SystemPhase {
    PRE_UPDATE,      // 입력 처리, 상태 초기화 등
    LOGIC_UPDATE,    // 게임 로직, AI, 이벤트 기반 로직 처리
    PHYSICS_UPDATE,  // 물리 계산, 이동
    POST_UPDATE,     // 카메라 추적, 애니메이션 프레임 업데이트 등 후처리
    RENDER           // 최종 상태를 화면에 렌더링
};

/**
 * @class SystemManager
 * @brief 모든 시스템의 생명주기와 실행 순서를 관리하는 클래스.
 * 
 * 시스템들을 정해진 실행 단계(SystemPhase)에 따라 그룹화하고,
 * 매 프레임 정해진 순서대로 모든 시스템의 update 함수를 호출함.
 */
class GNEngine_API SystemManager {
public:
    SystemManager(EntityManager& entityManager);
    ~SystemManager();

    /**
     * @brief 시스템을 특정 실행 단계에 등록함.
     * @tparam T 등록할 시스템의 타입.
     * @param phase 시스템이 속할 실행 단계.
     */
    template<typename T, typename... Args>
    void registerSystem(SystemPhase phase, Args&&... args) {
        auto system = std::make_shared<T>(std::forward<Args>(args)...);
        systems_[phase].push_back(
            [this, system](float deltaTime) {
                system->update(entityManager_, deltaTime);
            }
        );
    }

    /**
     * @brief 등록된 모든 시스템을 단계 순서에 따라 업데이트함.
     * @param deltaTime 이전 프레임과의 시간 간격.
     */
    void updateAll(float deltaTime);

private:
    EntityManager& entityManager_;
    std::map<SystemPhase, std::vector<std::function<void(float)>>> systems_;
};