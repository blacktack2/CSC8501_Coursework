#pragma once

#include <any>
#include <format>
#include <functional>
#include <iostream>
#include <numeric>
#include <optional>
#include <stack>
#include <string>
#include <vector>

#include "file_handle.h"
#include "polynomial.h"
#include "utils.h"

const std::string USER_INPUT_PROMPT = ">> ";
const std::string HELP_MESSAGE_SEPARATOR = " -- ";

class UIHandler {
public:
	UIHandler();

	void mainloop();
	void stopLoop();
private:
	typedef std::function<void()> user_action_t;
	class ActionData {
	public:
		ActionData(std::string identifier, user_action_t action_, std::string helpPrompt);

		std::string getIdentifier() const;
		std::string getPrompt() const;

		user_action_t action;
	private:
		std::string mIdentifier;
		std::string mHelpPrompt;
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
		std::vector<DataActionSet> dataActions{};
		std::map<std::string, std::any> availableData{};
	};

	struct MenuStackData {
		MenuContent content;
		int currentDataAction;
		bool justEntered;
		std::map<std::string, std::any> dataMap;
	};

	void hangUntilEnterPressed(bool isProgramExit);
	std::string requestUserInput();
	std::optional<int> castUserInputInt(std::string input);

	void softPopMenu();
	std::any& getCurrentMenuData(std::string name);
	void pushToMenuStack(const MenuContent& content);

	void handlePrompt();
	void parseInput(std::string input);
	void parseDataInput(std::string data);

	void printFilenames(std::vector<std::string> filenames) const;

