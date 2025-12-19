#pragma once
#include "../GNEngine_API.h"

#include "GNEngine/core/Component.h"
#include "GNEngine/core/Texture.h"
#include "GNEngine/core/RenderLayer.h"
#include "GNEngine/manager/RenderManager.h"
#include "GNEngine/component/TransformComponent.h"
#include "GNEngine/component/AnimationComponent.h"

/**
 * @class RenderComponent
 * @brief 게임 오브젝트를 렌더링하는 데 필요한 데이터를 담는 컴포넌트임.
 * @param texture 렌더링할 텍스처 포인터.
 * @param layer(RenderLayer::GAME_OBJECT) 렌더링 레이어.
 * @param isScreenSpace(false) UI처럼 화면 공간에 고정할지 여부.
 * @param hasAnimation(false) 애니메이션이 있는지 여부.
 * @param width(0) 0이면 텍스처 기본 크기로 렌더링함.
 * @param height(0) 0이면 텍스처 기본 크기로 렌더링함.
 * @param srcRect({0-0-0-0}) 텍스처의 원본 사각형 영역.
 * @param flipX(false) x축으로 뒤집을지 여부.
 * @param flipY(false) y축으로 뒤집을지 여부.
*/
class GNEngine_API RenderComponent : public Component {
public:
    RenderComponent(RenderLayer layer, bool isScreenSpace = false)
        : sdlTexture_(nullptr), width_(0), height_(0), layer_(layer), isScreenSpace_(isScreenSpace), hasAnimation_(false), srcRect_({0,0,0,0}), flipX_(false), flipY_(false) {}

    RenderComponent(SDL_Texture* sdlTexture, RenderLayer layer = RenderLayer::GAME_OBJECT, bool isScreenSpace = false, bool hasAnimation = false, int width = 0, int height = 0, SDL_Rect srcRect = {0,0,0,0}, bool flipX = false, bool flipY = false)
        : sdlTexture_(sdlTexture), layer_(layer), isScreenSpace_(isScreenSpace), hasAnimation_(hasAnimation), width_(width), height_(height), srcRect_(srcRect), flipX_(flipX), flipY_(flipY)
    {
        if (sdlTexture_ && (width_ == 0 || height_ == 0)) {
            float texW, texH;
            SDL_GetTextureSize(sdlTexture_, &texW, &texH);
            width_ = static_cast<int>(texW);
            height_ = static_cast<int>(texH);
        }

        if (sdlTexture_ && srcRect_.w == 0 && srcRect_.h == 0) {
            srcRect_ = {0, 0, width_, height_};
        }
    }
        
    ~RenderComponent() {
        // RenderComponent는 SDL_Texture*의 소유권을 가지지 않으므로 파괴하지 않음.
    }
    
    SDL_Texture* getSDLTexture() const { return sdlTexture_; }
    void setSDLTexture(SDL_Texture* sdlTexture, int width, int height) { 
        sdlTexture_ = sdlTexture; 
        width_ = width; 
        height_ = height; 

        if (sdlTexture_ && (width_ == 0 || height_ == 0)) {
            float texW, texH;
            SDL_GetTextureSize(sdlTexture_, &texW, &texH);
            width_ = static_cast<int>(texW);
            height_ = static_cast<int>(texH);
        }
        
        srcRect_ = {0, 0, width_, height_}; // Update srcRect_ when texture is set
    }

    int getWidth() const { return width_; }
    int getHeight() const { return height_; }

    RenderLayer getLayer() const { return layer_; }
    void setLayer(RenderLayer layer) { layer_ = layer; }

    bool isScreenSpace() const { return isScreenSpace_; }

    bool hasAnimation() const { return hasAnimation_; }
    void setHasAnimation(bool hasAnim) { hasAnim = hasAnim; }
    const SDL_Rect& getSrcRect() const { return srcRect_; }
    void setSrcRect(const SDL_Rect& rect) { srcRect_ = rect; }

    bool getFlipX() const { return flipX_; }
    void setFlipX(bool flip) { flipX_ = flip; }
    bool getFlipY() const { return flipY_; }
    void setFlipY(bool flip) { flipY_ = flip; }

private:
    SDL_Texture* sdlTexture_;
    RenderLayer layer_;
    int width_;
    int height_;
    SDL_Rect srcRect_;
    bool isScreenSpace_ = false;
    bool hasAnimation_ = false;
    bool flipX_ = false;
    bool flipY_ = false;
};



