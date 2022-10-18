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

	typedef std::function<void()> user_action_t;
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
			{"poly", [this]() { UIHandler::uiCreatePolynomial(); }, "Create a new polynomial from an algebraic expression"},
			{"seq" , [this]() {}, "Create a new polynomial from a sequence of values"},
			{"op"  , [this]() { mActionStack.push(HANDLE_POLYNOMIAL_ACTIONS); }, "Operate on the currently loaded polynomial"},
			{"clear", [this]() { mCurrentPolynomial.clear(); }, "Clear the currently loaded polynomial"},
			{"quit", [this]() { UIHandler::stopLoop(); }, ""},
		}
	};
	const ActionSet HANDLE_POLYNOMIAL_ACTIONS = {
		[this]() {},
		{
			{"back", [this]() { UIHandler::softPopActionStack(); } , ""}
		}
	};

	std::stack<ActionSet> mActionStack;

	bool mIsRunning;
	Algebra::Polynomial mCurrentPolynomial;
};
