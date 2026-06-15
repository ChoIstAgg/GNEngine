#include "Application.h"
#include "AppRootPath.h"

#include <chrono>
#include <iostream>
#include <algorithm>
#include <filesystem>

#include "GNEngine/manager/FileManager.h"

/* --- Include All Systems to use --- */
#include "GNEngine/system/AnimationSystem.h"
#include "GNEngine/system/RenderSystem.h"
#include "GNEngine/system/MovementSystem.h"
#include "GNEngine/system/InputSystem.h"
#include "GNEngine/system/SoundSystem.h"
#include "GNEngine/system/CameraSystem.h"
#include "GNEngine/system/InputToAccelerationSystem.h"
#include "GNEngine/system/PlayerAnimationControlSystem.h"
#include "GNEngine/system/FadeSystem.h"
#include "GNEngine/system/TextSystem.h"

/* --- Include All Components to use --- */
#include "GNEngine/component/SoundComponent.h"
#include "GNEngine/component/CameraComponent.h"
#include "GNEngine/component/RenderComponent.h"
#include "GNEngine/component/AccelerationComponent.h"
#include "GNEngine/component/VelocityComponent.h"
#include "GNEngine/component/InputControlComponent.h"
#include "GNEngine/component/TransformComponent.h"
#include "GNEngine/component/AnimationComponent.h"
#include "GNEngine/component/TextComponent.h"
#include "GNEngine/component/FadeComponent.h"
#include "GNEngine/component/PlayerAnimationControllerComponent.h"
#include "GNEngine/component/PlayerMovementComponent.h"

/* --- Include All Scenes to use --- */
#include "scene/LogoScene.h"
#include "scene/MainMenuScene.h"
#include "scene/InGame.h"


Application::Application() {}
Application::~Application() {
    std::cerr << "App is successfully quited.\n";
}

int Application::init(){
    /* Initialize SDL Systems*/
    if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return -1;
    }

    /* Init Managers. (Singleton pattern) */
    auto& fileManager = FileManager::getInstance();
    fileManager.init(static_cast<std::filesystem::path>(APP_ROOT_PATH) / "config.bin");

    windowWidth = std::stoi(fileManager.getSetting("windowWidth", "1920"));
    windowHeight = std::stoi(fileManager.getSetting("windowHeight", "1080"));

    window_ = SDL_CreateWindow("T.C.S", windowWidth, windowHeight, 0);
    renderer_ = SDL_CreateRenderer(window_, nullptr);
    if(!window_ || !renderer_){
        SDL_Log("Error occured in SDL_CreateWindow or SDL_CreateRenderer : %s", SDL_GetError());
        return -1;
    }

    /* Set additional settings */
    SDL_SetRenderVSync(renderer_, true); /* Enable VSync */


    /* ※Do not change the order of declarations.※ */
    /* (The order of declaration is the same as the order of destruction.) */
    /* --- Initialize all manager --- */
    entityManager_ = std::make_unique<EntityManager>();
    eventManager_ = std::make_unique<EventManager>();
    inputManager_ = std::make_unique<InputManager>(*eventManager_);
    soundManager_ = std::make_unique<SoundManager>();
    textureManager_ = std::make_unique<TextureManager>(renderer_);
    textManager_ = std::make_unique<TextManager>(renderer_);
    animationManager_ = std::make_unique<AnimationManager>(*textureManager_);
    fadeManager_ = std::make_unique<FadeManager>(*entityManager_);
    systemManager_ = std::make_unique<SystemManager>(*entityManager_);
    sceneManager_ = std::make_unique<SceneManager>();
    renderManager_ = std::make_unique<RenderManager>(renderer_, window_);

    SDL_Rect viewport = {0, 0, windowWidth, windowHeight}; //RenderManager이나 다른 클래스로 이항 예정.
    renderManager_->setViewport(viewport);

    /* --- Regist all systems --- */
    systemManager_->registerSystem<RenderSystem>(SystemPhase::RENDER, *renderManager_);
    systemManager_->registerSystem<InputSystem>(SystemPhase::PRE_UPDATE, *eventManager_, *entityManager_);
    systemManager_->registerSystem<PlayerAnimationControlSystem>(SystemPhase::LOGIC_UPDATE, *animationManager_, *textureManager_, *renderManager_);
    systemManager_->registerSystem<SoundSystem>(SystemPhase::LOGIC_UPDATE, *soundManager_);
    systemManager_->registerSystem<CameraSystem>(SystemPhase::POST_UPDATE, *renderManager_);
    systemManager_->registerSystem<AnimationSystem>(SystemPhase::POST_UPDATE);
    systemManager_->registerSystem<InputToAccelerationSystem>(SystemPhase::PRE_UPDATE, *eventManager_, *entityManager_);
    systemManager_->registerSystem<MovementSystem>(SystemPhase::PHYSICS_UPDATE);
    systemManager_->registerSystem<FadeSystem>(SystemPhase::LOGIC_UPDATE, *renderManager_);
    systemManager_->registerSystem<TextSystem>(SystemPhase::LOGIC_UPDATE, *entityManager_, *textManager_, renderer_);

    /* --- Regist all Conpontnt to use --- */
    entityManager_->registerComponentType<RenderComponent>();
    entityManager_->registerComponentType<AnimationComponent>();
    entityManager_->registerComponentType<SoundComponent>();
    entityManager_->registerComponentType<TextComponent>();
    entityManager_->registerComponentType<TransformComponent>();
    entityManager_->registerComponentType<VelocityComponent>();
    entityManager_->registerComponentType<FadeComponent>();
    entityManager_->registerComponentType<AccelerationComponent>();
    entityManager_->registerComponentType<PlayerAnimationControllerComponent>();
    entityManager_->registerComponentType<InputControlComponent>();
    entityManager_->registerComponentType<CameraComponent>();


    /* --- Regist all scenes ---*/
    sceneManager_->addScene("LogoScene", std::make_unique<LogoScene>(*entityManager_, *sceneManager_, *eventManager_, *renderManager_, *soundManager_, *textureManager_, *animationManager_, *fadeManager_));
    sceneManager_->addScene("InGame", std::make_unique<InGame>(*entityManager_, *eventManager_, *renderManager_, *soundManager_, *textureManager_, *textManager_, *animationManager_));
    //sceneManager_->addScene("MainMenuScene", std::make_unique<MainMenuScene>());


    /* Change scene */
    sceneManager_->loadScene("LogoScene");
    sceneManager_->changeScene("LogoScene");

    lastFrameTime_ = std::chrono::high_resolution_clock::now();
    return 0;
}

