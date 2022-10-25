#pragma once

#include "component.hpp"
#include "types.hpp"
#include <memory>

class Entity;
class EntityManager;

class Scene {
	private:
		std::unique_ptr<EntityManager> entityManager = std::make_unique<EntityManager>();
		std::unique_ptr<ComponentManager> componentManager = std::make_unique<ComponentManager>();
		ActiveEntityMap activeEntities {};

	public:
		std::shared_ptr<Entity> createEntity();
		void destroyEntity(EntityId entity);

		template <typename T>
		void addComponent(EntityId entity, T component) {
			componentManager->addComponent(entity, std::make_shared<T>(component));
		};

		template <typename T>
		void addComponent(EntityId entity, std::shared_ptr<T> component) {
			componentManager->addComponent(entity, component);
		};

		template <typename T>
		void removeComponent(EntityId entity) {
			componentManager->removeComponent<T>(entity);
		};

		template <typename T>
		OptionalComponent<T> getComponent(EntityId entity) {
			return componentManager->getComponent<T>(entity);
		};

		ActiveEntityMap &getActiveEntities() { return activeEntities; };
};