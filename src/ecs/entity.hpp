#pragma once

#include "scene.hpp"
#include "types.hpp"
#include <memory>
#include <queue>

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
	 	Scene &scene;
		std::queue<EntityId> availableEntityIds {};
		unsigned int nEntities = 0;

	public:
		EntityManager(Scene &scene) : scene(scene) {
			for (EntityId e = 0; e < MAX_ENTITIES; e++)
				availableEntityIds.push(e);
		};

		std::shared_ptr<Entity> createEntity();
		void destroyEntity(EntityId entityId);
};