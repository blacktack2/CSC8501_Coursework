#include "ui_handler.h"

#include <regex>

UIHandler::ActionData::ActionData(std::string identifier, user_action_t action_, std::string helpPrompt) :
mIdentifier(identifier), action(action_), mHelpPrompt(helpPrompt) {}

std::string UIHandler::ActionData::getIdentifier() const {
	return mIdentifier;
}

std::string UIHandler::ActionData::getPrompt() const {
	return "{" + mIdentifier + "}" + (mHelpPrompt == "" ? "" : HELP_MESSAGE_SEPARATOR + mHelpPrompt);
}

UIHandler::UIHandler() :
mMenuStack(), mCurrentPolynomials(), mCurrentSequences(), mFileHandler() {
	pushToMenuStack(ROOT_MENU);
}

void UIHandler::mainloop() {
	mIsRunning = true;
	while (mIsRunning) {
		handlePrompt();
		parseInput(requestUserInput());
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
	std::string input;
	std::cout << USER_INPUT_PROMPT;
	std::getline(std::cin, input, '\n');
	return input;
}

std::optional<int> UIHandler::castUserInputInt(std::string input) {
	if (!std::regex_match(input, std::regex("-?[0-9]+")) && input != "q")
		return std::optional<int>();
	return stoi(input);
}

void UIHandler::softPopMenu() {
	if (mMenuStack.size() <= 1) {
		std::cout << "[Warning] Attempted to empty stack!\n";
	} else {
		mMenuStack.pop();
		mMenuStack.top().justEntered = true;
	}
}

std::any& UIHandler::getCurrentMenuData(std::string name) {
	return mMenuStack.top().dataMap.find(name)->second;
}

void UIHandler::pushToMenuStack(const MenuContent& content) {
	mMenuStack.push({ content, 0, true, content.availableData});
}

void UIHandler::handlePrompt() {
	MenuStackData& menuData = mMenuStack.top();
	if (menuData.justEntered) {
		std::cout << menuData.content.actionPrefixPrompt();
		menuData.justEntered = false;
	}
	for (const auto& action : menuData.content.actionMap)
		std::cout << action.getPrompt() << "\n";
	if (!menuData.content.dataActions.empty())
		std::cout << menuData.content.dataActions[menuData.currentDataAction].dataActionPrefixPrompt();
}

void UIHandler::parseInput(std::string input) {
	MenuStackData& menuData = mMenuStack.top();
	for (const auto& actionData : menuData.content.actionMap) {
		if (actionData.getIdentifier() == input) {
			menuData.justEntered = true;
			actionData.action();
			return;
		}
	}
	if (menuData.content.dataActions.empty())
		std::cout << "Unrecognized command: [" << input << "]\n";
	else
		parseDataInput(input);
}

void UIHandler::parseDataInput(std::string input) {
	MenuStackData& menuData = mMenuStack.top();
	DataActionSet currentDataAction = menuData.content.dataActions[menuData.currentDataAction];
	std::pair<int, std::string> results = currentDataAction.dataAction(input);
	std::cout << results.second;
	menuData.currentDataAction = std::max(menuData.currentDataAction + results.first, 0);
	if (menuData.currentDataAction >= menuData.content.dataActions.size())
		softPopMenu();
}

void UIHandler::printFilenames(std::vector<std::string> filenames) const {
	if (filenames.empty()) {
		std::cout << "Empty\n";
	} else {
		std::cout << "< ";
		for (std::vector<std::string>::iterator i = filenames.begin(); i != filenames.end() - 1; i++)
			std::cout << *i << " | ";
		std::cout << filenames.back();
		std::cout << " >\n";
	}
}
