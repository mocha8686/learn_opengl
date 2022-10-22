#include "cursor.hpp"

#include "../context.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <utility>

void Cursor::process() {
	double xPos, yPos;
	glfwGetCursorPos(ctx.window, &xPos, &yPos);
	if (xPos == 0 && yPos == 0) return;

	if (firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	auto xOffset = xPos - lastX;
	auto yOffset = lastY - yPos;
	lastX = xPos;
	lastY = yPos;

	for (auto [_, cb] : callbacks) {
		cb(ctx, xOffset, yOffset);
	}
}

bool Cursor::addCallback(CursorPosCallback cb) {
	auto pair = std::pair(reinterpret_cast<unsigned long>(cb.target<CursorPosCallback>()), cb);
	if (callbacks.count(pair) == 1) return false;
	callbacks.insert(pair);
	return true;
}

bool Cursor::removeCallback(CursorPosCallback cb) {
	auto pair = std::pair(reinterpret_cast<unsigned long>(cb.target<CursorPosCallback>()), cb);
	if (callbacks.count(pair) != 1) return false;
	callbacks.erase(pair);
	return true;
}