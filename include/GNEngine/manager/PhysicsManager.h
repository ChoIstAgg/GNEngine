#pragma once
#include "GNEngine/GNEngine_API.h"
#include "GNEngine/manager/EntityManager.h"
#include "GNEngine/core/EventInterface.h"
#include <box2d/math_functions.h>
#include <box2d/box2d.h> 
#include <iostream>

/*
 * @brief 물리 시뮬레이션 설정을 관리하며
 *        Box2D와 같은 물리 엔진의 전역 설정을 담당함.
 * @param gravity({0.0f-0.0f}) 물리 세계의 중력 벡터.
 * @param pixelsPerMeter(32.0f) 물리 세계의 1미터당 픽셀 수.
 * @param timeStep(1.0f / 60.0f) 물리 시뮬레이션의 고정 시간 간격.
 */
class GNEngine_API PhysicsManager {
public:
    PhysicsManager(EntityManager& entityManager, b2Vec2 gravity = {0.0f, 0.0f}, float pixelsPerMeter = 32.0f, float timeStep = 1.0f / 60.0f);
    ~PhysicsManager();

    /*
     * @brief 물리 세계를 초기화하고 생성함.
     */
    void initPhysics();

    /*
     * @brief 물리 세계를 종료하고 파괴함.
     */
    void shutdownPhysics();

    /*
     * @brief 물리 세계의 중력 벡터를 설정함.
     * @param gravity 설정할 중력 벡터.
     */
    void setGravity(b2Vec2 gravity) { gravity_ = gravity; }
    /*
     * @brief 물리 세계의 현재 중력 벡터를 반환함.
     * @return 현재 중력 벡터.
     */
    b2Vec2 getGravity() const { return gravity_; }

    /*
     * @brief 물리 세계의 1미터당 픽셀 수를 설정함.
     * @param value 설정할 픽셀/미터 값.
     */
    void setPixelsPerMeter(float value) { pixelsPerMeter_ = value; }
    /*
     * @brief 물리 세계의 현재 1미터당 픽셀 수를 반환함.
     * @return 현재 픽셀/미터 값.
     */
    float getPixelsPerMeter() const { return pixelsPerMeter_; }

    /*
     * @brief 물리 시뮬레이션의 고정 시간 간격을 설정함.
     * @param value 설정할 시간 간격.
     */
    void setTimeStep(float value) { timeStep_ = value; }
    /*
     * @brief 물리 시뮬레이션의 현재 고정 시간 간격을 반환함.
     * @return 현재 시간 간격.
     */
    float getTimeStep() const { return timeStep_; }

    /*
     * @brief 현재 물리 월드의 ID를 반환함.
     * @return 물리 월드의 b2WorldId.
     */
    b2WorldId getWorldId() const { return worldId_; }

    /* 
     * @brief 
    */
    // void createBody(const ComponentAddedEvent& event);
    void destroyBoey(const ComponentRemovedEvent& event);

private:
    b2Vec2 gravity_ = {0.0f, 0.0f}; /* 물리 세계의 중력 벡터. */
    float pixelsPerMeter_ = 32.0f; /* 물리 세계의 1미터당 픽셀 수. */
    float timeStep_ = 1.0f / 60.0f; /* 고정 물리 시간 간격. */
    b2WorldId worldId_ = b2_nullWorldId; // 물리 월드 ID
    std::unordered_map<EntityID, b2BodyId> entityToBodyMap_; /* b2Body를 갖고있는 엔티티 맵*/

    EntityManager& entityManager_;
};
