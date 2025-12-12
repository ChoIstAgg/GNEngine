#include "TestScene.h"
#include "p_RootPath.h"

#include "GNEngine/component/TransformComponent.h"
#include "GNEngine/component/RenderComponent.h"
#include "GNEngine/component/RigidBodyComponent.h"
#include "GNEngine/resource/embedded/image/ImageError.h"
#include "GNEngine/component/SoundComponent.h"

#include <print>
#include <iostream>

TestScene::TestScene(EntityManager& entityManager, TextureManager& textureManager, SoundManager& soundManager)
    : entityManager_(entityManager), textureManager_(textureManager), soundManager_(soundManager) {}

void TestScene::onEnter() {
    // std::println("Entered Testscene.");
    std::cerr << "Entered TestScene.\n";

    // Create Floor
    auto floor = entityManager_.createEntity();
    entityManager_.addComponent<TransformComponent>(floor, 0, 300, 1, 1, 0);

    std::filesystem::path floorTexturePath = static_cast<std::filesystem::path>(IMAGE_ASSET_ROOT_PATH) / "example_png.png";
    textureManager_.loadTexture(floorTexturePath);
    auto floorTexture = textureManager_.getTexture(floorTexturePath);
    entityManager_.addComponent<RenderComponent>(floor, floorTexture->sdlTexture_, RenderLayer::GAME_OBJECT, false, false, 1280, 80);

    entityManager_.addComponent<RigidBodyComponent>(floor, b2Vec2(1280.0f, 80.0f), b2_staticBody);

    // Create Falling Boxes
    for (int i = 0; i < 5; ++i) {
        auto box = entityManager_.createEntity();
        entityManager_.addComponent<TransformComponent>(box,  i * 20.0f, 0, 1, 1, 0);
        // textureManager_.loadTextureEmbedded("__IMAGE_ERROR__", GNEngine::resource::embedded::imageErrorImage, GNEngine::resource::embedded::imageErrorImage_len);
        // auto boxTexture = textureManager_.getEmbeddedTexture("__IMAGE_ERROR__");
        std::filesystem::path boxTexturePath = static_cast<std::filesystem::path>(IMAGE_ASSET_ROOT_PATH) / "example_png.png";
        auto boxTexture = textureManager_.getTexture(boxTexturePath);
        entityManager_.addComponent<RenderComponent>(box, boxTexture->sdlTexture_, RenderLayer::GAME_OBJECT, false, false, 50, 50);
        entityManager_.addComponent<RigidBodyComponent>(box, b2Vec2(50.0f, 50.0f), b2_dynamicBody);
    }

     /* --- BGM --- */
    auto bgmEntity = entityManager_.createEntity();

    // sceneEntityIDs_.push_back(bgmEntity);
    std::filesystem::path bgmPath = static_cast<std::filesystem::path>(SOUND_ASSET_ROOT_PATH) / "TestMp3.mp3";
    auto bgmSound = soundManager_.getSound(bgmPath);
    if (bgmSound) {
        auto& soundComp = entityManager_.addComponent<SoundComponent>(bgmEntity);
        entityManager_.addComponent<TransformComponent>(bgmEntity);
        soundComp.addSound("bgm", bgmSound, true, 0.5f);
        soundComp.play("bgm");
    } else {
        std::cerr << "[ERROR] TestScene - Can't load bgm. \n";
    }
    // std::cerr << "[DEBUG] InGame::loadScene - bgm is successfully loaded.\n";
    
}

void TestScene::update(float deltaTime) {
    // Scene-specific update logic here
}

void TestScene::onExit() {
    // Scene-specific cleanup logic here
}
