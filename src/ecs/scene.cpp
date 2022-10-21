#include "scene.hpp"

Entity Scene::createEntity() {
	auto entity = entityManager->createEntity();
	activeEntities.insert(entity);
	return entity;
}

void Scene::destroyEntity(Entity entity) {
	entityManager->destroyEntity(entity);
	componentManager->onEntityDestroyed(entity);
	activeEntities.erase(entity);
}