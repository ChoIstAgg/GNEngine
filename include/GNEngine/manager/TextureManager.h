#pragma once
#include "../GNEngine_API.h"
#include "GNEngineRootPath.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <unordered_map>
#include <memory>
#include <filesystem>
#include <functional> // Required for std::hash<std::filesystem::path>

#include "GNEngine/core/Texture.h"

class GNEngine_API TextureManager {
private:
    SDL_Renderer* renderer_;

    /* 파일 경로 기반 텍스처 저장소 */
    std::unordered_map<std::filesystem::path, std::unique_ptr<Texture>> textureMap_;

    /* 엔진 내장 텍스처 저장소 */
    std::unique_ptr<Texture> imageErrorTexture_;

public:
    TextureManager(SDL_Renderer* renderer);
    ~TextureManager();

    /**
    * @brief 파일 경로에서 텍스처를 로드함.
    * @param filePath 텍스처 파일의 경로.
    * @return 성공 시 true, 실패 시 false.
    */
    bool loadTexture(const std::filesystem::path& filePath);

    /**
    * @brief 내장된 메모리에서 텍스처를 로드함.
    * @param name 텍스처를 식별할 고유 이름.
    * @param data 이미지 데이터의 시작을 가리키는 포인터.
    * @param size 데이터의 크기 (바이트 단위).
    * @return 성공 시 true, 실패 시 false.
    */
    bool loadTextureEmbedded(const std::string& name, const unsigned char* data, size_t size);

    /**
    * @brief 파일 경로로 텍스처를 가져옴. 맵에 없으면 로드를 시도함.
    * @param filePath 텍스처 파일의 경로.
    * @return Texture에 대한 포인터. 실패 시 nullptr.
    */
    Texture* getTexture(const std::filesystem::path& filePath);

    /**
    * @brief 내장 리소스 이름으로 텍스처를 가져옴.
    * @param name 텍스처의 고유 이름.
    * @return Texture에 대한 포인터. 맵에 없으면 nullptr.
    */
    Texture* getEmbeddedTexture(const std::string& name);

    void setScaleModeOfTexture(const std::string& name, SDL_ScaleMode scaleMode);
    void setScaleModeOfTexture(const std::filesystem::path& name, SDL_ScaleMode scaleMode);
};