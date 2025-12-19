#include "GNEngine/core/ComponentArray.h"
#include "GNEngine/manager/EntityManager.h"

/*
 * @brief 엔티티가 파괴될 때 호출되어 공유 맵과 각 컴포넌트 배열의 데이터를 정리함.
*/
GNEngine_API void SoAComponentArray::entityDestroyed(EntityId entity) {
    if (!entityToIndexMap.count(entity)) {
        return; // 이 엔티티는 SoA 컴포넌트를 가지고 있지 않음
    }

    // 1. 제거할 엔티티의 인덱스와 마지막 요소의 인덱스를 찾음
    size_t indexOfRemoved = entityToIndexMap.at(entity);
    size_t indexOfLast = entityToIndexMap.size() - 1;

    // 2. 데이터 벡터에서 실제 데이터 이동 및 삭제 (swap and pop)
    swapAndPop(indexOfRemoved, indexOfLast);

    // 3. 맵 업데이트
    if (indexOfRemoved == indexOfLast) {
        // 제거할 엔티티가 마지막 요소인 경우, 그냥 맵에서 제거
        entityToIndexMap.erase(entity);
        indexToEntityMap.erase(indexOfRemoved);
    } else {
        // 마지막 요소를 제거된 엔티티의 위치로 이동
        EntityId lastEntity = indexToEntityMap.at(indexOfLast);
        entityToIndexMap[lastEntity] = indexOfRemoved;
        indexToEntityMap[indexOfRemoved] = lastEntity;

        // 맵에서 원래 엔티티와 마지막 인덱스 항목 제거
        entityToIndexMap.erase(entity);
        indexToEntityMap.erase(indexOfLast);
    }
}




