#pragma once
#include "GNEngine/core/Scene.h"
#include "GNEngine/manager/EntityManager.h"
#include "GNEngine/manager/TextureManager.h"
#include "GNEngine/manager/SoundManager.h"

class TestScene : public Scene {
public:
    TestScene(EntityManager& entityManager, TextureManager& textureManager, SoundManager& soundManager);
    void onEnter() override;
    void update(float deltaTime) override;
    void onExit() override;

private:
    EntityID box1;
    EntityID box2;
    EntityID box3;
    EntityID box4;
    EntityID box5;

    EntityManager& entityManager_;
    TextureManager& textureManager_;
    SoundManager& soundManager_;
};
