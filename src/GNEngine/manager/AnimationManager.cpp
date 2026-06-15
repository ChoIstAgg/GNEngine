#include "GNEngine/manager/AnimationManager.h"
#include "GNEngineRootPath.h"

#include <fstream>
#include <iostream>

#include "./json.hpp"

#include "GNEngine/manager/TextureManager.h"

/*
 * @brief JSON 파일을 로드하여 애니메이션 데이터를 파싱하고 캐시에 저장함.
 * @param jsonPath - 애니메이션 데이터가 정의된 JSON 파일의 경로.
 * @return 로딩 및 파싱 성공 시 true, 실패 시 false.
 */
bool AnimationManager::loadAnimation(const std::filesystem::path& jsonPath) {
    std::ifstream file(jsonPath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open animation JSON file: " << jsonPath << std::endl;
        return false;
    }

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(file);
    } catch (const nlohmann::json::parse_error& error) {
        std::cerr << "Error parsing animation JSON file " << jsonPath << ": " << error.what() << std::endl;
        return false;
    }

    // JSON 구조에 따라 애니메이션 데이터를 파싱
    // 예시: {"animations": {"run": {...}, "idle": {...}}}
    if (!json.contains("animations") || !json["animations"].is_object()) {
        std::cerr << "Error: JSON file does not contain 'animations' object: " << jsonPath << std::endl;
        return false;
    }

    for (auto& [animName, animData] : json["animations"].items()) {
        if (animationCache_.count(animName)) {
            std::cerr << "Warning: Animation '" << animName << "' already loaded. Skipping." << std::endl;
            continue;
        }

        std::filesystem::path spritesheetPath;
        bool loop = true;

        if (animData.contains("spritesheetPath") && animData["spritesheetPath"].is_string()) {
            spritesheetPath = jsonPath.parent_path() / animData["spritesheetPath"].get<std::string>();
        } else {
            std::cerr << "Error: Animation '" << animName << "' in " << jsonPath << " missing 'spritesheetPath'." << std::endl;
            continue;
        }

        if (animData.contains("loop") && animData["loop"].is_boolean()) {
            loop = animData["loop"].get<bool>();
        }

        auto animation = std::make_shared<Animation>(spritesheetPath, loop);

        if (animData.contains("frames") && animData["frames"].is_array()) {
            for (const auto& frameData : animData["frames"]) {
                if (frameData.contains("x") && frameData["x"].is_number()
                    && frameData.contains("y") && frameData["y"].is_number()
                    && frameData.contains("w") && frameData["w"].is_number()
                    && frameData.contains("h") && frameData["h"].is_number()
                    && frameData.contains("duration") && frameData["duration"].is_number()) {

                    SDL_Rect rect = {
                        frameData["x"].get<int>(),
                        frameData["y"].get<int>(),
                        frameData["w"].get<int>(),
                        frameData["h"].get<int>()
                    };
                    int duration = frameData["duration"].get<int>();
                    animation->addFrame(rect, duration);
                } else {
                    std::cerr << "Error: Malformed frame data for animation '" << animName << "' in " << jsonPath << "." << std::endl;
                }
            }
        } else {
            std::cerr << "Error: Animation '" << animName << "' in " << jsonPath << " missing 'frames' array." << std::endl;
            continue;
        }

        if (animation->getFrameCount() > 0) {
            animationCache_[animName] = animation;
        } else {
            std::cerr << "Warning: Animation '" << animName << "' in " << jsonPath << " has no frames. Not caching." << std::endl;
        }
    }

    return true;
}

/*
 * @brief 캐시에서 애니메이션 데이터를 가져옴.
 * @param animationName - 가져올 애니메이션의 이름 (JSON 파일에 정의된 키).
 * @return 해당 이름의 std::shared_ptr<Animation>. 찾지 못하면 nullptr를 반환.
 */
std::shared_ptr<Animation> AnimationManager::getAnimation(const std::string& animationName) {
    auto it = animationCache_.find(animationName);
    if (it != animationCache_.end()) {
        return it->second;
    }
    std::cerr << "Warning: Animation '" << animationName << "' not found in cache." << std::endl;
    return nullptr;
}

void AnimationManager::setScaleModeOfAnimation(const std::string& animationName, SDL_ScaleMode scaleMode) {
    if(!SDL_SetTextureScaleMode(textureManager_.getTexture(getAnimation(animationName)->getTexturePath())->sdlTexture_, scaleMode)) {
        SDL_Log("AnimationManager::setScaleModeOfAnimation - Failed to set texture scale mode. (%s) Reason : %s", animationName.c_str(), SDL_GetError());
    }
}