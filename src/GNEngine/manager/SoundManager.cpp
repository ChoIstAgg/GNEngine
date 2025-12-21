#include "GNEngine/manager/SoundManager.h"
#include "GNEngine/core/Sound.h"
#include <iostream>
#include <algorithm>
#include <filesystem>

#include <AL/alc.h>
#include <AL/alext.h> 

/* for decording sound files*/
#define DR_MP3_IMPLEMENTATION
#include "./dr_mp3.h"
#define DR_WAV_IMPLEMENTATION
#include "./dr_wav.h"
#define DR_FLAC_IMPLEMENTATION
#include "./dr_flac.h"

// ogg decoding
#include "./stb_vorbis.c"

/**
 * SoundManager 생성시 OpenAL 시작함. 
 */
SoundManager::SoundManager() {
    initAL();
}

/**
 * SoundManager 파괴시 OpenAL 종료
 */
SoundManager::~SoundManager() {
    quitAL();
    std::cerr << "SoundManager " << this << " is successfully destroyed. \n";
}

bool SoundManager::initAL() {
    device_ = alcOpenDevice(NULL);
    if (!device_) {
        std::cerr << "Failed to open OpenAL device." << std::endl;
        return false;
    }
    
    context_ = alcCreateContext(device_, NULL);
    if (!context_ || !alcMakeContextCurrent(context_)) {
        std::cerr << "Failed to create or set OpenAL context." << std::endl;
        alcCloseDevice(device_);
        return false;
    }
    
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
    setListenerPosition(0.0f, 0.0f, 0.0f);
    setListenerOrientation(0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

    // SoA 벡터들 초기화
    sourceIdsLeft_.resize(MAX_VOICES);
    sourceIdsRight_.resize(MAX_VOICES);
    sounds_.assign(MAX_VOICES, nullptr);
    priorities_.assign(MAX_VOICES, SoundPriority::LOW);
    arePlaying_.assign(MAX_VOICES, false);
    areSplitStereo_.assign(MAX_VOICES, false);
    ownerEntityIds_.assign(MAX_VOICES, 0);

    alGenSources(MAX_VOICES, sourceIdsLeft_.data());
    alGenSources(MAX_VOICES, sourceIdsRight_.data());
    if (alGetError() != AL_NO_ERROR) {
        std::cerr << "Failed to generate OpenAL sources." << std::endl;
        return false;
    }

    for(size_t i = 0; i < MAX_VOICES; ++i) {
        sourceIdToIndexMap_[sourceIdsLeft_[i]] = i;
        alSourcef(sourceIdsLeft_[i], AL_ROLLOFF_FACTOR, 1.0f);
        alSourcef(sourceIdsRight_[i], AL_ROLLOFF_FACTOR, 1.0f);
    }

    std::cout << "OpenAL initialized with " << MAX_VOICES << " voices." << std::endl;
    return true;
}

void SoundManager::quitAL() {
    if (context_) {
        stopAllSounds();
        alDeleteSources(MAX_VOICES, sourceIdsLeft_.data());
        alDeleteSources(MAX_VOICES, sourceIdsRight_.data());
        sourceIdsLeft_.clear();
        sourceIdsRight_.clear();
        sourceIdToIndexMap_.clear();
        soundCache_.clear();
        
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context_);
        context_ = nullptr;
    }
    if (device_) {
        alcCloseDevice(device_);
        device_ = nullptr;
    }
    std::cout << "OpenAL shut down." << std::endl;
}

void SoundManager::checkAlErrors(const std::string& filename, int line) {
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        std::cerr << "[OpenAL Error] in " << filename << " (" << line << "): " << alGetString(error) << std::endl;
    }
}

/*
 * If the sound is already loaded, return it. 
 * But sound not loaded yet, Load and return it.
*/
std::shared_ptr<Sound> SoundManager::getSound(const std::filesystem::path& filePath) {
    if (auto it = soundCache_.find(filePath); it != soundCache_.end()) {
        return it->second;
    }

    ALuint monoBuffer = 0, stereoBufferRight = 0;
    bool isStereo = false, loaded = false;
    std::string ext = filePath.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == ".wav") { loaded = loadWav(filePath, monoBuffer, stereoBufferRight, isStereo); }
    else if (ext == ".mp3") { loaded = loadMp3(filePath, monoBuffer, stereoBufferRight, isStereo); }
    else if (ext == ".ogg") { loaded = loadOgg(filePath, monoBuffer, stereoBufferRight, isStereo); }
    else if (ext == ".flac") { loaded = loadFlac(filePath, monoBuffer, stereoBufferRight, isStereo); }
    else { std::cerr << "Unsupported format: " << ext << std::endl; 
        return nullptr; 
    }

    if (loaded) {
        auto sound = std::make_shared<Sound>(monoBuffer, stereoBufferRight, isStereo);
        soundCache_[filePath] = sound;
        return sound; /* Success */
    }
    /* failure */
    if (monoBuffer != 0) alDeleteBuffers(1, &monoBuffer);
    if (stereoBufferRight != 0) alDeleteBuffers(1, &stereoBufferRight);
    return nullptr;
}

