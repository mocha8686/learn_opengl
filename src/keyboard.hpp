#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <map>
#include <utility>

enum Action {
	PRESS,
	RELEASE,
	RISING,
	FALLING,
};

using Key = std::pair<int, Action>; // key, action
using KeyCallback = void (*)(GLFWwindow *window);
using Data = std::pair<KeyCallback, int>; // cb, prev state

class Keyboard {
	private:
	 	GLFWwindow *window;
		std::map<Key, Data> callbacks;

	public:
		Keyboard(GLFWwindow *window) : window(window) {};

		bool addCallback(int key, Action action, KeyCallback cb);
		bool removeCallback(int key, Action action);

		void process();
};