#pragma once
#include "../GNEngine_API.h"

#include <string>
#include <unordered_map>
#include <memory>
#include <optional>
#include <AL/al.h>

#include "GNEngine/core/Component.h"
#include "GNEngine/core/Sound.h"

// std::filesystem::path로 경로로 저장하지 않고 std::string으로 이름으로 Sound를 저장하는 이유
// : 같은 경로의 같은 사운드라도 속성을 달리 할 수 있기 때문. 

/*
 * @class SoundComponent
 * @brief 엔티티의 사운드 재생을 관리하는 '사운드 보관함' 컴포넌트임.
 *        이름(string)으로 여러 사운드를 등록하고, 각 사운드의 상태와 속성을 관리함.
 *        재생/정지 등은 상태 플래그를 변경하는 방식으로 동작하며, 실제 OpenAL 제어는 SoundSystem이 담당함.
 */
class GNEngine_API SoundComponent : public Component {
public:
    /*
     * @struct SoundData
     * @brief 개별 사운드의 모든 데이터와 상태를 담는 구조체.
     */
    struct SoundData {
        std::shared_ptr<Sound> resource = nullptr;
        std::optional<ALuint> sourceId = std::nullopt; // 재생 중일 때 OpenAL 소스 ID

        // --- 재생 속성(기본값) ---
        float volume = 1.0f;
        float pitch = 1.0f;
        bool loop = false;
        SoundPriority priority = SoundPriority::NORMAL;
        bool spatialized = true; // 3D 사운드 여부

        // --- 상태 플래그 ---
        bool wantsToPlay = false;
        bool wantsToStop = false;
    };

    SoundComponent() = default;

    /*
     * @brief 보관함에 새로운 사운드를 추가함.
     * @param name 사운드를 식별할 고유한 이름 (e.g. "walk", "hit").
     * @param sound 리소스.
     * @param loop 반복 여부.
     * @param volume 볼륨.
     */
    void addSound(const std::string& name, std::shared_ptr<Sound> sound, bool loop = false, float volume = 1.0f);

    /*
     * @brief 지정된 이름의 사운드 재생을 요청함.
     *        실제 재생은 SoundSystem이 다음 업데이트에서 처리함.
     */
    void play(const std::string& name);

    /*
     * @brief 지정된 이름의 사운드 정지를 요청함.
     */
    void stop(const std::string& name);

    /*
     * @brief 지정된 이름의 사운드가 현재 재생 중인지 확인함.
     */
    bool isPlaying(const std::string& name) const;

    /*
     * @brief 지정된 이름의 사운드 데이터에 대한 참조를 반환함.
     *        외부(주로 SoundSystem)에서 상태를 직접 수정할 때 사용.
     */
    SoundData* getSoundData(const std::string& name);
    const SoundData* getSoundData(const std::string& name) const;

    /*
     * @brief 컴포넌트가 가진 모든 사운드 데이터 맵에 대한 참조를 반환함.
     */
    std::unordered_map<std::string, SoundData>& getAllSounds();

private:
    std::unordered_map<std::string, SoundData> sounds_;
};



