#include "GNEngine/manager/TextureManager.h"

#include <iostream>
#include <filesystem>
#include <memory>

#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>

#include "GNEngine/resource/image/ImageError.h"

TextureManager::TextureManager(SDL_Renderer* renderer)
    : renderer_(renderer) {
    if (!renderer) {
        SDL_Log("TextureManager::init - rawRenderer is null: %s", SDL_GetError());
    }

    /* --- Load embedded images. --- */
    /* 이미지 오류시 대체되어 렌더링되는 이미지. */
    SDL_IOStream* io = SDL_IOFromConstMem(GNEngine::resource::embedded::imageErrorImage, GNEngine::resource::embedded::imageErrorImage_len);
    if (io == NULL) {
        SDL_Log("Failed to create IOStream from memory for __IMAGE_ERROR__: %s", SDL_GetError());
        return;
    }

    SDL_Texture* sdlTexture = IMG_LoadTexture_IO(renderer_, io, true);
    if (sdlTexture == nullptr) {
        SDL_Log("Failed to load embedded texture __IMAGE_ERROR__: %s", SDL_GetError());
        return;
    }
    
    float width_f, height_f;
    SDL_GetTextureSize(sdlTexture, &width_f, &height_f);
    imageErrorTexture_ = std::make_unique<Texture>(sdlTexture, static_cast<int>(width_f), static_cast<int>(height_f));
}

TextureManager::~TextureManager() {
    textureMap_.clear();
    std::cerr << "TextureManager " << this << " is successfully destroyed" << std::endl;
}

bool TextureManager::loadTexture(const std::filesystem::path& filePath) {
    if (textureMap_.count(filePath)) {
        return true;
    }

    if (!std::filesystem::exists(filePath)) {
        SDL_Log("TextureManager::loadTexture - File does not exist: %s", filePath.string().c_str());
        return false;
    }

    SDL_Surface* tmpSurface = IMG_Load(filePath.string().c_str());
    if (tmpSurface == nullptr) {
        SDL_Log("TextureManager::loadTexture - Failed to load surface %s: %s", filePath.string().c_str(), SDL_GetError());
        return false;
    }

    SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(renderer_, tmpSurface);
    SDL_DestroySurface(tmpSurface);

    if (sdlTexture == nullptr) {
        SDL_Log("TextureManager::loadTexture - Failed to create texture from surface %s: %s", filePath.string().c_str(), SDL_GetError());
        return false;
    }

    float width_f, height_f;
    SDL_GetTextureSize(sdlTexture, &width_f, &height_f);
    int width = static_cast<int>(width_f);
    int height = static_cast<int>(height_f);

    textureMap_[filePath] = std::make_unique<Texture>(sdlTexture, width, height);
    return true;
}

/* 
 * @brief GNEngine 내장 이미지를 로딩함.
*/
bool TextureManager::loadTextureEmbedded(const std::string& name, const unsigned char* data, size_t size) {
    if (textureMap_.count(name)) {
        return true;
    }

    SDL_IOStream* io = SDL_IOFromConstMem(data, size);
    if (io == NULL) {
        SDL_Log("Failed to create IOStream from memory for %s: %s", name.c_str(), SDL_GetError());
        return false;
    }

    SDL_Texture* sdlTexture = IMG_LoadTexture_IO(renderer_, io, true);
    if (sdlTexture == nullptr) {
        SDL_Log("Failed to load embedded texture %s: %s", name.c_str(), SDL_GetError());
        return false;
    }

    float width_f, height_f;
    SDL_GetTextureSize(sdlTexture, &width_f, &height_f);
    int width = static_cast<int>(width_f);
    int height = static_cast<int>(height_f);
 
    textureMap_[name] = std::make_unique<Texture>(sdlTexture, width, height);
    return true;
}

Texture* TextureManager::getTexture(const std::filesystem::path& filePath) {
    if (!textureMap_.count(filePath)) {
        if (!loadTexture(filePath)) {
            SDL_Log("TextureManager::getTexture - Texture not found and could not be loaded: %s. Returning default texture.", filePath.string().c_str());
            return imageErrorTexture_.get();
        }
    }
    return textureMap_.at(filePath).get();
}

Texture* TextureManager::getEmbeddedTexture(const std::string& name) {
    auto it = textureMap_.find(name);
    if (it != textureMap_.end()) {
        return it->second.get();
    }

    SDL_Log("TextureManager::getTexture - Embedded texture not found: %s. Returning default texture.", name.c_str());
    return imageErrorTexture_.get();
}

void TextureManager::setScaleModeOfTexture(const std::string& name, SDL_ScaleMode scaleMode) {
    if (!SDL_SetTextureScaleMode(getEmbeddedTexture(name)->sdlTexture_, scaleMode)) {
        SDL_Log("TextureManager::setScaleModeOfTexture - Failed to set texture scale mode for %s: %s", name.c_str(), SDL_GetError());
        // Even if setting scale mode fails, we still want to use the texture if it was created successfully.
        // This log helps diagnose, but doesn't prevent texture usage.
    }
}

void TextureManager::setScaleModeOfTexture(const std::filesystem::path& texturePath, SDL_ScaleMode scaleMode) {
    if (!SDL_SetTextureScaleMode(getTexture(texturePath)->sdlTexture_, scaleMode)) {
        SDL_Log("TextureManager::setScaleModeOfTexture - Failed to set texture scale mode for %s: %s", texturePath.c_str(), SDL_GetError());
        // Even if setting scale mode fails, we still want to use the texture if it was created successfully.
        // This log helps diagnose, but doesn't prevent texture usage.
    }
}