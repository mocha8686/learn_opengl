#pragma once

#include "component.hpp"
#include "types.hpp"
#include <cstddef>
#include <iterator>
#include <map>
#include <memory>
#include <utility>

class Entity;
class EntityManager;

class ActiveEntities {
	private:
		using Value = std::shared_ptr<Entity>;
		using ActiveEntityMap = std::map<EntityId, std::shared_ptr<Entity>>;

	public:
		ActiveEntityMap map;

		struct Iterator {
			using value_type = Value;
			using reference = value_type&;
			using pointer = value_type*;
			using difference_type = std::ptrdiff_t;
			using iterator_category	= std::forward_iterator_tag;

			ActiveEntityMap::iterator iter;

			Iterator(ActiveEntityMap::iterator iter) : iter(iter) {};

			reference operator*() const { return iter->second; };
			reference operator->() const { return iter->second; };

			Iterator& operator++() { iter++; return *this; };
			Iterator operator++(int) { Iterator i(*this); ++(*this); return i; };

			bool operator==(const Iterator &rhs) const { return iter == rhs.iter; };
			bool operator!=(const Iterator &rhs) const { return iter != rhs.iter; };
		};

		Iterator begin() { return Iterator(map.begin()); };
		Iterator end() { return Iterator(map.end()); };
};

class Scene {
	private:
		std::unique_ptr<EntityManager> entityManager = std::make_unique<EntityManager>(*this);
		std::unique_ptr<ComponentManager> componentManager = std::make_unique<ComponentManager>();
		std::unique_ptr<ActiveEntities> activeEntities = std::make_unique<ActiveEntities>();

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

		ActiveEntities &getActiveEntities() { return *activeEntities; };
};