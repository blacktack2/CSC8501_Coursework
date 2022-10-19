#pragma once

#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>

#include "polynomial.h"

class FileHandler {
public:
	FileHandler();

	bool readSets(std::string filename, std::vector<std::vector<int>>& sets);
	bool writeSets(std::string filename, const std::vector<std::vector<int>> sets);
	bool appendSet(std::string filename, const std::vector<int> set);

	bool sequenceFileExists(std::string filename);
	bool expressionFileExists(std::string filename);

	std::vector<std::string> getSequenceFiles();
	std::vector<std::string> getExpressionFiles();

	enum ErrorState {
		NoError,
		FileNotFound
	};
	std::string getError();

	const std::map<ErrorState, std::string> ERROR_MESSAGES = {
		{NoError, ""},
		{FileNotFound, "File not found"},
	};
private:
	bool readSets(std::ifstream& stream, std::vector<std::vector<int>>& sets);
	bool writeSets(std::ofstream& stream, const std::vector<std::vector<int>> sets);
	bool appendSet(std::ofstream& stream, const std::vector<int> set);

	std::string setToString(const std::vector<int> set);

	const std::string SEQUENCE_EXTENSION = ".sequence";
	const std::string EXPRESSION_EXTENSION = ".expression";

	const std::string SEQUENCE_DIRECTORY = "resources/sequences/";
	const std::string EXPRESSION_DIRECTORY = "resources/expressions/";

	const std::regex SEQUENCE_FILE_REGEX = std::regex(".*\\/([a-zA-Z]+)\\.sequence");
	const std::regex EXPRESSION_FILE_REGEX = std::regex(".*\\/([a-zA-Z]+)\\.expression");

	const std::string SEQUENCE_DELIMITER = "\n";
	const std::string SEQUENCE_ELEMENT_DELIMITER = ",";
	const std::string EXPRESSION_DELIMITER = "\n";

	ErrorState mCurrentErrorState = NoError;
};
