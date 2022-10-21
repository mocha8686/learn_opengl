#pragma once

#include "component.hpp"
#include "entity.hpp"
#include <memory>
#include <set>

class Scene {
	private:
		std::unique_ptr<EntityManager> entityManager = std::make_unique<EntityManager>();
		std::unique_ptr<ComponentManager> componentManager = std::make_unique<ComponentManager>();
		std::set<Entity> activeEntities {};

	public:
		Entity createEntity();
		void destroyEntity(Entity entity);

		template <typename T>
		void addComponent(Entity entity, T component) {
			componentManager->addComponent(entity, std::make_shared<T>(component));
		};

		template <typename T>
		void addComponent(Entity entity, std::shared_ptr<T> component) {
			componentManager->addComponent(entity, component);
		};

		template <typename T>
		void removeComponent(Entity entity) {
			componentManager->removeComponent<T>(entity);
		};

		template <typename T>
		OptionalComponent<T> getComponent(Entity entity) {
			return componentManager->getComponent<T>(entity);
		};

		std::set<Entity> &getActiveEntities() { return activeEntities; };
};