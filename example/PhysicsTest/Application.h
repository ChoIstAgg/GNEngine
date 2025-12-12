#pragma once
#include <SDL3/SDL.h>
#include <memory>
#include <chrono>

#include "GNEngine/manager/EntityManager.h"
#include "GNEngine/manager/EventManager.h"
#include "GNEngine/manager/InputManager.h"
#include "GNEngine/manager/SoundManager.h"
#include "GNEngine/manager/TextureManager.h"
#include "GNEngine/manager/TextManager.h"
#include "GNEngine/manager/AnimationManager.h"
#include "GNEngine/manager/FadeManager.h"
#include "GNEngine/manager/SystemManager.h"
#include "GNEngine/manager/SceneManager.h"
#include "GNEngine/manager/RenderManager.h"
#include "GNEngine/manager/PhysicsManager.h"

class Application {
public:
    Application();
    ~Application();

    int init();
    void run();
    void quit();

private:
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    
    int windowWidth_;
    int windowHeight_;

    bool isRunning_ = false;

    std::chrono::high_resolution_clock::time_point lastFrameTime_;
    
    std::unique_ptr<EntityManager> entityManager_;
    std::unique_ptr<EventManager> eventManager_;
    std::unique_ptr<InputManager> inputManager_;
    std::unique_ptr<SoundManager> soundManager_;
    std::unique_ptr<TextureManager> textureManager_;
    std::unique_ptr<TextManager> textManager_;
    std::unique_ptr<AnimationManager> animationManager_;
    std::unique_ptr<FadeManager> fadeManager_;
    std::unique_ptr<SystemManager> systemManager_;
    std::unique_ptr<SceneManager> sceneManager_;
    std::unique_ptr<RenderManager> renderManager_;
    std::unique_ptr<PhysicsManager> physicsManager_;
};
