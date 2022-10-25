#include "scene.hpp"

#include "entity.hpp"
#include <type_traits>

std::shared_ptr<Entity> Scene::createEntity() {
	auto entity = entityManager->createEntity();
	activeEntities.insert({entity->getId(), entity});
	return entity;
}

void Scene::destroyEntity(EntityId entity) {
	entityManager->destroyEntity(entity);
	componentManager->onEntityDestroyed(entity);
	activeEntities.erase(entity);
}