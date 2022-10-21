#include "entity.hpp"

#include <stdexcept>
#include <string>

EntityManager::EntityManager() {
	for (Entity e = 0; e < MAX_ENTITIES; e++) {
		availableEntities.push(e);
	}
}

Entity EntityManager::createEntity() {
	if (nEntities >= MAX_ENTITIES) throw std::length_error("Entity limit exceeded.");
	auto entity = availableEntities.front();
	availableEntities.pop();
	nEntities++;
	return entity;
}

void EntityManager::destroyEntity(Entity entity) {
	if (entity >= MAX_ENTITIES) throw std::out_of_range("Entity " + std::to_string(entity) + " out of range.");
	availableEntities.push(entity);
	nEntities--;
}