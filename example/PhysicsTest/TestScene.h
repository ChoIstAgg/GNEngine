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
    EntityId box1;
    EntityId box2;
    EntityId box3;
    EntityId box4;
    EntityId box5;

    EntityManager& entityManager_;
    TextureManager& textureManager_;
    SoundManager& soundManager_;
};
