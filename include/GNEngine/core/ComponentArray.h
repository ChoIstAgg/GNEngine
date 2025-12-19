#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <format>

#include "Entity.h"
#include "GNEngine/component/TransformComponent.h"
#include "GNEngine/component/VelocityComponent.h"
#include "GNEngine/component/AccelerationComponent.h"
#include "GNEngine/component/RenderComponent.h"
#include "GNEngine/component/AnimationComponent.h"
#include "GNEngine/component/TextComponent.h"
#include "GNEngine/component/CameraComponent.h"

class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void entityDestroyed(EntityId entity) = 0;
    virtual bool hasComponent(EntityId entity) const = 0;
};

template<typename T>
class ComponentArray : public IComponentArray {
public:
    void addComponent(EntityId entity, T&& component) {
        if (entityToIndexMap.count(entity)) {
            throw std::runtime_error(std::format("Component already added to entity. {}", entity));
        }
        size_t newIndex = components.size();
        entityToIndexMap[entity] = newIndex;
        indexToEntityMap[newIndex] = entity;
        components.push_back(std::move(component));
    }

    void removeComponent(EntityId entity) {
        if (!entityToIndexMap.count(entity)) {
            throw std::runtime_error(std::format("Component not found for entity. {}", entity));
        }
        size_t indexOfRemoved = entityToIndexMap[entity];
        size_t indexOfLast = components.size() - 1;
        components[indexOfRemoved] = std::move(components[indexOfLast]);
        
        EntityId entityOfLast = indexToEntityMap[indexOfLast];
        entityToIndexMap[entityOfLast] = indexOfRemoved;
        indexToEntityMap[indexOfRemoved] = entityOfLast;

        components.pop_back();
        entityToIndexMap.erase(entity);
        indexToEntityMap.erase(indexOfLast);
    }

    T& getComponent(EntityId entity) {
        if (!entityToIndexMap.count(entity)) {
            throw std::runtime_error("Component not found for entity.");
        }
        return components[entityToIndexMap.at(entity)];
    }

    bool hasComponent(EntityId entity) const override {
        return entityToIndexMap.count(entity);
    }

    void entityDestroyed(EntityId entity) override {
        if (entityToIndexMap.count(entity)) {
            removeComponent(entity);
        }
    }

protected:
    std::vector<T> components;
    std::unordered_map<EntityId, size_t> entityToIndexMap;
    std::unordered_map<size_t, EntityId> indexToEntityMap;
};

class GNEngine_API SoAComponentArray : public IComponentArray {
public:
    void entityDestroyed(EntityId entity) override;

    bool hasComponent(EntityId entity) const override {
        return entityToIndexMap.count(entity);
    }

    const std::unordered_map<EntityId, size_t>& getEntityToIndexMap() const {
        return entityToIndexMap;
    }

protected:
    virtual void swapAndPop(size_t indexOfRemoved, size_t indexOfLast) = 0;

    std::unordered_map<EntityId, size_t> entityToIndexMap;
    std::unordered_map<size_t, EntityId> indexToEntityMap;
};

template<>
class ComponentArray<TransformComponent> : public SoAComponentArray {
public:
    void addComponent(EntityId entity, TransformComponent&& component) {
        size_t index;
        auto it = entityToIndexMap.find(entity);
        if (it == entityToIndexMap.end()) {
            index = indexToEntityMap.size();
            entityToIndexMap[entity] = index;
            indexToEntityMap[index] = entity;
        } else {
            index = it->second;
        }

        if (index >= positionX.size()) {
            positionX.resize(index + 1);
            positionY.resize(index + 1);
            scaleX.resize(index + 1);
            scaleY.resize(index + 1);
            rotatedAngle.resize(index + 1);
        }

        positionX[index] = component.positionX_;
        positionY[index] = component.positionY_;
        scaleX[index] = component.scaleX_;
        scaleY[index] = component.scaleY_;
        rotatedAngle[index] = component.angle_;
    }

