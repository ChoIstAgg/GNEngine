#pragma once
#include "GNEngine/GNEngine_API.h"
#include <box2d/box2d.h>

class RigidBodyComponent;
class TransformComponent;

/*
 * @brief 물리 시뮬레이션 설정을 관리하며
 *        Box2D와 같은 물리 엔진의 전역 설정을 담당함.
 */
class GNEngine_API PhysicsManager {
public:
    PhysicsManager();
    ~PhysicsManager();

    /*
     * @brief 물리 세계를 초기화하고 생성함.
     */
    void init();

    /*
     * @brief 물리 세계를 종료하고 파괴함.
     */
    void shutdown();

    /*
     * @brief 물리 시뮬레이션을 진행함.
     * @param timeStep 시간 간격 (deltaTime)
     * @param subStepCount 물리 연산 반복 횟수
     */
    void step(float timeStep, int subStepCount);

    /*
     * @brief 물리 세계의 중력 벡터를 설정함.
     * @param gravity 설정할 중력 벡터.
     */
    void setGravity(b2Vec2 gravity);

    b2WorldId getWorldId() const { return worldId_; }

    // --- Creation / Destruction Wrappers ---

    b2BodyId createBody(const b2BodyDef& def);
    void destroyBody(b2BodyId bodyId);

    b2ShapeId createPolygonShape(b2BodyId bodyId, const b2ShapeDef& shapeDef, const b2Polygon& polygon);
    b2ShapeId createCircleShape(b2BodyId bodyId, const b2ShapeDef& shapeDef, const b2Circle& circle);
    b2ShapeId createCapsuleShape(b2BodyId bodyId, const b2ShapeDef& shapeDef, const b2Capsule& capsule);
    b2ShapeId createSegmentShape(b2BodyId bodyId, const b2ShapeDef& shapeDef, const b2Segment& segment);

    // --- State Sync ---
    
    /**
     * @brief Box2D 바디의 상태를 RigidBodyComponent에 동기화함.
     */
    void updateRigidBodyComponent(RigidBodyComponent& rb);
    /**
     * @brief TransformComponent를 
    */
    void updateTransformComponent(TransformComponent& tc);


    // --- Conversion Helpers ---

    float getPixelsPerMeter() const { return pixelsPerMeter_; }
    void setPixelsPerMeter(float value) { pixelsPerMeter_ = value; }

    // Meter -> Pixel
    float toPixels(float meters) const { return meters * pixelsPerMeter_; }
    b2Vec2 toPixels(const b2Vec2& meters) const { return {meters.x * pixelsPerMeter_, meters.y * pixelsPerMeter_}; }

    // Pixel -> Meter
    float toMeters(float pixels) const { return pixels / pixelsPerMeter_; }
    b2Vec2 toMeters(const b2Vec2& pixels) const { return {pixels.x / pixelsPerMeter_, pixels.y / pixelsPerMeter_}; }


private:
    b2WorldId worldId_ = b2_nullWorldId;
    b2Vec2 gravity_ = {0.0f, 9.8f};
    float pixelsPerMeter_ = 32.0f; 
};
