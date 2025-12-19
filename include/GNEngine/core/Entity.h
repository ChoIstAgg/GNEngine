#pragma once

#include <cstdint>
#include <bitset>

// Unique Entity ID that recognized by ECS system.
using EntityId = uint32_t;

// Invalid EntityId 
constexpr EntityId INVALID_ENTITY_ID = 0;

// Max component that entity can have
constexpr size_t MAX_COMPONENTS = 32;

// Bitset indicating which types of components an Entity has
using Signature = std::bitset<MAX_COMPONENTS>;

