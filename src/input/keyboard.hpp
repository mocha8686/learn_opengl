#pragma once

#include "../context.hpp"
#include <functional>
#include <map>
#include <utility>

enum Action {
	PRESS,
	RELEASE,
	RISING,
	FALLING,
};

using KeyCallback = std::function<void (Context &ctx)>;
using Key = std::pair<int, Action>; // key, action
using Data = std::pair<KeyCallback, int>; // cb, prev state

class Keyboard {
	private:
	 	Context &ctx;
		std::map<Key, Data> callbacks;

	public:
		Keyboard(Context &ctx) : ctx(ctx) {};

		bool addCallback(int key, Action action, KeyCallback cb);
		bool removeCallback(int key, Action action);

		void process();
};