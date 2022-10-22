#pragma once

#include "../context.hpp"
#include <set>
#include <functional>

using CursorPosCallback = std::function<void (Context &ctx, double xOffset, double yOffset)>;
using CursorPosCallbackPair = std::pair<unsigned long, CursorPosCallback>;

auto cmpCbPair = [](CursorPosCallbackPair p1, CursorPosCallbackPair p2) { return p1.first < p2.first; };

class Cursor {
	private:
		Context &ctx;
		std::set<CursorPosCallbackPair, decltype(cmpCbPair)> callbacks { cmpCbPair };

		float lastX = ctx.screen.width / 2;
		float lastY = ctx.screen.height / 2;
		bool firstMouse = true;

	public:
		Cursor(Context &ctx) : ctx(ctx) {};

		void process();
		bool addCallback(CursorPosCallback cb);
		bool removeCallback(CursorPosCallback cb);
		void resetFirstMouse() { firstMouse = true; };
};