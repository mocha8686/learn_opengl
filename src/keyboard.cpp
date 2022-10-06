#include "keyboard.hpp"
#include <GLFW/glfw3.h>

bool Keyboard::addCallback(int key, Action action, KeyCallback cb) {
	std::pair keyPair(key, action);
	if (callbacks[keyPair].first != nullptr) return false;
	callbacks[keyPair] = std::pair(cb, GLFW_RELEASE);
	return true;
}

bool Keyboard::removeCallback(int key, Action action) {
	std::pair keyPair(key, action);
	if (callbacks[keyPair].first == nullptr) return false;
	callbacks[keyPair].first = nullptr;
	return true;
}

void Keyboard::process() {
	for (const auto &[keyPair, valPair] : callbacks) {
		auto [key, action] = keyPair;
		auto [cb, prevState] = valPair;
		int state;
		switch (action) {
			case PRESS:
				if (glfwGetKey(window, key) == GLFW_PRESS) (*cb)(window);
				break;
			case RELEASE:
				if (glfwGetKey(window, key) == GLFW_RELEASE) (*cb)(window);
				break;
			case RISING:
				state = glfwGetKey(window, key);
				if (prevState == GLFW_RELEASE && state == GLFW_PRESS) (*cb)(window);
				callbacks[keyPair].second = state;
				break;
			case FALLING:
				state = glfwGetKey(window, key);
				if (prevState == GLFW_PRESS && state == GLFW_RELEASE) (*cb)(window);
				callbacks[keyPair].second = state;
				break;
		}
	}
}