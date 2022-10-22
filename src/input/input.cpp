#include "input.hpp"

void InputManager::process() {
	keyboard->process();
	cursor->process();
}