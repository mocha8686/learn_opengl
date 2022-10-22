#pragma once

#include "keyboard.hpp" // IWYU pragma: export
#include "cursor.hpp" // IWYU pragma: export
#include <memory>
#include <set>

class Context;

class InputManager {
	private:
		Context &ctx;
		std::unique_ptr<Keyboard> keyboard;
		std::unique_ptr<Cursor> cursor;
		std::set<CursorPosCallback> cursorPosCallbacks;

	public:
		InputManager(Context &ctx) :
			ctx(ctx),
			keyboard(std::make_unique<Keyboard>(ctx)),
			cursor(std::make_unique<Cursor>(ctx)) {};

		void process();

		bool addKeyCallback(int key, Action action, KeyCallback cb) { return keyboard->addCallback(key, action, cb); };
		bool removeKeyCallback(int key, Action action) { return keyboard->removeCallback(key, action); };
		bool addCursorPosCallback(CursorPosCallback cb) { return cursor->addCallback(cb); };
		bool removeCursorPosCallback(CursorPosCallback cb) { return cursor->addCallback(cb); };

		void resetFirstMouse() { cursor->resetFirstMouse(); };
};