    void removeComponent(EntityId entity) {
        if (!entityToIndexMap.count(entity)) {
            //throw std::runtime_error(std::format("Component not found for entity. {}", entity));
            return; // 예외 대신 반환하여 처리하지 않음.
        }
        size_t indexOfRemoved = entityToIndexMap[entity];
        size_t indexOfLast = indexToEntityMap.size() - 1;

        swapAndPop(indexOfRemoved, indexOfLast);
        
        EntityId entityOfLast = indexToEntityMap[indexOfLast];
        entityToIndexMap[entityOfLast] = indexOfRemoved;
        indexToEntityMap[indexOfRemoved] = entityOfLast;

        entityToIndexMap.erase(entity);
        indexToEntityMap.erase(indexOfLast);
    }

    TransformComponent getComponent(EntityId entity) {
        if (!entityToIndexMap.count(entity)) {
            throw std::runtime_error("TransformComponent not found for entity.");
        }
        size_t index = entityToIndexMap.at(entity);
        return TransformComponent{
            positionX[index],
            positionY[index],
            scaleX[index],
            scaleY[index],
            rotatedAngle[index]
        };
    }

    // T& getComponent(EntityId entity) { /* For systems that need to directly modify SoA data */}
    // 이 메서드는 SoA의 각 벡터에 대한 참조를 반환하도록 변경해야 할 수도 있습니다.

    std::vector<float> positionX;
    std::vector<float> positionY;
    std::vector<float> scaleX;
    std::vector<float> scaleY;
    std::vector<float> rotatedAngle;

protected:
    void swapAndPop(size_t indexOfRemoved, size_t indexOfLast) override {
        positionX[indexOfRemoved] = positionX[indexOfLast];
        positionY[indexOfRemoved] = positionY[indexOfLast];
        scaleX[indexOfRemoved] = scaleX[indexOfLast];
        scaleY[indexOfRemoved] = scaleY[indexOfLast];
        rotatedAngle[indexOfRemoved] = rotatedAngle[indexOfLast];

        positionX.pop_back();
        positionY.pop_back();
        scaleX.pop_back();
        scaleY.pop_back();
        rotatedAngle.pop_back();
    }
};

template<>
class ComponentArray<VelocityComponent> : public SoAComponentArray {
public:
    void addComponent(EntityId entity, VelocityComponent&& component) {
        size_t index;
        auto it = entityToIndexMap.find(entity);
        if (it == entityToIndexMap.end()) {
            index = indexToEntityMap.size();
            entityToIndexMap[entity] = index;
            indexToEntityMap[index] = entity;
        } else {
            index = it->second;
        }

        if (index >= vx.size()) {
            vx.resize(index + 1);
            vy.resize(index + 1);
        }
        vx[index] = component.vx;
        vy[index] = component.vy;
    }

    void removeComponent(EntityId entity) { /* Stub */ }

    VelocityComponent getComponent(EntityId entity) {
        if (!entityToIndexMap.count(entity)) {
            throw std::runtime_error("VelocityComponent not found for entity.");
        }
        size_t index = entityToIndexMap.at(entity);
        return VelocityComponent{vx[index], vy[index]};
    }

    std::vector<float> vx;
    std::vector<float> vy;

protected:
    void swapAndPop(size_t indexOfRemoved, size_t indexOfLast) override {
        vx[indexOfRemoved] = vx[indexOfLast];
        vy[indexOfRemoved] = vy[indexOfLast];
        vx.pop_back();
        vy.pop_back();
    }
};

template<>
class ComponentArray<AccelerationComponent> : public SoAComponentArray {
public:
    void addComponent(EntityId entity, AccelerationComponent&& component) {
        size_t index;
        auto it = entityToIndexMap.find(entity);
        if (it == entityToIndexMap.end()) {
            index = indexToEntityMap.size();
            entityToIndexMap[entity] = index;
            indexToEntityMap[index] = entity;
        } else {
            index = it->second;
        }

        if (index >= ax.size()) {
            ax.resize(index + 1);
            ay.resize(index + 1);
        }
        ax[index] = component.ax;
        ay[index] = component.ay;
    }

    void removeComponent(EntityId entity) { /* Stub */ }