	const MenuContent ROOT_MENU = {
		[this]() {
			std::string prompt = "Polynomials:\n";
			for (int i = 0; i < mCurrentPolynomials.size(); i++)
				prompt += "[" + std::vformat("{:" + std::to_string((int)std::log10(std::max(1, (int)mCurrentPolynomials.size() - 1)) + 1) + "}", std::make_format_args(i)) + "](" + mCurrentPolynomials[i].toString() + ")\n";
			prompt += "Sequences:\n";
			for (int i = 0; i < mCurrentSequences.size(); i++)
				prompt += "[" + std::vformat("{:" + std::to_string((int)std::log10(std::max(1, (int)mCurrentSequences.size() - 1)) + 1) + "}", std::make_format_args(i)) + "](" + mCurrentSequences[i].toString() + ")\n";
			return prompt;
		},
		{
			{"create", [this]() { pushToMenuStack(CREATE_MENU); }, "Create a new polynomial/sequence"},
			{"list", [this]() {
				std::vector<std::string> polynomials;
				std::vector<std::string> sequences;
				if (!mFileHandler.getSequenceFiles(sequences) || !mFileHandler.getExpressionFiles(polynomials)) {
					std::cout << "[Error] " << mFileHandler.getError() << "\n";
					return;
				}
				std::cout << "Polynomials: <" << Utils::join(polynomials) << ">\n";
				std::cout << "Sequences: <" << Utils::join(sequences) << ">\n";
			}, "List all available polynomial and sequence files"},
			{"save", [this]() { pushToMenuStack(SAVE_MENU); }, "Save current polynomial/sequence to a file"},
			{"load", [this]() { pushToMenuStack(LOAD_MENU); }, "Load polynomial/sequence from a file"},
			{"quit", [this]() { stopLoop(); }, ""},
		}
	};
	const MenuContent CREATE_MENU = {
		[this]() { return "Create...\n";  },
		{
			{"polynomial", [this]() { pushToMenuStack(CREATE_POLYNOMIAL_MENU); }, "Create new polynomial"},
			{"sequence", [this]() { pushToMenuStack(CREATE_SEQUENCE_MENU); }, "Create new sequence"},
			{"back", [this]() { softPopMenu(); }, ""},
		}
	};
	const MenuContent CREATE_POLYNOMIAL_MENU = {
		[this]() { return "Creating new polynomial...\n";  },
		{
			{"back", [this]() { softPopMenu(); }, ""},
		},
		{
			{
				[this]() { return "Type your expression in the format:\nAx^4 + Bx^3 + Cx^2 + Dx + E\n"; },
				[this](std::string input) {
					Algebra::Polynomial& newPolynomial = mCurrentPolynomials.emplace_back();
					if (newPolynomial.parseFrom(input)) {
						return std::make_pair(1, std::string("Polynomial created successfuly\n"));
					} else {
						std::string error = "[Error] " + newPolynomial.getError() + "\n";
						mCurrentPolynomials.pop_back();
						return std::make_pair(0, error);
					}
				}
			}
		}
	};
	const MenuContent CREATE_SEQUENCE_MENU = {
		[this]() { return "Creating new sequence...\n";  },
		{
			{"back", [this]() { softPopMenu(); }, ""},
		},
		{
			{
				[this]() { return "Sequence start:\n"; },
				[this](std::string input) {
					std::optional<int> parsedInput = castUserInputInt(input);
					if (parsedInput.has_value()) {
						int& sequenceStart = std::any_cast<int&>(getCurrentMenuData("sequenceStart"));
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
						int& sequenceEnd = std::any_cast<int&>(getCurrentMenuData("sequenceEnd"));
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
						Algebra::Sequence& newSequence = mCurrentSequences.emplace_back();
						int& sequenceStart = std::any_cast<int&>(getCurrentMenuData("sequenceStart"));
						int& sequenceEnd = std::any_cast<int&>(getCurrentMenuData("sequenceEnd"));
						newSequence.generateFrom(sequenceStart, sequenceEnd, sequenceStep);
						return std::make_pair(1, std::string());
					}
					return std::make_pair(0, std::string("[Error] Expected an integer\n"));
				}
			},
		},
		{
			{"sequenceStart", 0},
			{"sequenceEnd", 0},
		}
	};
	const MenuContent SAVE_MENU = {
		[this]() { return "Save...\n";  },
		{
			{"polynomial", [this]() { pushToMenuStack(SAVE_POLYNOMIAL_MENU); }, "Save current polynomial to a file"},
			{"sequence", [this]() { pushToMenuStack(SAVE_SEQUENCE_MENU); }, "Save current sequence to a file"},
			{"back", [this]() { softPopMenu(); }, ""},
		}
	};
	const MenuContent SAVE_POLYNOMIAL_MENU = {
		[this]() { return "Save polynomial expression...\n";  },
		{
			{"back", [this]() { softPopMenu(); }, ""},
		},
		{
			{
				[this]() { return "What do you want to call the file?\n"; },
				[this](std::string input) {
					if (mFileHandler.expressionFileExists(input)) {
						std::cout << "File already exists\n(a | append, o | overwrite, n | new name)\n";
						std::string action = requestUserInput();
						if (action == "a") {
							if (mFileHandler.appendExpressions(input, mCurrentPolynomials))
								return std::make_pair(1, "Successfully appended polynomial to '" + input + "'\n");
						} else if (action == "o") {
							if (mFileHandler.writeExpressions(input, mCurrentPolynomials))
								return std::make_pair(1, "Successfully saved polynomial to '" + input + "'\n");
						} else {
							return std::make_pair(0, std::string(""));
						}
					} else if (mFileHandler.writeExpressions(input, mCurrentPolynomials)) {
						return std::make_pair(1, "Successfully saved polynomial to '" + input + "'\n");
					}
					return std::make_pair(0, "[Error] " + mFileHandler.getError() + "\n");
				}
			}
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
					if (mFileHandler.sequenceFileExists(input)) {
						std::cout << "File already exists\n(a | append, o | overwrite, n | new name)\n";
						std::string action = requestUserInput();
						if (action == "a") {
							if (mFileHandler.appendSequences(input, mCurrentSequences))
								return std::make_pair(1, "Successfully appended sequence to '" + input + "'\n");
						} else if (action == "o") {
							if (mFileHandler.writeSequences(input, mCurrentSequences))
								return std::make_pair(1, "Successfully saved sequence to '" + input + "'\n");
						} else {
							return std::make_pair(0, std::string(""));
						}
					} else if (mFileHandler.writeSequences(input, mCurrentSequences)) {
						return std::make_pair(1, "Successfully saved sequence to '" + input + "'\n");
					}
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
		}
	};

	std::stack<MenuStackData> mMenuStack;

	bool mIsRunning = false;
	std::vector<Algebra::Polynomial> mCurrentPolynomials;
	std::vector<Algebra::Sequence> mCurrentSequences;

	FileHandler mFileHandler;
};
