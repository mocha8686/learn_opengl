#pragma once

#include "scene.hpp"
#include "types.hpp"
#include <memory>
#include <optional>
#include <queue>

class Scene;

class Entity {
	private:
		Scene &scene;
		EntityId id;

	public:
		Entity(Scene &scene, EntityId id) : scene(scene), id(id) {};
		EntityId getId() { return id; };
		void destroy() { scene.destroyEntity(id); };

		template <typename T>
		void addComponent(T component) {
			scene.addComponent(id, std::make_shared<T>(component));
		};

		template <typename T>
		void addComponent(std::shared_ptr<T> component) {
			scene.addComponent(id, component);
		};

		template <typename T>
		void removeComponent() {
			scene.removeComponent<T>(id);
		};

		template <typename T>
		OptionalComponent<T> getComponent() {
			return scene.getComponent<T>(id);
		};
};

class EntityManager {
	private:
		std::queue<EntityId> availableEntityIds {};
		unsigned int nEntities = 0;

	public:
		EntityManager();
		std::shared_ptr<Entity> createEntity();
		void destroyEntity(EntityId entityId);
};