void Application::quit() {

    /* Save config file. */
    auto& fileManager = FileManager::getInstance();
    const std::filesystem::path configBinPath = static_cast<std::filesystem::path>(PROJECT_ROOT_PATH) /"app/data/config.bin";
    fileManager.saveSettings(configBinPath);
    //fileManager.saveLogs();

    // // Get current window size and save it
    // SDL_GetWindowSize(window_, &windowWidth, &windowHeight);
    // fileManager.setSetting("WindowWidth", std::to_string(windowWidth));
    // fileManager.setSetting("WindowHeight", std::to_string(windowHeight));
    // fileManager.saveSettings(configPath);

    std::cout << "Application - Cleaning up and quitting... " << std::endl;

    /* All Manager need no destruction */

    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);

    SDL_Quit();
}


/**
* @brief 애플리케이션의 메인 루프.
* 이벤트 처리, 업데이트, 렌더링을 반복함.
*/
void Application::run() {
    isRunning_ = true;
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg([](void* arg) {
        static_cast<Application*>(arg)->loop();
    }, this, 0, 1);
#else
    while(isRunning_) {
        loop();
    }
#endif
}

void Application::loop() {
    // std::cerr << "[DEBUG] Application::run() - Entered run loop function.\n";
    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime_).count();
    lastFrameTime_ = currentTime;

    // To prevent logic explosion by large deltaTime, clamp it.
    deltaTime = std::min(deltaTime, 0.1f);

    /* Process all events */
    if(!inputManager_->processEvents()){
        // std::cerr << "[DEBUG] Application::run() - processEvents() returned false. Exiting loop.\n";
        isRunning_ = false;
#ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
#endif
        return;
    }
    inputManager_->updateKeyStates();

    renderManager_->clear();

    /*
    * SystemManager perform in the order. {PRE_UPDATE, LOGIT_UPDATE, PHYSICS_UPDATE, POST_UPDATE, RENDER}
    */
    // std::cerr << "[DEBUG] Application::run() - Calling systemManager_->updateAll().\n";
    systemManager_->updateAll(deltaTime);
    // std::cerr << "[DEBUG] Application::run() - Finished systemManager_->updateAll().\n";

    // std::cerr << "[DEBUG] Application::run() - Calling sceneManager_->update()\n";
    sceneManager_->update(deltaTime);

    renderManager_->present();
}