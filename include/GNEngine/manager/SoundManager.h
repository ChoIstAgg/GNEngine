#pragma once
#include "../GNEngine_API.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>

#include "GNEngine/core/Entity.h"
#include "GNEngine/core/Sound.h"

#define AL_CHECK_ERROR() checkAlErrors(__FILE__, __LINE__)

class GNEngine_API SoundManager {
public:
    SoundManager();
    ~SoundManager();

    bool initAL();
    void quitAL();

    std::shared_ptr<Sound> getSound(const std::filesystem::path& filePath);

    ALuint playSound(EntityId entityId, Sound* sound,
                     Position position = {0.0f, 0.0f, 0.0f},
                     SoundPriority priority = SoundPriority::NORMAL,
                     float volume = 1.0f,
                     float pitch = 1.0f,
                     bool loop = false,
                     bool spatialized = false);

    void stopSound(ALuint sourceId);
    void pauseSound(ALuint sourceId);
    void resumeSound(ALuint sourceId);
    void togglePauseSound(ALuint sourceId);
    void stopAllSounds();

    void setListenerPosition(float x, float y, float z);
    void setListenerOrientation(float atX, float atY, float atZ, float upX, float upY, float upZ);

    void setSourcePosition(ALuint sourceId, float x, float y, float z);
    void setSourceVolume(ALuint sourceId, float volume);
    void setSourcePitch(ALuint sourceId, float pitch);

    // --- SoundSystem이 SoA 데이터에 접근하기 위한 Getter 함수들 ---
    const std::vector<ALuint>& getSourceIds() const { return sourceIdsLeft_; }
    const std::vector<EntityId>& getOwnerEntityIds() const { return ownerEntityIds_; }
    const std::vector<bool>& getArePlayingFlags() const { return arePlaying_; }
    size_t getVoiceCount() const { return sourceIdsLeft_.size(); }
    
    /* 최대 보이스 풀 개수*/
    static constexpr int MAX_VOICES = 64;
private:
    ALCdevice* device_ = nullptr;
    ALCcontext* context_ = nullptr;

    Position listenerPosition_ = {0.0f, 0.0f, 0.0f};

    std::unordered_map<std::filesystem::path, std::shared_ptr<Sound>> soundCache_;
    
    // --- SoA Voice Pool ---
    std::vector<ALuint> sourceIdsLeft_;
    std::vector<ALuint> sourceIdsRight_;
    std::vector<Sound*> sounds_;
    std::vector<SoundPriority> priorities_;
    std::vector<bool> arePlaying_;
    std::vector<bool> areSplitStereo_;
    std::vector<EntityId> ownerEntityIds_;

    // 빠른 조회를 위한 소스 ID -> 인덱스 맵
    std::unordered_map<ALuint, size_t> sourceIdToIndexMap_;

    std::optional<size_t> findAvailableVoice(SoundPriority priority);
    void releaseVoice(size_t voiceIndex);

    void checkAlErrors(const std::string& filename, int line);

    // Load files. (Decode sound file -> PCM Data)
    bool loadWav(const std::filesystem::path& filePath, ALuint& monoBuffer, ALuint& stereoBufferRight, bool& isStereo);
    bool loadMp3(const std::filesystem::path& filePath, ALuint& monoBuffer, ALuint& stereoBufferRight, bool& isStereo);
    bool loadFlac(const std::filesystem::path& filePath, ALuint& monoBuffer, ALuint& stereoBufferRight, bool& isStereo);
    bool loadOgg(const std::filesystem::path& filePath, ALuint& monoBuffer, ALuint& stereoBufferRight, bool& isStereo);
};



