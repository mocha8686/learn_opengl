#include "component.hpp"

#include <iosfwd>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeinfo>

void ComponentManager::onEntityDestroyed(Entity entity) {
	for (const auto &[_, componentArray] : componentArrays) {
		componentArray->onEntityDestroyed(entity);
	}
}