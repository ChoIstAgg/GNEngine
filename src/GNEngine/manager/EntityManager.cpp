#include "GNEngine/manager/EntityManager.h"
#include <algorithm>
#include <stdexcept>

EntityId EntityManager::createEntity() {
    EntityId newId;
    if (!availableEntityIds_.empty()) {
        newId = availableEntityIds_.front();
        availableEntityIds_.pop();
    } else {
        newId = nextEntityId_++;
    }
    activeEntities_.push_back(newId);
    entitySignatures_[newId] = Signature(); // 새로운 엔티티의 시그니처 초기화
    return newId;
}

void EntityManager::destroyEntity(EntityId entity) {
    // 모든 컴포넌트 배열에서 해당 엔티티의 컴포넌트 제거
    for (auto const& pair : componentArrays_) {
        pair.second->entityDestroyed(entity);
    }

    // activeEntities_에서 엔티티 제거
    activeEntities_.erase(std::remove(activeEntities_.begin(), activeEntities_.end(), entity), activeEntities_.end());

    // 엔티티 시그니처 제거
    entitySignatures_.erase(entity);

    // ID를 재활용 풀에 추가
    availableEntityIds_.push(entity);
}

std::vector<EntityId> EntityManager::getAllEntities() const {
    return activeEntities_;
}



