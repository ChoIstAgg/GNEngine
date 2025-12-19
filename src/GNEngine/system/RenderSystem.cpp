#include "GNEngine/system/RenderSystem.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <SDL3/SDL_render.h>

#include "GNEngine/core/Entity.h"
#include "GNEngine/core/RenderLayer.h"
#include "GNEngine/component/FadeComponent.h"

RenderSystem::RenderSystem(RenderManager& renderManager)
    : renderManager_(renderManager) {}


/*
 * TransformComponent와 RenderComponent를 가진 엔티티를 렌더링 계층 순서대로 렌더링함. 
*/
void RenderSystem::update(EntityManager& entityManager, float deltaTime) {
    // 이 함수 내에서만 사용할 로컬 구조체
    struct Renderable {
        EntityId entity;
        RenderLayer layer;
    };

    // 정렬용 람다
    auto compareRenderables = [](const Renderable& a, const Renderable& b) {
        return static_cast<int>(a.layer) < static_cast<int>(b.layer);
    };

    std::vector<Renderable> renderables;

    // 1. RenderComponent가 있는 엔티티 수집
    auto renderArray = entityManager.getComponentArray<RenderComponent>();
    if (renderArray) {
        for (const auto& entity : entityManager.getEntitiesWith<RenderComponent, TransformComponent>()) {
            const auto& renderComponent = renderArray->getComponent(entity);
            renderables.push_back({entity, renderComponent.getLayer()});
        }
    }

    // 2. 레이어 기준으로 모든 렌더링 대상 정렬
    std::sort(renderables.begin(), renderables.end(), compareRenderables);

    // 3. 정렬된 순서대로 렌더링
    auto transformArray = entityManager.getComponentArray<TransformComponent>();
    auto animArray = entityManager.getComponentArray<AnimationComponent>();
    auto fadeArray = entityManager.getComponentArray<FadeComponent>();

    for (const auto& renderable : renderables) {
        EntityId entity = renderable.entity;

        // TransformComponent
        if (!transformArray->hasComponent(entity)) continue;
        const auto& transform = transformArray->getComponent(entity);

        // RenderComponent 처리
        if (renderArray && renderArray->hasComponent(entity)) {
            const auto& render = renderArray->getComponent(entity);
            if (render.getSDLTexture()) {
                SDL_Rect srcRect = render.getSrcRect();
                float destW = static_cast<float>(render.getWidth()) * transform.scaleX_;
                float destH = static_cast<float>(render.getHeight()) * transform.scaleY_;

                if (render.hasAnimation() && animArray && animArray->hasComponent(entity)) {
                    const auto& anim = animArray->getComponent(entity);
                    if (anim.animation_) { // animation_이 유효한지 확인
                        srcRect = anim.animation_->getFrame(anim.currentFrame_); // currentFrame_ 사용
                        destW = static_cast<float>(srcRect.w) * transform.scaleX_;
                        destH = static_cast<float>(srcRect.h) * transform.scaleY_;
                    }
                }

                SDL_FlipMode flip = SDL_FLIP_NONE;
                if (render.getFlipX()) flip = static_cast<SDL_FlipMode>(flip | SDL_FLIP_HORIZONTAL);
                if (render.getFlipY()) flip = static_cast<SDL_FlipMode>(flip | SDL_FLIP_VERTICAL);

                if (render.isScreenSpace()) {
                    renderManager_.renderUITexture(render.getSDLTexture(), transform.positionX_, transform.positionY_, &srcRect, destW, destH, flip);
                } else {
                    renderManager_.renderTexture(render.getSDLTexture(), transform.positionX_, transform.positionY_, &srcRect, destW, destH, flip);
                }

                // SDL_Log("RenderSystem::update - Rendering texture at (%.2f, %.2f) with size (%.2f, %.2f)", transform.positionX_, transform.positionY_, destW, destH);
            } else {
                // TODO 4 - 아래 로직 삭제. imageError 이미지를 대신 렌더링하게 하기. 
                // 임시 : 텍스처가 없는 RenderComponent는 페이드 효과로 간주 
                if (fadeArray && fadeArray->hasComponent(entity)) {
                    const auto& fade = fadeArray->getComponent(entity);
                    SDL_Renderer* renderer = renderManager_.getRenderer();
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                    SDL_SetRenderDrawColor(renderer, fade.color.r, fade.color.g, fade.color.b, static_cast<Uint8>(fade.currentAlpha));

                    int w, h;
                    SDL_GetRenderOutputSize(renderer, &w, &h);
                    SDL_FRect fadeRect = { 0, 0, static_cast<float>(w), static_cast<float>(h) };

                    SDL_RenderFillRect(renderer, &fadeRect);
                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
                }
            }
        }
    }
}


