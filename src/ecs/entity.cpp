#include "entity.hpp"

#include <stdexcept>

std::shared_ptr<Entity> EntityManager::createEntity() {
	if (nEntities >= MAX_ENTITIES) throw std::length_error("Entity limit exceeded.");
	auto entity = std::make_shared<Entity>(scene, availableEntityIds.front());
	availableEntityIds.pop();
	nEntities++;
	return entity;
}

void EntityManager::destroyEntity(EntityId entity) {
	availableEntityIds.push(entity);
	nEntities--;
}