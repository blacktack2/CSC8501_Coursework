#include "file_handle.h"

#include <filesystem>
#include <fstream>

#define SEQUENCE_PATH(filename) SEQUENCE_DIRECTORY + filename + SEQUENCE_EXTENSION
#define EXPRESSION_PATH(filename) EXPRESSION_DIRECTORY + filename + EXPRESSION_EXTENSION

FileHandler::FileHandler() {

}

bool FileHandler::readSets(std::string filename, std::vector<Algebra::set_t>& sets) {
	if (!checkDirectory(SEQUENCE_DIRECTORY)) return false;
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

bool FileHandler::writeSets(std::string filename, const std::vector<Algebra::set_t> sets) {
	if (!checkDirectory(SEQUENCE_DIRECTORY)) return false;
	std::ofstream file;
	file.open(SEQUENCE_PATH(filename));
	writeSets(file, sets);
	file.close();
	return true;
}

bool FileHandler::appendSet(std::string filename, const Algebra::set_t set) {
	if (!checkDirectory(SEQUENCE_DIRECTORY)) return false;
	std::ofstream file;
	file.open(SEQUENCE_PATH(filename), std::ios::app);
	appendSet(file, set);
	file.close();
	return true;
}

bool FileHandler::readExpressions(std::string filename, std::vector<Algebra::Polynomial>& expressions) {
	if (!checkDirectory(EXPRESSION_DIRECTORY)) return false;
	std::ifstream file;
	file.open(EXPRESSION_PATH(filename));
	if (!file.is_open()) {
		mCurrentErrorState = FileNotFound;
		return false;
	}
	readExpressions(file, expressions);
	file.close();
	mCurrentErrorState = NoError;
	return true;
}

bool FileHandler::writeExpressions(std::string filename, const std::vector<Algebra::Polynomial> expressions) {
	if (!checkDirectory(EXPRESSION_DIRECTORY)) return false;
	std::ofstream file;
	file.open(EXPRESSION_PATH(filename));
	writeExpressions(file, expressions);
	file.close();
	return true;
}

bool FileHandler::appendExpression(std::string filename, const Algebra::Polynomial expressions) {
	if (!checkDirectory(EXPRESSION_DIRECTORY)) return false;
	std::ofstream file;
	file.open(EXPRESSION_PATH(filename), std::ios::app);
	appendExpression(file, expressions);
	file.close();
	return true;
}

bool FileHandler::sequenceFileExists(std::string filename) {
	std::vector<std::string> files;
	return getSequenceFiles(files) && std::find(files.begin(), files.end(), filename) != files.end();
}

bool FileHandler::expressionFileExists(std::string filename) {
	std::vector<std::string> files;
	return getExpressionFiles(files) && std::find(files.begin(), files.end(), filename) != files.end();
}

bool FileHandler::getSequenceFiles(std::vector<std::string>& filenames) {
	if (!checkDirectory(SEQUENCE_DIRECTORY)) return false;
	for (const auto& entry : std::filesystem::directory_iterator(SEQUENCE_DIRECTORY)) {
		std::string filepath = entry.path().string();
		std::smatch m;
		if (std::regex_search(filepath, m, SEQUENCE_FILE_REGEX))
			filenames.push_back(m[1]);
	}
	return true;
}

bool FileHandler::getExpressionFiles(std::vector<std::string>& filenames) {
	if (!checkDirectory(EXPRESSION_DIRECTORY)) return false;
	for (const auto& entry : std::filesystem::directory_iterator(EXPRESSION_DIRECTORY)) {
		std::string filepath = entry.path().string();
		std::smatch m;
		if (std::regex_search(filepath, m, EXPRESSION_FILE_REGEX))
			filenames.push_back(m[1]);
	}
	return true;
}

std::string FileHandler::getError() {
	return ERROR_MESSAGES.find(mCurrentErrorState)->second;
}

bool FileHandler::readSets(std::ifstream& stream, std::vector<Algebra::set_t>& sets) {

	return true;
}

bool FileHandler::writeSets(std::ofstream& stream, const std::vector<Algebra::set_t> sets) {
	std::vector<std::string> setLines;
	for (auto& set : sets)
		setLines.push_back(Algebra::setToString(set));
	std::ostringstream oss;
	if (!setLines.empty()) {
		std::copy(setLines.begin(), setLines.end(), std::ostream_iterator<std::string>(oss, SEQUENCE_DELIMITER.c_str()));
	}
	stream << oss.str();
	return true;
}

bool FileHandler::appendSet(std::ofstream& stream, const Algebra::set_t set) {
	stream << Algebra::setToString(set) << SEQUENCE_DELIMITER;
	return true;
}

bool FileHandler::readExpressions(std::ifstream& stream, std::vector<Algebra::Polynomial>& expressions) {
	return true;
}

bool FileHandler::writeExpressions(std::ofstream& stream, const std::vector<Algebra::Polynomial> expressions) {
	for (const auto& expression : expressions)
		stream << expression.toString() << EXPRESSION_DELIMITER;
	return true;
}

bool FileHandler::appendExpression(std::ofstream& stream, const Algebra::Polynomial expression) {
	stream << expression.toString() << EXPRESSION_DELIMITER;
	return true;
}

bool FileHandler::checkDirectory(std::string dir) {
	if (!std::filesystem::exists(dir)) {
		mCurrentErrorState = DirectoryMissing;
		return false;
	}
	return true;
}
