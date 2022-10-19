#pragma once

#include <functional>
#include <iostream>
#include <optional>
#include <stack>
#include <string>
#include <vector>

#include "file_handle.h"
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
	std::optional<int> castUserInputInt(std::string input);

	void softPopMenu();

	void handlePrompt();
	void parseInput(std::string input);
	void parseDataInput(std::string data);

	typedef std::function<void()> user_action_t;
	struct ActionData {
		std::string actionIdentifier;
		user_action_t action;
		std::string actionHelpPrompt;
	};
	typedef std::vector<ActionData> action_map_t;
	// @return Prompt to be displayed before the list of actions
	typedef std::function<std::string()> action_prompt_t;
	// @param User input @return <amount to increment data action pointer (usually 1 or 0), prompt>
	typedef std::function<std::pair<int, std::string>(std::string)> data_action_t;

	struct DataActionSet {
		action_prompt_t dataActionPrefixPrompt;
		data_action_t dataAction;
	};

	struct MenuContent {
		action_prompt_t actionPrefixPrompt;
		action_map_t actionMap;
		std::vector<DataActionSet> dataActions;
	};
	
	const MenuContent ROOT_MENU = {
		[this]() {
			std::string prompt = mCurrentPolynomial.isLoaded() ? "Currently loaded polynomial: (" + mCurrentPolynomial.toString() + ")\n" : "No polynomial loaded...\n";
			if (mCurrentSequence.size() == 0) {
				prompt += "No sequence loaded...\n";
			} else {
				prompt += "Currently loaded sequence: [";
				for (const auto& y : mCurrentSequence)
					prompt += " " + std::to_string(y);
				prompt += " ]\n";
			}
			return prompt;
		} ,
		{
			{"create", [this]() { mMenuStack.push({CREATE_MENU}); }, "Create a new polynomial"},
			{"generate", [this]() {
				if (!mCurrentPolynomial.isLoaded())
					std::cout << "Must load or create a polynomial in order to generate a sequence\n";
				else
					mMenuStack.push({GENERATE_MENU});
			}, "Generate polynomial from the currently loaded sequence"},
			{"save", [this]() { mMenuStack.push({SAVE_MENU}); }, "Save current polynomial/sequence to a file"},
			{"load", [this]() { mMenuStack.push({LOAD_MENU}); }, "Load polynomial/sequence from a file"},
			{"quit", [this]() { stopLoop(); }, ""},
		},
		{

		}
	};
	const MenuContent CREATE_MENU = {
		[this]() { return "Creating new polynomial...\n";  },
		{
			{"back", [this]() { softPopMenu(); }, ""},
		},
		{
			{
				[this]() { return "Type in your expression in the format:\nAx^4 + Bx^3 + Cx^2 + Dx + E\n"; },
				[this](std::string input) {
					mCurrentPolynomial.clear();
					mCurrentPolynomial.parseFrom(input);
					if (mCurrentPolynomial.getErrorState() == Algebra::Regex::Error::State::NoError)
						return std::make_pair(1, std::string("Polynomial created successfuly\n"));
					else
						return std::make_pair(0, "[Error] " + Algebra::Regex::Error::ERROR_MESSAGES.find(mCurrentPolynomial.getErrorState())->second + "\n");
				}
			}
		}
	};
	int sequenceStart, sequenceEnd;
	const MenuContent GENERATE_MENU = {
		[this]() { return "Generating new sequence...\n";  },
		{
			{"back", [this]() { softPopMenu(); }, ""},
		},
		{
			{
				[this]() { return "Sequence start:\n"; },
				[this](std::string input) {
					std::optional<int> parsedInput = castUserInputInt(input);
					if (parsedInput.has_value()) {
						sequenceStart = parsedInput.value();
						return std::make_pair(1, std::string());
					}
					return std::make_pair(0, std::string("[Error] Expected an integer\n"));
				}
			},
			{
				[this]() { return "Sequence end:\n"; },
				[this](std::string input) {
					std::optional<int> parsedInput = castUserInputInt(input);
					if (parsedInput.has_value()) {
						sequenceEnd = parsedInput.value();
						return std::make_pair(1, std::string());
					}
					return std::make_pair(0, std::string("[Error] Expected an integer\n"));
				}
			},
			{
				[this]() { return "Sequence step:\n"; },
				[this](std::string input) {
					std::optional<int> parsedInput = castUserInputInt(input);
					if (parsedInput.has_value()) {
						int sequenceStep = parsedInput.value();
						std::vector<int> sequence;
						for (int i = sequenceStart; i <= sequenceEnd; i += sequenceStep)
							sequence.push_back(i);
						mCurrentSequence = mCurrentPolynomial.apply(sequence);
						return std::make_pair(1, std::string());
					}
					return std::make_pair(0, std::string("[Error] Expected an integer\n"));
				}
			},
		}
	};
	const MenuContent SAVE_MENU = {
		[this]() { return "Save...\n";  },
		{
			{"polynomial", [this]() {}, "Save current polynomial to a file"},
			{"sequence", [this]() { mMenuStack.push({SAVE_SEQUENCE_MENU}); }, "Save current sequence to a file"},
			{"back", [this]() { softPopMenu(); }, ""},
		},
		{

		}
	};
	const MenuContent SAVE_SEQUENCE_MENU = {
		[this]() { return "Save sequence...\n";  },
		{
			{"back", [this]() { softPopMenu(); }, ""},
		},
		{
			{
				[this]() { return "What do you want to call the file?\n"; },
				[this](std::string input) {
					if (mFileHandler.sequenceFileExists(input))
						return std::make_pair(0, std::string("File already exists\n"));
					else if (mFileHandler.writeSets(input, std::vector<std::vector<int>>{mCurrentSequence}))
						return std::make_pair(1, "Successfully saved sequence to '" + input + "'\n");
					return std::make_pair(0, "[Error] " + mFileHandler.getError() + "\n");
				}
			}
		}
	};
	const MenuContent LOAD_MENU = {
		[this]() { return "";  },
		{
			{"polynomial", [this]() {}, "Load polynomial from a file"},
			{"sequence", [this]() {}, "Load sequence from a file"},
			{"back", [this]() { softPopMenu(); }, ""},
		},
		{

		}
	};

	struct MenuStackData {
		MenuContent content;
		int currentDataAction = 0;
		bool justEntered = true;
	};

	std::stack<MenuStackData> mMenuStack;

	bool mIsRunning = false;
	Algebra::Polynomial mCurrentPolynomial;
	std::vector<int> mCurrentSequence;

	FileHandler mFileHandler;
};
