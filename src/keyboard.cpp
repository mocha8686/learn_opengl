#include "context.hpp"
#include "keyboard.hpp"
#include <GLFW/glfw3.h>

bool Keyboard::addCallback(int key, Action action, KeyCallback cb) {
	std::pair keyPair(key, action);
	if (callbacks.count(keyPair) == 1) return false;
	callbacks[keyPair] = std::pair(cb, GLFW_RELEASE);
	return true;
}

bool Keyboard::removeCallback(int key, Action action) {
	std::pair keyPair(key, action);
	if (callbacks.count(keyPair) == 1) return false;
	callbacks.erase(keyPair);
	return true;
}

void Keyboard::process() {
	for (const auto &[keyPair, valPair] : callbacks) {
		auto [key, action] = keyPair;
		auto [cb, prevState] = valPair;
		int state;
		switch (action) {
			case PRESS:
				if (glfwGetKey(ctx.window, key) == GLFW_PRESS) (*cb)(ctx);
				break;
			case RELEASE:
				if (glfwGetKey(ctx.window, key) == GLFW_RELEASE) (*cb)(ctx);
				break;
			case RISING:
				state = glfwGetKey(ctx.window, key);
				if (prevState == GLFW_RELEASE && state == GLFW_PRESS) (*cb)(ctx);
				callbacks[keyPair].second = state;
				break;
			case FALLING:
				state = glfwGetKey(ctx.window, key);
				if (prevState == GLFW_PRESS && state == GLFW_RELEASE) (*cb)(ctx);
				callbacks[keyPair].second = state;
				break;
		}
	}
}