ALuint SoundManager::playSound(EntityId entityId, Sound* sound, Position position, SoundPriority priority, float volume, float pitch, bool loop, bool spatialized) {
    if (!sound) return 0;

    auto voiceIndexOpt = findAvailableVoice(priority);
    if (!voiceIndexOpt) return 0;
    size_t i = *voiceIndexOpt;

    bool is3DStereo = sound->isStereo() && spatialized;

    // 보이스 상태 설정
    sounds_[i] = sound;
    priorities_[i] = priority;
    arePlaying_[i] = true;
    areSplitStereo_[i] = is3DStereo;
    ownerEntityIds_[i] = entityId;

    ALuint sourceLeft = sourceIdsLeft_[i];
    ALuint sourceRight = sourceIdsRight_[i];

    alSourcef(sourceLeft, AL_GAIN, volume);
    alSourcef(sourceLeft, AL_PITCH, pitch);
    alSourcei(sourceLeft, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    alSourcei(sourceLeft, AL_SOURCE_RELATIVE, !spatialized);

    if (is3DStereo) {
        alSourcei(sourceLeft, AL_BUFFER, sound->getMonoBuffer());
        alSourcei(sourceRight, AL_BUFFER, sound->getStereoBufferRight());
        alSourcef(sourceRight, AL_GAIN, volume);
        alSourcef(sourceRight, AL_PITCH, pitch);
        alSourcei(sourceRight, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
        alSourcei(sourceRight, AL_SOURCE_RELATIVE, !spatialized);
        setSourcePosition(sourceLeft, position.x, position.y, position.z);
        alSourcePlay(sourceRight);
    } else {
        alSourcei(sourceLeft, AL_BUFFER, sound->getMonoBuffer());
        if (spatialized) {
            alSource3f(sourceLeft, AL_POSITION, position.x, position.y, position.z);
        } else {
            alSource3f(sourceLeft, AL_POSITION, 0.0f, 0.0f, 0.0f);
        }
    }
    alSourcePlay(sourceLeft);
    AL_CHECK_ERROR();
    return sourceLeft;
}

void SoundManager::stopSound(ALuint sourceId) {
    if (auto it = sourceIdToIndexMap_.find(sourceId); it != sourceIdToIndexMap_.end()) {
        releaseVoice(it->second);
    }
}

void SoundManager::pauseSound(ALuint sourceId) { alSourcePause(sourceId); }
void SoundManager::resumeSound(ALuint sourceId) { alSourcePlay(sourceId); }

void SoundManager::togglePauseSound(ALuint sourceId) {
    ALint state; alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
    if (state == AL_PLAYING) alSourcePause(sourceId);
    else if (state == AL_PAUSED) alSourcePlay(sourceId);
}

void SoundManager::stopAllSounds() {
    for (size_t i = 0; i < MAX_VOICES; ++i) {
        if (arePlaying_[i]) {
            releaseVoice(i);
        }
    }
}

std::optional<size_t> SoundManager::findAvailableVoice(SoundPriority priority) {
    // 1. 사용 중이 아닌 보이스 검색
    for (size_t i = 0; i < MAX_VOICES; ++i) {
        if (!arePlaying_[i]) {
            ALint state; alGetSourcei(sourceIdsLeft_[i], AL_SOURCE_STATE, &state);
            if (state != AL_PLAYING && state != AL_PAUSED) {
                return i;
            }
        }
    }
    // 2. 훔칠 보이스 검색
    size_t lowestPriorityIndex = -1;
    SoundPriority lowestPriority = SoundPriority::CRITICAL;
    for (size_t i = 0; i < MAX_VOICES; ++i) {
        if (priorities_[i] <= priority && priorities_[i] <= lowestPriority) {
            lowestPriority = priorities_[i];
            lowestPriorityIndex = i;
        }
    }
    if (lowestPriorityIndex != -1) {
        releaseVoice(lowestPriorityIndex);
        return lowestPriorityIndex;
    }
    return std::nullopt;
}

void SoundManager::releaseVoice(size_t i) {
    alSourceStop(sourceIdsLeft_[i]);
    alSourcei(sourceIdsLeft_[i], AL_BUFFER, 0);
    if (areSplitStereo_[i]) {
        alSourceStop(sourceIdsRight_[i]);
        alSourcei(sourceIdsRight_[i], AL_BUFFER, 0);
    }
    arePlaying_[i] = false;
    sounds_[i] = nullptr;
    priorities_[i] = SoundPriority::LOW;
    ownerEntityIds_[i] = 0;
    areSplitStereo_[i] = false;
}

void SoundManager::setListenerPosition(float x, float y, float z) {
    listenerPosition_ = {x, y, z};
    alListener3f(AL_POSITION, x, y, z);
}

void SoundManager::setListenerOrientation(float atX, float atY, float atZ, float upX, float upY, float upZ) {
    ALfloat orientation[] = {atX, atY, atZ, upX, upY, upZ};
    alListenerfv(AL_ORIENTATION, orientation);
}

void SoundManager::setSourcePosition(ALuint sourceId, float x, float y, float z) {
    if (auto it = sourceIdToIndexMap_.find(sourceId); it != sourceIdToIndexMap_.end()) {
        size_t i = it->second;
        if (areSplitStereo_[i]) {
            const float stereo_separation = 0.5f;
            alSource3f(sourceIdsLeft_[i], AL_POSITION, x - stereo_separation, y, z);
            alSource3f(sourceIdsRight_[i], AL_POSITION, x + stereo_separation, y, z);
        } else {
            alSource3f(sourceIdsLeft_[i], AL_POSITION, x, y, z);
        }
    }
}

void SoundManager::setSourceVolume(ALuint sourceId, float volume) {
    if (auto it = sourceIdToIndexMap_.find(sourceId); it != sourceIdToIndexMap_.end()) {
        size_t i = it->second;
        alSourcef(sourceIdsLeft_[i], AL_GAIN, volume);
        if (areSplitStereo_[i]) alSourcef(sourceIdsRight_[i], AL_GAIN, volume);
    }
}

void SoundManager::setSourcePitch(ALuint sourceId, float pitch) {
    if (auto it = sourceIdToIndexMap_.find(sourceId); it != sourceIdToIndexMap_.end()) {
        size_t i = it->second;
        alSourcef(sourceIdsLeft_[i], AL_PITCH, pitch);
        if (areSplitStereo_[i]) alSourcef(sourceIdsRight_[i], AL_PITCH, pitch);
    }
}

// --- 파일 로더 함수들은 변경 없음 (생략) ---

bool SoundManager::loadWav(const std::filesystem::path& filePath, ALuint& monoBuffer, ALuint& stereoBufferRight, bool& isStereo) {
    unsigned int channels;
    unsigned int sampleRate;
    drwav_uint64 totalPcmFrameCount;
    short* pData = drwav_open_file_and_read_pcm_frames_s16(filePath.string().c_str(), &channels, &sampleRate, &totalPcmFrameCount, NULL);
    if (pData == NULL) {
        std::cerr << "Failed to load WAV file: " << filePath.string() << std::endl;
        return false;
    }

    if (channels == 1) { // 모노
        isStereo = false;
        alGenBuffers(1, &monoBuffer);
        alBufferData(monoBuffer, AL_FORMAT_MONO16, pData, totalPcmFrameCount * sizeof(short), sampleRate);
    } else if (channels == 2) { // 스테레오
        isStereo = true;
        std::vector<short> pcmLeft(totalPcmFrameCount);
        std::vector<short> pcmRight(totalPcmFrameCount);
        for (drwav_uint64 i = 0; i < totalPcmFrameCount; ++i) {
            pcmLeft[i] = pData[i * 2];
            pcmRight[i] = pData[i * 2 + 1];
        }
        alGenBuffers(1, &monoBuffer);
        alBufferData(monoBuffer, AL_FORMAT_MONO16, pcmLeft.data(), pcmLeft.size() * sizeof(short), sampleRate);
        alGenBuffers(1, &stereoBufferRight);
        alBufferData(stereoBufferRight, AL_FORMAT_MONO16, pcmRight.data(), pcmRight.size() * sizeof(short), sampleRate);
    } else {
        std::cerr << "Unsupported WAV channel count: " << channels << "\n";
        drwav_free(pData, NULL);
        return false;
    }
    drwav_free(pData, NULL);
    AL_CHECK_ERROR();
    return true;
}

bool SoundManager::loadMp3(const std::filesystem::path& filePath, ALuint& monoBuffer, ALuint& stereoBufferRight, bool& isStereo) {
    drmp3_config config;
    drmp3_uint64 totalPcmFrameCount;
    float* pPcmData = drmp3_open_file_and_read_pcm_frames_f32(filePath.string().c_str(), &config, &totalPcmFrameCount, NULL);
    if (pPcmData == NULL) { return false; }

    std::vector<short> pcm16(totalPcmFrameCount * config.channels);
    for (drmp3_uint64 i = 0; i < totalPcmFrameCount * config.channels; ++i) {
        pcm16[i] = static_cast<short>(pPcmData[i] * 32767.0f);
    }
    drmp3_free(pPcmData, NULL);

    if (config.channels == 1) {
        isStereo = false;
        alGenBuffers(1, &monoBuffer);
        alBufferData(monoBuffer, AL_FORMAT_MONO16, pcm16.data(), pcm16.size() * sizeof(short), config.sampleRate);
    } else if (config.channels == 2) {
        isStereo = true;
        std::vector<short> pcmLeft(totalPcmFrameCount);
        std::vector<short> pcmRight(totalPcmFrameCount);
        for (drmp3_uint64 i = 0; i < totalPcmFrameCount; ++i) {
            pcmLeft[i] = pcm16[i * 2];
            pcmRight[i] = pcm16[i * 2 + 1];
        }
        alGenBuffers(1, &monoBuffer);
        alBufferData(monoBuffer, AL_FORMAT_MONO16, pcmLeft.data(), pcmLeft.size() * sizeof(short), config.sampleRate);
        alGenBuffers(1, &stereoBufferRight);
        alBufferData(stereoBufferRight, AL_FORMAT_MONO16, pcmRight.data(), pcmRight.size() * sizeof(short), config.sampleRate);
    } else { return false; }
    AL_CHECK_ERROR();
    return true;
}

bool SoundManager::loadFlac(const std::filesystem::path& filePath, ALuint& monoBuffer, ALuint& stereoBufferRight, bool& isStereo) {
    unsigned int channels, sampleRate;
    drflac_uint64 totalPcmFrameCount;
    short* pData = drflac_open_file_and_read_pcm_frames_s16(filePath.string().c_str(), &channels, &sampleRate, &totalPcmFrameCount, NULL);
    if (pData == NULL) return false;

    if (channels == 1) {
        isStereo = false;
        alGenBuffers(1, &monoBuffer);
        alBufferData(monoBuffer, AL_FORMAT_MONO16, pData, totalPcmFrameCount * sizeof(short), sampleRate);
    } else if (channels == 2) {
        isStereo = true;
        std::vector<short> pcmLeft(totalPcmFrameCount), pcmRight(totalPcmFrameCount);
        for (drflac_uint64 i = 0; i < totalPcmFrameCount; ++i) { pcmLeft[i] = pData[i * 2]; pcmRight[i] = pData[i * 2 + 1]; }
        alGenBuffers(1, &monoBuffer);
        alBufferData(monoBuffer, AL_FORMAT_MONO16, pcmLeft.data(), pcmLeft.size() * sizeof(short), sampleRate);
        alGenBuffers(1, &stereoBufferRight);
        alBufferData(stereoBufferRight, AL_FORMAT_MONO16, pcmRight.data(), pcmRight.size() * sizeof(short), sampleRate);
    } else { drflac_free(pData, NULL); return false; }
    drflac_free(pData, NULL);
    AL_CHECK_ERROR();
    return true;
}

bool SoundManager::loadOgg(const std::filesystem::path& filePath, ALuint& monoBuffer, ALuint& stereoBufferRight, bool& isStereo) {
    int channels, sample_rate, samples;
    short* pcm;
    samples = stb_vorbis_decode_filename(filePath.string().c_str(), &channels, &sample_rate, &pcm);
    if (samples == -1) return false;

    if (channels == 1) {
        isStereo = false;
        alGenBuffers(1, &monoBuffer);
        alBufferData(monoBuffer, AL_FORMAT_MONO16, pcm, samples * sizeof(short), sample_rate);
    } else if (channels == 2) {
        isStereo = true;
        std::vector<short> pcmLeft(samples), pcmRight(samples);
        for (int i = 0; i < samples; ++i) { pcmLeft[i] = pcm[i * 2]; pcmRight[i] = pcm[i * 2 + 1]; }
        alGenBuffers(1, &monoBuffer);
        alBufferData(monoBuffer, AL_FORMAT_MONO16, pcmLeft.data(), pcmLeft.size() * sizeof(short), sample_rate);
        alGenBuffers(1, &stereoBufferRight);
        alBufferData(stereoBufferRight, AL_FORMAT_MONO16, pcmRight.data(), pcmRight.size() * sizeof(short), sample_rate);
    } else { free(pcm); return false; }
    free(pcm);
    AL_CHECK_ERROR();
    return true;
}




