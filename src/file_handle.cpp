#include "file_handle.h"

#include <filesystem>
#include <fstream>

#define SEQUENCE_PATH(filename) SEQUENCE_DIRECTORY + filename + SEQUENCE_EXTENSION
#define EXPRESSION_PATH(filename) EXPRESSION_DIRECTORY + filename + EXPRESSION_EXTENSION

FileHandler::FileHandler() {

}

bool FileHandler::readSets(std::string filename, std::vector<std::vector<int>>& sets) {
	std::ifstream file;
	file.open(SEQUENCE_PATH(filename));
	if (!file.is_open()) {
		mCurrentErrorState = FileNotFound;
		return false;
	}
	readSets(file, sets);
	file.close();
	mCurrentErrorState = NoError;
	return true;
}

bool FileHandler::writeSets(std::string filename, const std::vector<std::vector<int>> sets) {
	std::ofstream file;
	file.open(SEQUENCE_PATH(filename));
	writeSets(file, sets);
	file.close();
	return true;
}

bool FileHandler::sequenceFileExists(std::string filename) {
	std::vector<std::string> files = getSequenceFiles();
	return std::find(files.begin(), files.end(), filename) != files.end();
}

bool FileHandler::expressionFileExists(std::string filename) {
	std::vector<std::string> files = getExpressionFiles();
	return std::find(files.begin(), files.end(), filename) != files.end();
}

std::vector<std::string> FileHandler::getSequenceFiles() {
	std::vector<std::string> files;
	for (const auto& entry : std::filesystem::directory_iterator(SEQUENCE_DIRECTORY)) {
		std::string filepath = entry.path().string();
		std::smatch m;
		if (std::regex_search(filepath, m, SEQUENCE_FILE_REGEX))
			files.push_back(m[1]);
	}
	return files;
}

std::vector<std::string> FileHandler::getExpressionFiles() {
	std::vector<std::string> files;
	for (const auto& entry : std::filesystem::directory_iterator(EXPRESSION_DIRECTORY)) {
		std::string filepath = entry.path().string();
		std::smatch m;
		if (std::regex_search(filepath, m, EXPRESSION_FILE_REGEX))
			files.push_back(m[1]);
	}
	return files;
}

std::string FileHandler::getError() {
	return ERROR_MESSAGES.find(mCurrentErrorState)->second;
}

bool FileHandler::readSets(std::ifstream& stream, std::vector<std::vector<int>>& sets) {

	return true;
}

bool FileHandler::writeSets(std::ofstream& stream, const std::vector<std::vector<int>> sets) {
	std::vector<std::string> setLines;
	for (auto& set : sets)
		setLines.push_back(setToString(set));
	std::ostringstream oss;
	if (!setLines.empty()) {
		std::copy(setLines.begin(), setLines.end() - 1, std::ostream_iterator<std::string>(oss, SEQUENCE_DELIMITER.c_str()));
		oss << setLines.back();
	}
	stream << oss.str();
	return true;
}

std::string FileHandler::setToString(const std::vector<int> set) {
	if (set.empty()) return "";
	std::ostringstream oss;
	std::copy(set.begin(), set.end() - 1, std::ostream_iterator<int>(oss, SEQUENCE_ELEMENT_DELIMITER.c_str()));
	oss << set.back();
	return oss.str();
}
