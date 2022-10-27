#include "component.hpp"

#include <type_traits>

void ComponentManager::onEntityDestroyed(EntityId entity) {
	for (const auto &[_, componentArray] : componentArrays) {
		componentArray->onEntityDestroyed(entity);
	}
}