#include "ui_handler.h"

#include <iostream>

UIHandler::UIHandler() : mIsRunning(false), mCurrentPolynomial(), mActionStack() {
	mActionStack.push(ROOT_ACTIONS);
}

void UIHandler::mainloop() {
	mIsRunning = true;
	while (mIsRunning) {
		handlePrompt();
		performAction(requestUserInput());
	}
	hangUntilEnterPressed(true);
}

void UIHandler::stopLoop() {
	mIsRunning = false;
}

void UIHandler::hangUntilEnterPressed(bool isProgramExit) {
	std::cout << "Press enter to " << (isProgramExit ? "exit" : "continue") << "...";
	std::cin.ignore();
	std::cin.get();
}

std::string UIHandler::requestUserInput() {
	std::string action;
	std::cout << USER_INPUT_PROMPT;
	std::cin >> action;
	return action;
}

void UIHandler::printPolynomial() {
	if (mCurrentPolynomial.isLoaded())
		std::cout << "Currently loaded polynomial:\n" << mCurrentPolynomial.toString() << "\n";
	else
		std::cout << "No polynomial loaded...\n";
}

void UIHandler::softPopActionStack() {
	if (mActionStack.size() <= 1)
		std::cout << "[Warning] Attempted to empty stack!\n";
	else
		mActionStack.pop();
	auto x = [this]() {};
}

void UIHandler::handlePrompt() {
	ActionSet currentSet = mActionStack.top();
	currentSet.actionPrefixPrompt();
	for (const auto& action : currentSet.actionMap)
		std::cout << action.actionIdentifier << HELP_MESSAGE_SEPARATOR << action.actionHelpPrompt << "\n";
}

void UIHandler::performAction(std::string action) {
	action_map_t actionMap = mActionStack.top().actionMap;
	for (ActionData actionData : actionMap) {
		if (actionData.actionIdentifier == action) {
			((*this).*(actionData.action))();
			return;
		}
	}
	std::cout << "Unrecognized command: [" << action << "]\n";
}

void UIHandler::uiCreatePolynomial() {
	while (true) {
		mCurrentPolynomial.clear();
		std::cout << "Type in your expression in the format:\nAx^4 + Bx^3 + Cx^2 + Dx + E\n(Type b to go back)\n";
		std::string expression = requestUserInput();
		if (expression == "b")
			break;
		mCurrentPolynomial.parseFrom(expression);
		if (mCurrentPolynomial.getErrorState() == Algebra::Regex::Error::State::NoError)
			break;
		else
			std::cout << Algebra::Regex::Error::ERROR_MESSAGES.find(mCurrentPolynomial.getErrorState())->second << "\n";
	}
}

void UIHandler::uiHandlePolynomial() {
}
