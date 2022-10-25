#pragma once

#include "types.hpp"
#include <array>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <utility>

class IComponentArray {
	public:
		virtual ~IComponentArray() = default;
		virtual void onEntityDestroyed(EntityId entity) = 0;
};

template <typename T>
class ComponentArray : public IComponentArray {
	private:
		std::array<std::shared_ptr<T>, MAX_ENTITIES> componentArray {};
		std::unordered_map<EntityId, size_t> entityToIndexMap {};
		std::unordered_map<size_t, EntityId> indexToEntityMap {};
		size_t nComponents = 0;

	public:
		void insertData(EntityId entity, std::shared_ptr<T> component) {
			if (entityToIndexMap.count(entity) == 1)
				throw std::runtime_error("Entity " + std::to_string(entity) + " already has a(n) `" + typeid(T).name() + "` component.");

			componentArray[nComponents] = component;
			entityToIndexMap[entity] = nComponents;
			indexToEntityMap[nComponents] = entity;
			nComponents++;
		};

		void removeData(EntityId entity) {
			if (entityToIndexMap.count(entity) != 1)
				throw std::runtime_error("Entity " + std::to_string(entity) + " does not have a(n) `" + typeid(T).name() + "` component.");

			auto index = entityToIndexMap[entity];
			nComponents--;

			componentArray[index] = componentArray[nComponents];
			auto lastEntity = indexToEntityMap[nComponents];
			entityToIndexMap[lastEntity] = index;
			indexToEntityMap[index] = lastEntity;

			entityToIndexMap.erase(entity);
			indexToEntityMap.erase(nComponents);
		};

		OptionalComponent<T> getData(EntityId entity) {
			auto it = entityToIndexMap.find(entity);
			if (it == entityToIndexMap.end()) return std::nullopt;
			return std::optional<std::shared_ptr<T>>(componentArray[it->second]);
		};

		void onEntityDestroyed(EntityId entity) override {
			removeData(entity);
		};
};

class ComponentManager {
	private:
		std::unordered_map<const char *, std::shared_ptr<IComponentArray>> componentArrays {};

		template <typename T>
		std::shared_ptr<ComponentArray<T>> getComponentArray() {
			auto name = typeid(T).name();
			auto it = componentArrays.find(name);
			if (it == componentArrays.end()) {
				auto strName = std::string(name);
				throw std::runtime_error("Component `" + strName + "` has not been registered.");
			}
			return std::static_pointer_cast<ComponentArray<T>>(it->second);
		};

	public:
		template <typename T>
		void addComponent(EntityId entity, std::shared_ptr<T> component) {
			std::shared_ptr<ComponentArray<T>> componentArray;
			try {
				componentArray = getComponentArray<T>();
			} catch (const std::runtime_error&) {
				auto name = typeid(T).name();
				componentArrays[name] = std::make_shared<ComponentArray<T>>();
				componentArray = getComponentArray<T>();
			}

			componentArray->insertData(entity, component);
		};

		template <typename T>
		void removeComponent(EntityId entity) {
			getComponentArray<T>()->removeData(entity);
		};

		template <typename T>
		OptionalComponent<T> getComponent(EntityId entity) {
			try {
				return getComponentArray<T>()->getData(entity);
			} catch (const std::runtime_error&) {
				return std::nullopt;
			}
		};

		void onEntityDestroyed(EntityId entity);
};