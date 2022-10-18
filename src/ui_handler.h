#pragma once

#include <functional>
#include <stack>
#include <string>
#include <vector>

#include "polynomial.h"

const std::string USER_INPUT_PROMPT = ">> ";
const std::string HELP_MESSAGE_SEPARATOR = " -- ";

class UIHandler {
public:
	UIHandler();

	void mainloop();
	void stopLoop();
private:
	void hangUntilEnterPressed(bool isProgramExit);
	std::string requestUserInput();
	void printPolynomial();

	void softPopActionStack();

	void handlePrompt();
	void performAction(std::string action);

	void uiCreatePolynomial();
	void uiHandlePolynomial();

	typedef void(UIHandler::*user_action_t)();
	struct ActionData {
		std::string actionIdentifier;
		user_action_t action;
		std::string actionHelpPrompt;
	};
	typedef std::vector<ActionData> action_map_t;

	typedef std::function<void()> action_prompt_t;

	struct ActionSet {
		action_prompt_t actionPrefixPrompt;
		action_map_t actionMap;
	};
	
	const ActionSet ROOT_ACTIONS = {
		[this]() { printPolynomial(); } ,
		{
			{"create", &UIHandler::uiCreatePolynomial, "Create a sequence from an algebraic expression"},
			{"quit"  , &UIHandler::stopLoop, "Exit program"},
		}
	};
	const ActionSet HANDLE_POLYNOMIAL_ACTIONS = {
		[this]() {},
		{
			{"back", &UIHandler::softPopActionStack, ""}
		}
	};

	std::stack<ActionSet> mActionStack;

	bool mIsRunning;
	Algebra::Polynomial mCurrentPolynomial;
};