    AccelerationComponent getComponent(EntityId entity) {
        if (!entityToIndexMap.count(entity)) {
            throw std::runtime_error("AccelerationComponent not found for entity.");
        }
        size_t index = entityToIndexMap.at(entity);
        return AccelerationComponent{ax[index], ay[index]};
    }

    std::vector<float> ax;
    std::vector<float> ay;

protected:
    void swapAndPop(size_t indexOfRemoved, size_t indexOfLast) override {
        ax[indexOfRemoved] = ax[indexOfLast];
        ay[indexOfRemoved] = ay[indexOfLast];
        ax.pop_back();
        ay.pop_back();
    }
};


template<>
class ComponentArray<RenderComponent> : public SoAComponentArray {
public:
    void addComponent(EntityId entity, RenderComponent&& component) {
        size_t index;
        auto it = entityToIndexMap.find(entity);
        if (it == entityToIndexMap.end()) {
            index = indexToEntityMap.size();
            entityToIndexMap[entity] = index;
            indexToEntityMap[index] = entity;
        } else {
            index = it->second;
        }

        if (index >= sdlTextures.size()) {
            sdlTextures.resize(index + 1);
            widths.resize(index + 1);
            heights.resize(index + 1);
            layers.resize(index + 1);
            isScreenSpace.resize(index + 1);
            hasAnimations.resize(index + 1);
            srcRectX.resize(index + 1);
            srcRectY.resize(index + 1);
            srcRectW.resize(index + 1);
            srcRectH.resize(index + 1);
            flipX.resize(index + 1);
            flipY.resize(index + 1);
        }

        sdlTextures[index] = component.getSDLTexture();
        layers[index] = component.getLayer();
        widths[index] = component.getWidth();
        heights[index] = component.getHeight();
        hasAnimations[index] = component.hasAnimation();
        isScreenSpace[index] = component.isScreenSpace();
        const auto& rect = component.getSrcRect();
        srcRectX[index] = rect.x;
        srcRectY[index] = rect.y;
        srcRectW[index] = rect.w;
        srcRectH[index] = rect.h;
        flipX[index] = component.getFlipX();
        flipY[index] = component.getFlipY();
    }

    void removeComponent(EntityId entity) { /* Stub */ }
    RenderComponent getComponent(EntityId entity) {
        if (!entityToIndexMap.count(entity)) {
            throw std::runtime_error(std::format("RenderComponent not found for entity({}).", entity));
        }
        size_t i = entityToIndexMap.at(entity);
        return RenderComponent(sdlTextures[i], layers[i], isScreenSpace[i], hasAnimations[i], widths[i], heights[i], {srcRectX[i], srcRectY[i], srcRectW[i], srcRectH[i]}, flipX[i], flipY[i]);
    }

    void updateTexture(EntityId entity, SDL_Texture* texture, int width, int height) {
        if (!entityToIndexMap.count(entity)) {
            return; // Or throw an exception
        }
        size_t i = entityToIndexMap.at(entity);

        // Destroy the old texture if it exists to prevent leaks
        if (sdlTextures[i] != nullptr) {
            SDL_DestroyTexture(sdlTextures[i]);
        }

        sdlTextures[i] = texture;
        widths[i] = width;
        heights[i] = height;
        srcRectX[i] = 0;
        srcRectY[i] = 0;
        srcRectW[i] = width;
        srcRectH[i] = height;
    }

