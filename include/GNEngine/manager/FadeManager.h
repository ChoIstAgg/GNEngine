#pragma once
#include "../GNEngine_API.h"

#include <SDL3/SDL.h>
#include <functional>

class EntityManager;

/* 
 * 화면 전체에 Fade 효과를 적용시킴.
*/
class GNEngine_API FadeManager {
public:
    FadeManager(EntityManager& entityManager);

    /* Fade effects */
    void startFadeIn(float duration, SDL_Color color = { 0, 0, 0, 255 }, std::function<void()> onComplete = nullptr);
    void startFadeOut(float duration, SDL_Color color = { 0, 0, 0, 255 }, std::function<void()> onComplete = nullptr);

private:
    EntityManager& entityManager_;
};


