#include "GNEngine/system/CameraSystem.h"
#include <iostream>
#include <SDL3/SDL_log.h>

CameraSystem::CameraSystem(RenderManager& renderManager)
    : renderManager_(renderManager) {}

void CameraSystem::update(EntityManager& entityManager, float deltaTime) {
    int windowWidth = renderManager_.getWindowWidth();
    int windowHeight = renderManager_.getWindowHeight();

    auto cameraArray = entityManager.getComponentArray<CameraComponent>();
    auto transformArray = entityManager.getComponentArray<TransformComponent>();

    if (!cameraArray || !transformArray) {
        return;
    }

    auto& cameraX = cameraArray->x;
    auto& cameraY = cameraArray->y;
    auto& cameraZoom = cameraArray->zoom;
    auto& targetEntityIds = cameraArray->targetEntityIds;

    auto& transformX = transformArray->positionX;
    auto& transformY = transformArray->positionY;

    for (EntityId entity : entityManager.getEntitiesWith<CameraComponent>()) {
        const size_t cameraIndex = cameraArray->getEntityToIndexMap().at(entity);
        // SDL_Log("CameraSystem: entity=%u, cameraIndex=%zu", entity, cameraIndex);

        EntityId targetId = targetEntityIds[cameraIndex];
        if (targetId != 0) {
            // SDL_Log("CameraSystem: Target entity ID is %u.", targetId);
            
            if (transformArray->hasComponent(targetId)) {
                const size_t targetTransformIndex = transformArray->getEntityToIndexMap().at(targetId);
                // SDL_Log("CameraSystem: targetId=%u, targetTransformIndex=%zu", targetId, targetTransformIndex);
                
                // 카메라를 타겟 엔티티의 위치로 이동 (간단한 따라가기 로직)
                cameraX[cameraIndex] = transformX[targetTransformIndex];
                cameraY[cameraIndex] = transformY[targetTransformIndex];

                // RenderManager에 카메라 위치 업데이트
                renderManager_.setCameraPosition(cameraX[cameraIndex], cameraY[cameraIndex]);
                renderManager_.setZoomLevel(cameraZoom[cameraIndex]);
                // SDL_Log("CameraSystem: Camera position set to (%.2f, %.2f) and zoom set to %.2f for target entity %u at (%.2f, %.2f).", cameraX[cameraIndex], cameraY[cameraIndex], cameraZoom[cameraIndex], targetId, transformX[targetTransformIndex], transformY[targetTransformIndex]);

                // TODO: 부드러운 카메라 이동, 경계 처리, 줌 레벨 조정 등 추가 로직 구현
            } else {
                // SDL_Log("CameraSystem: Target entity %u does not have TransformComponent.", targetId);
            }
        } else {
            // SDL_Log("CameraSystem: No target entity set for camera %u.", entity);
        }
        
        // TODO: 줌 레벨 조정 로직 등 추가
    }
}