    std::vector<SDL_Texture*> sdlTextures;
    std::vector<int> widths;
    std::vector<int> heights;
    std::vector<RenderLayer> layers;
    std::vector<bool> hasAnimations;
    std::vector<int> srcRectX, srcRectY, srcRectW, srcRectH;
    std::vector<bool> flipX, flipY;
    std::vector<bool> isScreenSpace;

protected:
    void swapAndPop(size_t indexOfRemoved, size_t indexOfLast) override {
        if (sdlTextures[indexOfRemoved] != nullptr) {
            SDL_DestroyTexture(sdlTextures[indexOfRemoved]);
        }
        sdlTextures[indexOfRemoved] = sdlTextures[indexOfLast];
        layers[indexOfRemoved] = layers[indexOfLast];
        widths[indexOfRemoved] = widths[indexOfLast];
        heights[indexOfRemoved] = heights[indexOfLast];
        hasAnimations[indexOfRemoved] = hasAnimations[indexOfLast];
        isScreenSpace[indexOfRemoved] = isScreenSpace[indexOfLast];
        srcRectX[indexOfRemoved] = srcRectX[indexOfLast];
        srcRectY[indexOfRemoved] = srcRectY[indexOfLast];
        srcRectW[indexOfRemoved] = srcRectW[indexOfLast];
        srcRectH[indexOfRemoved] = srcRectH[indexOfLast];
        flipX[indexOfRemoved] = flipX[indexOfLast];
        flipY[indexOfRemoved] = flipY[indexOfLast];

        sdlTextures.pop_back();
        layers.pop_back();
        widths.pop_back();
        heights.pop_back();
        isScreenSpace.pop_back();
        hasAnimations.pop_back();
        srcRectX.pop_back();
        srcRectY.pop_back();
        srcRectW.pop_back();
        srcRectH.pop_back();
        flipX.pop_back();
        flipY.pop_back();
    }
};

template<>
class ComponentArray<AnimationComponent> : public SoAComponentArray {
public:
    void addComponent(EntityId entity, AnimationComponent&& component) {
        size_t index;
        auto it = entityToIndexMap.find(entity);
        if (it == entityToIndexMap.end()) {
            index = indexToEntityMap.size();
            entityToIndexMap[entity] = index;
            indexToEntityMap[index] = entity;
        } else {
            index = it->second;
        }

        if (index >= animations.size()) {
            animations.resize(index + 1);
            currentFrames.resize(index + 1);
            frameTimers.resize(index + 1);
            arePlaying.resize(index + 1);
            areFinished.resize(index + 1);
        }

        animations[index] = component.animation_;
        currentFrames[index] = component.currentFrame_;
        frameTimers[index] = component.frameTimer_;
        arePlaying[index] = component.isPlaying_;
        areFinished[index] = component.isFinished_;
    }

    void removeComponent(EntityId entity) { /* Stub */ }

    AnimationComponent getComponent(EntityId entity) {
        if (!entityToIndexMap.count(entity)) {
            throw std::runtime_error("AnimationComponent not found for entity.");
        }
        size_t i = entityToIndexMap.at(entity);
        AnimationComponent comp(animations[i]);
        comp.currentFrame_ = currentFrames[i];
        comp.frameTimer_ = frameTimers[i];
        comp.isPlaying_ = arePlaying[i];
        comp.isFinished_ = areFinished[i];
        return comp;
    }

    std::vector<std::shared_ptr<Animation>> animations;
    std::vector<int> currentFrames;
    std::vector<float> frameTimers;
    std::vector<bool> arePlaying;
    std::vector<bool> areFinished;

protected:
    void swapAndPop(size_t indexOfRemoved, size_t indexOfLast) override {
        animations[indexOfRemoved] = std::move(animations[indexOfLast]);
        currentFrames[indexOfRemoved] = currentFrames[indexOfLast];
        frameTimers[indexOfRemoved] = frameTimers[indexOfLast];
        arePlaying[indexOfRemoved] = arePlaying[indexOfLast];
        areFinished[indexOfRemoved] = areFinished[indexOfLast];

        animations.pop_back();
        currentFrames.pop_back();
        frameTimers.pop_back();
        arePlaying.pop_back();
        areFinished.pop_back();
    }
};

template<>
class ComponentArray<TextComponent> : public SoAComponentArray {
public:
    void addComponent(EntityId entity, TextComponent&& component) {
        size_t index;
        auto it = entityToIndexMap.find(entity);
        if (it == entityToIndexMap.end()) {
            index = indexToEntityMap.size();
            entityToIndexMap[entity] = index;
            indexToEntityMap[index] = entity;
        } else {
            index = it->second;
        }

        if (index >= texts.size()) {
            texts.resize(index + 1);
            fontPaths.resize(index + 1);
            fontSizes.resize(index + 1);
            colorsR.resize(index + 1);
            colorsG.resize(index + 1);
            colorsB.resize(index + 1);
            colorsA.resize(index + 1);
            areDirty.resize(index + 1);
            layers.resize(index + 1);
        }

        texts[index] = std::move(component.text);
        fontPaths[index] = std::move(component.fontPath);
        fontSizes[index] = component.fontSize;
        colorsR[index] = component.color.r;
        colorsG[index] = component.color.g;
        colorsB[index] = component.color.b;
        colorsA[index] = component.color.a;
        areDirty[index] = component.isDirty;
        layers[index] = component.layer;
    }

