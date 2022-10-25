#include "entity.hpp"

#include <stdexcept>
#include <string>

EntityManager::EntityManager() {
	for (EntityId e = 0; e < MAX_ENTITIES; e++)
		availableEntityIds.push(e);
}

std::shared_ptr<Entity> EntityManager::createEntity() {
	if (nEntities >= MAX_ENTITIES) throw std::length_error("Entity limit exceeded.");
	auto entity = std::make_shared<Entity>(*this, availableEntityIds.front());
	availableEntityIds.pop();
	nEntities++;
	return entity;
}

void EntityManager::destroyEntity(EntityId entity) {
	availableEntityIds.push(entity);
	nEntities--;
}