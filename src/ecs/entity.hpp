#pragma once

#include <queue>

#define MAX_ENTITIES 1024

using Entity = unsigned int;

class EntityManager {
	private:
		std::queue<Entity> availableEntities {};
		unsigned int nEntities = 0;

	public:
		EntityManager();
		Entity createEntity();
		void destroyEntity(Entity entity);
};