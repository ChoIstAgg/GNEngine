#pragma once
#include "GNEngine/GNEngine_API.h"
#include <box2d/box2d.h> // b2WorldId, b2BodyId, b2Vec2 등 기본 타입 포함
#include <vector>       // std::vector 사용
#include <cstdint>      // uint32_t 등 정수 타입

class RigidBodyComponent;
class TransformComponent;

/**
 * @brief 물리 시뮬레이션 설정을 관리하고 Box2D와 같은 물리 엔진의 전역 설정을 담당함.
 * 
 * 이 클래스는 물리 세계를 초기화, 종료, 업데이트하며,
 * 중력 설정, 바디 생성/파괴, 그리고 내부 상태 동기화 기능을 제공함.
 */
class GNEngine_API PhysicsManager {
public:
    PhysicsManager();
    ~PhysicsManager();

    /**
     * @brief 물리 세계를 초기화하고 생성함.
     *        Box2D 월드를 생성하고 필요한 초기 설정을 적용함.
     */
    void init();

    /**
     * @brief 물리 세계를 종료하고 파괴함.
     *        생성된 모든 물리 객체를 정리하고 리소스를 해제함.
     */
    void shutdown();

    /**
     * @brief 물리 시뮬레이션을 진행함.
     * @param timeStep 시간 간격 (deltaTime)
     * @param subStepCount 물리 연산 반복 횟수
     *        지정된 시간 간격과 반복 횟수만큼 물리 세계를 업데이트함.
     */
    void step(float timeStep, int subStepCount);

    /**
     * @brief 물리 세계의 중력 벡터를 설정함.
     * @param gravity 설정할 중력 벡터.
     *        월드 전체에 적용될 중력 값을 지정함.
     */
    void setGravity(b2Vec2 gravity);

    /**
     * @brief 현재 물리 세계의 ID를 반환함.
     * @return 물리 세계의 b2WorldId.
     */
    b2WorldId getWorldId() const { return worldId_; }

    // --- Creation / Destruction Wrappers ---

    /**
     * @brief 새로운 물리 바디를 생성하고, 해당 BodyId를 관리 목록에 추가함.
     * @param def 바디 생성 정의.
     * @return 생성된 바디의 ID. 실패 시 b2_nullBodyId를 반환함.
     */
    b2BodyId createBody(const b2BodyDef& def);

    /**
     * @brief 지정된 ID의 물리 바디를 파괴하고, 관리 목록에서 제거함.
     * @param bodyId 파괴할 바디의 ID.
     */
    void destroyBody(b2BodyId bodyId);

    // --- Shape Creation ---
    /**
     * @brief 바디에 다각형 모양(Polygon shape)을 생성하여 추가함.
     * @param bodyId 모양을 추가할 바디의 ID.
     * @param shapeDef 모양의 물리적 속성 정의.
     * @param polygon 다각형의 정점들.
     * @return 생성된 모양의 ID. 실패 시 b2_nullShapeId를 반환함.
     */
    b2ShapeId createPolygonShape(b2BodyId bodyId, const b2ShapeDef& shapeDef, const b2Polygon& polygon);
    /**
     * @brief 바디에 원형 모양(Circle shape)을 생성하여 추가함.
     * @param bodyId 모양을 추가할 바디의 ID.
     * @param shapeDef 모양의 물리적 속성 정의.
     * @param circle 원의 반지름 및 중심.
     * @return 생성된 모양의 ID. 실패 시 b2_nullShapeId를 반환함.
     */
    b2ShapeId createCircleShape(b2BodyId bodyId, const b2ShapeDef& shapeDef, const b2Circle& circle);
    /**
     * @brief 바디에 캡슐 모양(Capsule shape)을 생성하여 추가함.
     * @param bodyId 모양을 추가할 바디의 ID.
     * @param shapeDef 모양의 물리적 속성 정의.
     * @param capsule 캡슐의 양 끝점과 반지름.
     * @return 생성된 모양의 ID. 실패 시 b2_nullShapeId를 반환함.
     */
    b2ShapeId createCapsuleShape(b2BodyId bodyId, const b2ShapeDef& shapeDef, const b2Capsule& capsule);
    /**
     * @brief 바디에 선분 모양(Segment shape)을 생성하여 추가함.
     * @param bodyId 모양을 추가할 바디의 ID.
     * @param shapeDef 모양의 물리적 속성 정의.
     * @param segment 선분의 시작점과 끝점.
     * @return 생성된 모양의 ID. 실패 시 b2_nullShapeId를 반환함.
     */
    b2ShapeId createSegmentShape(b2BodyId bodyId, const b2ShapeDef& shapeDef, const b2Segment& segment);

    // --- State Sync ---
    
    /**
     * @brief Box2D 바디의 상태를 RigidBodyComponent에 동기화함.
     * @param rb 동기화할 RigidBodyComponent 참조.
     *        Box2D 바디의 위치, 회전, 속도 등을 RigidBodyComponent에 업데이트함.
     */
    void updateRigidBodyComponent(RigidBodyComponent& rb);
    
    /**
     * @brief TransformComponent의 상태를 Box2D 바디에 반영함. (TODO: 완성하기)
     * @param tc 반영할 TransformComponent 참조.
     * @note 이 함수는 주로 초기화 시 사용하며, 런타임 중 빈번한 호출은 성능에 영향을 줄 수 있음.
     */
    void updateTransformComponent(TransformComponent& tc);

    // --- Conversion Helpers ---
    /**
     * @brief 미터 단위를 픽셀 단위로 변환함.
     * @param meters 변환할 거리 값 (미터).
     * @return 변환된 거리 값 (픽셀).
     */
    float toPixels(float meters) const { return meters * pixelsPerMeter_; }
    /**
     * @brief 미터 단위 벡터를 픽셀 단위 벡터로 변환함.
     * @param meters 변환할 벡터 (미터).
     * @return 변환된 벡터 (픽셀).
     */
    b2Vec2 toPixels(const b2Vec2& meters) const { return {meters.x * pixelsPerMeter_, meters.y * pixelsPerMeter_}; }

    /**
     * @brief 픽셀 단위를 미터 단위로 변환함.
     * @param pixels 변환할 거리 값 (픽셀).
     * @return 변환된 거리 값 (미터).
     */
    float toMeters(float pixels) const { return pixels / pixelsPerMeter_; }
    /**
     * @brief 픽셀 단위 벡터를 미터 단위 벡터로 변환함.
     * @param pixels 변환할 벡터 (픽셀).
     * @return 변환된 벡터 (미터).
     */
    b2Vec2 toMeters(const b2Vec2& pixels) const { return {pixels.x / pixelsPerMeter_, pixels.y / pixelsPerMeter_}; }

    // --- Debugging ---
    /**
     * @brief 현재 물리 세계에 있는 모든 바디의 상태를 콘솔에 출력함. (디버깅용)
     *        각 바디의 ID, 위치, 각도, 속도 등의 정보를 표시함.
     */
    void debugPrintPhysicsState();

private:
    b2WorldId worldId_ = b2_nullWorldId; // 물리 세계를 식별하는 ID
    b2Vec2 gravity_ = {0.0f, 9.8f}; // 물리 세계의 중력 벡터
    float pixelsPerMeter_ = 32.0f; // 미터와 픽셀 간의 변환 비율


    std::vector<b2BodyId> bodyIds_; 

    /**
     * @brief 관리 목록에서 특정 b2BodyId를 찾아 제거함.
     * @param id 제거할 b2BodyId.
     */
    void removeBodyId(b2BodyId id);
};