    void removeComponent(EntityId entity) { /* Stub */ }

    TextComponent getComponent(EntityId entity) {
        if (!entityToIndexMap.count(entity)) {
            throw std::runtime_error("TextComponent not found for entity.");
        }
        size_t i = entityToIndexMap.at(entity);
        TextComponent comp(texts[i], fontPaths[i], fontSizes[i], {colorsR[i], colorsG[i], colorsB[i], colorsA[i]}, layers[i]);
        comp.isDirty = areDirty[i];
        return comp;
    }

    void setDirty(EntityId entity, bool isDirty) {
        if (!entityToIndexMap.count(entity)) {
            return; // Or throw an exception
        }
        size_t i = entityToIndexMap.at(entity);
        areDirty[i] = isDirty;
    }

    std::vector<std::string> texts;
    std::vector<std::filesystem::path> fontPaths;
    std::vector<int> fontSizes;
    std::vector<Uint8> colorsR, colorsG, colorsB, colorsA;
    std::vector<bool> areDirty;
    std::vector<RenderLayer> layers;

protected:
    void swapAndPop(size_t i, size_t last_i) override {
        texts[i] = std::move(texts[last_i]);
        fontPaths[i] = std::move(fontPaths[last_i]);
        fontSizes[i] = fontSizes[last_i];
        colorsR[i] = colorsR[last_i];
        colorsG[i] = colorsG[last_i];
        colorsB[i] = colorsB[last_i];
        colorsA[i] = colorsA[last_i];
        areDirty[i] = areDirty[last_i];
        layers[i] = layers[last_i];

        texts.pop_back();
        fontPaths.pop_back();
        fontSizes.pop_back();
        colorsR.pop_back();
        colorsG.pop_back();
        colorsB.pop_back();
        colorsA.pop_back();
        areDirty.pop_back();
        layers.pop_back();
    }
};

template<>
class ComponentArray<CameraComponent> : public SoAComponentArray {
public:
    void addComponent(EntityId entity, CameraComponent&& component) {
        size_t index;
        auto it = entityToIndexMap.find(entity);
        if (it == entityToIndexMap.end()) {
            index = indexToEntityMap.size();
            entityToIndexMap[entity] = index;
            indexToEntityMap[index] = entity;
        } else {
            index = it->second;
        }

        if (index >= x.size()) {
            x.resize(index + 1);
            y.resize(index + 1);
            zoom.resize(index + 1);
            targetEntityIds.resize(index + 1);
        }

        x[index] = component.x;
        y[index] = component.y;
        zoom[index] = component.zoom;
        targetEntityIds[index] = component.targetEntityId;
    }

    void removeComponent(EntityId entity) { /* Stub */ }

    CameraComponent getComponent(EntityId entity) {
        if (!entityToIndexMap.count(entity)) {
            throw std::runtime_error("CameraComponent not found for entity.");
        }
        size_t i = entityToIndexMap.at(entity);
        return CameraComponent(x[i], y[i], zoom[i], targetEntityIds[i]);
    }

    std::vector<float> x;
    std::vector<float> y;
    std::vector<float> zoom;
    std::vector<EntityId> targetEntityIds;

protected:
    void swapAndPop(size_t indexOfRemoved, size_t indexOfLast) override {
        x[indexOfRemoved] = x[indexOfLast];
        y[indexOfRemoved] = y[indexOfLast];
        zoom[indexOfRemoved] = zoom[indexOfLast];
        targetEntityIds[indexOfRemoved] = targetEntityIds[indexOfLast];

        x.pop_back();
        y.pop_back();
        zoom.pop_back();
        targetEntityIds.pop_back();
    }
};