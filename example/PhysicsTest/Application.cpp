#include "Application.h"
#include "p_RootPath.h"

#include <chrono>
#include <iostream>
#include <algorithm>
#include <filesystem>

#include "GNEngine/manager/FileManager.h"

/* --- Include All Systems to use --- */
#include "GNEngine/system/RenderSystem.h"
#include "GNEngine/system/InputSystem.h"
#include "GNEngine/system/PhysicsSystem.h"

/* --- Include All Components to use --- */
#include "GNEngine/component/RigidBodyComponent.h"
#include "GNEngine/component/TransformComponent.h"
#include "GNEngine/component/RenderComponent.h"
#include "GNEngine/component/SoundComponent.h"

/* --- Include All Scenes to use --- */
#include "TestScene.h"


Application::Application() {}
Application::~Application() {
    std::cerr << "App is successfully quited.\n";
}

int Application::init(){
    /* Initialize SDL Systems*/
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return -1;
    }

    windowWidth_ = 1280;
    windowHeight_ = 720;

    window_ = SDL_CreateWindow("PhysicsTest", windowWidth_, windowHeight_, 0);
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
    physicsManager_ = std::make_unique<PhysicsManager>();
    systemManager_ = std::make_unique<SystemManager>(*entityManager_);
    sceneManager_ = std::make_unique<SceneManager>();
    renderManager_ = std::make_unique<RenderManager>(renderer_, window_);
    
    physicsManager_->init(); 
    physicsManager_->setGravity(b2Vec2(0.0f, 9.8f));

    /* --- Regist all systems --- */
    systemManager_->registerSystem<RenderSystem>(SystemPhase::RENDER, *renderManager_);
    systemManager_->registerSystem<InputSystem>(SystemPhase::PRE_UPDATE, *eventManager_, *entityManager_);
    systemManager_->registerSystem<PhysicsSystem>(SystemPhase::PHYSICS_UPDATE, *entityManager_, *physicsManager_, *eventManager_);

    /* --- Regist all Conpontnt to use --- */
    entityManager_->registerComponentType<RenderComponent>();
    entityManager_->registerComponentType<TransformComponent>();
    entityManager_->registerComponentType<RigidBodyComponent>();
    entityManager_->registerComponentType<SoundComponent>();

    /* --- Regist all scenes ---*/
    sceneManager_->addScene("TestScene", std::make_unique<TestScene>(*entityManager_, *textureManager_, *soundManager_));

    /* Change scene */
    sceneManager_->loadScene("TestScene");
    sceneManager_->changeScene("TestScene");

    lastFrameTime_ = std::chrono::high_resolution_clock::now();
    return 0;
}

void Application::quit() {
    std::cout << "Application - Cleaning up and quitting... " << std::endl;

    physicsManager_->shutdown();

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
    while(isRunning_) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastFrameTime_).count();
        lastFrameTime_ = currentTime;

        deltaTime = std::min(deltaTime, 0.1f);

        /* Process all events */
        if(!inputManager_->processEvents()){
            isRunning_ = false;
            break;
        }
        inputManager_->updateKeyStates();

        renderManager_->clear();

        systemManager_->updateAll(deltaTime);
        
        sceneManager_->update(deltaTime);

        renderManager_->present();
    }
}
