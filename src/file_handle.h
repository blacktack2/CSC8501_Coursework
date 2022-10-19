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

	bool readSets(std::string filename, std::vector<Algebra::set_t>& sets);
	bool writeSets(std::string filename, const std::vector<Algebra::set_t> sets);
	bool appendSet(std::string filename, const Algebra::set_t set);

	bool readExpressions(std::string filename, std::vector<Algebra::Polynomial>& expressions);
	bool writeExpressions(std::string filename, const std::vector<Algebra::Polynomial> expressions);
	bool appendExpression(std::string filename, const Algebra::Polynomial expressions);

	bool sequenceFileExists(std::string filename);
	bool expressionFileExists(std::string filename);

	bool getSequenceFiles(std::vector<std::string>& filenames);
	bool getExpressionFiles(std::vector<std::string>& filenames);

	enum ErrorState {
		NoError,
		FileNotFound,
		DirectoryMissing,
	};
	std::string getError();

	const std::map<ErrorState, std::string> ERROR_MESSAGES = {
		{NoError, ""},
		{FileNotFound, "File not found"},
		{DirectoryMissing, "Missing resource directory"},
	};
private:
	bool readSets(std::ifstream& stream, std::vector<Algebra::set_t>& sets);
	bool writeSets(std::ofstream& stream, const std::vector<Algebra::set_t> sets);
	bool appendSet(std::ofstream& stream, const Algebra::set_t set);

	bool readExpressions(std::ifstream& stream, std::vector<Algebra::Polynomial>& expressions);
	bool writeExpressions(std::ofstream& stream, const std::vector<Algebra::Polynomial> expressions);
	bool appendExpression(std::ofstream& stream, const Algebra::Polynomial expression);

	bool checkDirectory(std::string dir);

	const std::string SEQUENCE_EXTENSION = ".sequence";
	const std::string EXPRESSION_EXTENSION = ".expression";

	const std::string SEQUENCE_DIRECTORY = "resources/sequences/";
	const std::string EXPRESSION_DIRECTORY = "resources/expressions/";

	const std::regex SEQUENCE_FILE_REGEX = std::regex(".*\\/([a-zA-Z]+)\\.sequence");
	const std::regex EXPRESSION_FILE_REGEX = std::regex(".*\\/([a-zA-Z]+)\\.expression");

	const std::string SEQUENCE_DELIMITER = "\n";
	const std::string EXPRESSION_DELIMITER = "\n";

	ErrorState mCurrentErrorState = NoError;
};
