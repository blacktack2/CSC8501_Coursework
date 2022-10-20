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

	bool readSequences(std::string filename, std::vector<Algebra::Sequence>& sequences);
	bool writeSequences(std::string filename, const std::vector<Algebra::Sequence> sequences);
	bool appendSequences(std::string filename, const std::vector<Algebra::Sequence> sequences);

	bool readExpressions(std::string filename, std::vector<Algebra::Polynomial>& expressions);
	bool writeExpressions(std::string filename, const std::vector<Algebra::Polynomial> expressions);
	bool appendExpressions(std::string filename, const std::vector<Algebra::Polynomial> expressions);

	bool sequenceFileExists(std::string filename);
	bool expressionFileExists(std::string filename);

	bool getSequenceFiles(std::vector<std::string>& filenames);
	bool getExpressionFiles(std::vector<std::string>& filenames);

	std::string getError();
private:
	bool readSequences(std::ifstream& stream, std::vector<Algebra::Sequence>& sequences);
	bool writeSequences(std::ofstream& stream, const std::vector<Algebra::Sequence> sequences);

	bool readExpressions(std::ifstream& stream, std::vector<Algebra::Polynomial>& expressions);
	bool writeExpressions(std::ofstream& stream, const std::vector<Algebra::Polynomial> expressions);

	bool checkDirectory(std::string dir);

	const std::string SEQUENCE_EXTENSION = ".sequence";
	const std::string EXPRESSION_EXTENSION = ".expression";

	const std::string SEQUENCE_DIRECTORY = "resources/sequences/";
	const std::string EXPRESSION_DIRECTORY = "resources/expressions/";

	const std::regex SEQUENCE_FILE_REGEX = std::regex(".*\\/([a-zA-Z]+)\\.sequence");
	const std::regex EXPRESSION_FILE_REGEX = std::regex(".*\\/([a-zA-Z]+)\\.expression");

	const std::string SEQUENCE_DELIMITER = "\n";
	const std::string EXPRESSION_DELIMITER = "\n";

	enum ErrorState {
		NoError,
		FileNotFound,
		MalformedExpression,
		MalformedSequence,
		DirectoryMissing,
	};
	ErrorState mCurrentErrorState = NoError;

	const std::map<ErrorState, std::string> ERROR_MESSAGES = {
		{NoError, ""},
		{FileNotFound, "File not found"},
		{DirectoryMissing, "Missing resource directory"},
	};
};
