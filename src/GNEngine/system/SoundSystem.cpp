#include "GNEngine/system/SoundSystem.h"
#include "GNEngine/manager/EntityManager.h"
#include <iostream>

SoundSystem::SoundSystem(SoundManager& soundManager)
    : soundManager_(soundManager) {}

void SoundSystem::update(EntityManager& entityManager, float deltaTime) {
    auto soundComponentArray = entityManager.getComponentArray<SoundComponent>();
    if (!soundComponentArray) {
        return; // No SoundComponents to process
    }

    // 컴포넌트를 순회하며 재생/정지 요청 처리
    for (auto entity : entityManager.getEntitiesWith<SoundComponent, TransformComponent>()) {
        auto& soundComponent = soundComponentArray->getComponent(entity);
        auto transform = entityManager.getComponent<TransformComponent>(entity).value();

        for (auto& pair : soundComponent.getAllSounds()) {
            auto& soundName = pair.first;
            auto& soundData = pair.second;

            // 재생 요청 처리
            if (soundData.wantsToPlay) {
                soundData.wantsToPlay = false; // 요청 처리 플래그 리셋
                Position pos = { transform.positionX_, transform.positionY_, 0.0f };
                ALuint sourceId = soundManager_.playSound(entity, soundData.resource.get(), pos, soundData.priority, soundData.volume, soundData.pitch, soundData.loop, soundData.spatialized);
                if (sourceId != 0) {
                    soundData.sourceId = sourceId;
                }
            }
            // 정지 요청 처리
            else if (soundData.wantsToStop) {
                soundData.wantsToStop = false; // 요청 처리 플래그 리셋
                if (soundData.sourceId.has_value()) {
                    soundManager_.stopSound(soundData.sourceId.value());
                    soundData.sourceId.reset();
                }
            }
        }
    }

    // 2. SoundManager의 Voice Pool을 순회하며 상태 동기화
    const auto& sourceIds = soundManager_.getSourceIds();
    const auto& ownerEntityIds = soundManager_.getOwnerEntityIds();
    const auto& arePlaying = soundManager_.getArePlayingFlags();

    for (size_t i = 0; i < soundManager_.getVoiceCount(); ++i) {
        if (arePlaying[i]) {
            EntityId ownerId = ownerEntityIds[i];
            if (ownerId == 0) continue; // 주인이 없는 소스는 스킵

            // 위치 동기화
            if (auto transformOpt = entityManager.getComponent<TransformComponent>(ownerId)) {
                auto& transform = transformOpt.value();
                soundManager_.setSourcePosition(sourceIds[i], transform.positionX_, transform.positionY_, 0.0f);
            }

            // 재생 완료된 소스 정리
            ALint state;
            alGetSourcei(sourceIds[i], AL_SOURCE_STATE, &state);
            if (state == AL_STOPPED) {
                if (soundComponentArray->hasComponent(ownerId)) {
                    auto& soundComp = soundComponentArray->getComponent(ownerId);
                    // 컴포넌트에서 해당 sourceId를 가진 soundData를 찾아 초기화
                    for (auto& pair : soundComp.getAllSounds()) {
                        if (pair.second.sourceId.has_value() && pair.second.sourceId.value() == sourceIds[i]) {
                            pair.second.sourceId.reset();
                            break; // 찾았으므로 루프 종료
                        }
                    }
                }
                // SoundManager에게 보이스 반납을 요청 (내부적으로 arePlaying 플래그 등을 false로 만듦)
                soundManager_.stopSound(sourceIds[i]); 
            }
        }
    }
}


