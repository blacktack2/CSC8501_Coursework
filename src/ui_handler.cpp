#include "ui_handler.h"

#include <iostream>

UIHandler::UIHandler() : mIsRunning(false) {

}

void UIHandler::mainloop() {
	mIsRunning = true;
	while (mIsRunning) {
		std::cout << "What would you like to do?\nquit -- Exit program\ncreate -- Create a sequence from an algebraic expression\n";
		std::string action = requestUserInput();
		if (USER_ACTIONS.count(action))
			((*this).*(USER_ACTIONS.find(action)->second))();
		else
			std::cout << "Unrecognized command: [" << action << "]\n";
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

void UIHandler::userCreatePolynomial() {
	std::cout << "Type in your expression in the format: Ax^4 + Bx^3 + Cx^2 + Dx + E\n";
	std::string expression = requestUserInput();
	Algebra::Polynomial polynomial;
	polynomial.parseFrom(expression);
	if (polynomial.getState() == Algebra::Polynomial::State::ParseSuccessful) {
		std::cout << "Your expression is: " << polynomial.toString() << "\n";
		hangUntilEnterPressed(false);
	} else {
		std::cout << "Invalid polynomial!\n";
	}
}
