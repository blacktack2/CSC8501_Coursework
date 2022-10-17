#pragma once

#include <functional>
#include <map>
#include <string>

#include "polynomial.h"

const std::string USER_INPUT_PROMPT = ">> ";

class UIHandler {
public:
	UIHandler();

	void mainloop();
	void stopLoop();
private:
	void hangUntilEnterPressed(bool isProgramExit);
	std::string requestUserInput();

	void userCreatePolynomial();

	typedef void(UIHandler::*user_action_t)();
	const std::map<std::string, user_action_t> USER_ACTIONS = {
		{"quit"  , &UIHandler::stopLoop},
		{"create", &UIHandler::userCreatePolynomial},
	};

	bool mIsRunning;
};
