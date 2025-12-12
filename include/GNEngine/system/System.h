#pragma once
#include "../GNEngine_API.h"

/*
 * @brief 모든 시스템의 기본 인터페이스를 정의하는 추상 클래스.
 *        모든 구체적인 시스템은 이 클래스를 상속받아 init, shutdown, update 메서드를 구현해야 함.
 */
class GNEngine_API System {
public:
    /*
     * @brief 시스템 초기화 메서드.
     *        시스템이 사용될 준비를 하는 데 필요한 모든 설정을 수행함.
     */
    virtual void init() = 0;

    /*
     * @brief 시스템 업데이트 메서드.
     *        매 프레임마다 시스템의 핵심 로직을 실행함.
     * @param entityManager 엔티티 및 컴포넌트 관리를 위한 EntityManager 참조.
     * @param deltaTime 이전 프레임과의 시간 간격.
     */
    virtual void update(class EntityManager& entityManager, float deltaTime) = 0;

    /*
     * @brief 시스템 종료 메서드.
     *        시스템이 사용을 마칠 때 필요한 모든 정리 작업을 수행함.
     */
    virtual void shutdown() = 0;

    /*
     * @brief 가상 소멸자.
     *        상속받는 클래스의 올바른 소멸을 보장함.
     */
    virtual ~System() = default;
};