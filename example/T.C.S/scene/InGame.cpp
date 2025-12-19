#include "InGame.h"
#include "AppRootPath.h"

#include <iostream>
#include <string>
#include <filesystem>

/* --- Managers --- */
#include "GNEngine/manager/EntityManager.h"
#include "GNEngine/manager/EventManager.h"
#include "GNEngine/manager/RenderManager.h"
#include "GNEngine/manager/TextureManager.h"
#include "GNEngine/manager/SoundManager.h"
#include "GNEngine/manager/AnimationManager.h"
#include "GNEngine/manager/TextManager.h"

/* --- Components --- */
#include "GNEngine/component/TransformComponent.h"
#include "GNEngine/component/RenderComponent.h"
#include "GNEngine/component/CameraComponent.h"
#include "GNEngine/component/SoundComponent.h"
#include "GNEngine/component/InputControlComponent.h"
#include "GNEngine/component/PlayerMovementComponent.h"
#include "GNEngine/component/PlayerAnimationControllerComponent.h"

/* --- Prefabs --- */
#include "../prefab/PlayerPrefab.h"

InGame::InGame(EntityManager& entityManager,
                    EventManager& eventManager,
                    RenderManager& renderManager,
                    SoundManager& soundManager,
                    TextureManager& textureManager,
                    TextManager& textManager,
                    AnimationManager& animationManager)
    : entityManager_(entityManager),
    eventManager_(eventManager),
    renderManager_(renderManager),
    textureManager_(textureManager),
    textManager_(textManager),
    soundManager_(soundManager),
    animationManager_(animationManager)
{}

bool InGame::loadScene() {
    /* --- Player --- */
    playerEntity_ = PlayerPrefab::create(entityManager_, eventManager_, textureManager_, renderManager_, soundManager_, animationManager_);
    sceneEntityIDs_.push_back(playerEntity_);
    // std::cerr << "[DEBUG] InGame::loadScene -  Player is successfully loaded.\n";
    
    /* --- Camera --- */
    cameraEntity_ = entityManager_.createEntity();
    entityManager_.addComponent<CameraComponent>(cameraEntity_, playerEntity_, 1.0f);
    sceneEntityIDs_.push_back(cameraEntity_);
    // std::cerr << "[DEBUG] InGame::loadScene - Camera is successfully loaded.\n";
    
    /* --- BGM --- */
    auto bgmEntity = entityManager_.createEntity();
    sceneEntityIDs_.push_back(bgmEntity);
    std::filesystem::path bgmPath = static_cast<std::filesystem::path>(SOUND_ASSET_ROOT_PATH) / "TestMp3.mp3";
    auto bgmSound = soundManager_.getSound(bgmPath);
    if (bgmSound) {
        auto& soundComp = entityManager_.addComponent<SoundComponent>(bgmEntity);
        entityManager_.addComponent<TransformComponent>(bgmEntity);
        soundComp.addSound("bgm", bgmSound, true, 0.5f);
        soundComp.play("bgm");
    } else {
        std::cerr << "[ERROR] InGame - Can't load bgm. \n";
    }
    // std::cerr << "[DEBUG] InGame::loadScene - bgm is successfully loaded.\n";
    
    // Add components to the existing textEntity
    std::filesystem::path fontPath = static_cast<std::filesystem::path>(APP_ROOT_PATH) / "asset" / "font" / "CookieRun Regular.ttf";
    if (!textManager_.loadFont(fontPath, 24)) {
        std::cerr << "[ERROR] InGame - Failed to load font: " << fontPath << std::endl;
    }
    
    EntityId textEntity = entityManager_.createEntity();
    sceneEntityIDs_.push_back(textEntity);
    std::filesystem::path textPath = static_cast<std::filesystem::path>(TEXT_ASSET_ROOT_PATH) / "test.txt";
    std::string text = textManager_.loadTextFromFile(textPath);
    SDL_Color textColor = {255, 255, 255, 255};
    entityManager_.addComponent<TextComponent>(textEntity, text, fontPath, 24, textColor, RenderLayer::UI);
    entityManager_.addComponent<TransformComponent>(textEntity, 100.0f, 50.0f, 1.0f, 1.0f, 1.0f);
    entityManager_.addComponent<RenderComponent>(textEntity, RenderLayer::UI, true);

    // auto imageErrorImageEntity = entityManager_.createEntity();
    // sceneEntityIDs_.push_back(imageErrorImageEntity);
    // Texture* imageErrorImage = textureManager_.getEmbeddedTexture("__IMAGE_ERROR__");
    // entityManager_.addComponent<TransformComponent>(imageErrorImageEntity);
    // entityManager_.addComponent<RenderComponent>(imageErrorImageEntity, RenderLayer::GAME_OBJECT);

    EntityId exampleEntity = entityManager_.createEntity();
    sceneEntityIDs_.push_back(exampleEntity);
    std::filesystem::path texturePath = static_cast<std::filesystem::path>(IMAGE_ASSET_ROOT_PATH) / "example_png.png";
    Texture* exampleTexture = textureManager_.getTexture(texturePath);
    if (exampleTexture)
    {
        entityManager_.addComponent<RenderComponent>(exampleEntity, exampleTexture->sdlTexture_, RenderLayer::GAME_OBJECT, false, false, exampleTexture->width_, exampleTexture->height_);
        entityManager_.addComponent<TransformComponent>(exampleEntity, 100.0f, 100.0f);
    }

    isLoaded_ = true;
    return true;
}

void InGame::onEnter() {
    if (!isLoaded_) {
        loadScene();
    }
    renderManager_.setBackgroundColor({0, 0, 0, 255}); // black
    std::cerr << "InGame::onEnter()" << std::endl;
}

void InGame::onExit() {
    for (auto entity : sceneEntityIDs_) {
        entityManager_.destroyEntity(entity);
    }
    sceneEntityIDs_.clear();
    isLoaded_ = false;
    std::cerr << "InGame::onExit()" << std::endl;
}

void InGame::update(float deltaTime) {
    // This scene might not have complex update logic itself
}

