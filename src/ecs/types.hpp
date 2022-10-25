#pragma once

#include <map>
#include <optional>

#define MAX_ENTITIES 1024

class Entity;

using EntityId = unsigned int;
using ActiveEntityMap = std::map<EntityId, std::shared_ptr<Entity>>;

template <typename T>
using OptionalComponent = std::optional<std::shared_ptr<T>>;
