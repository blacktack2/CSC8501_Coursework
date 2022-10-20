#include "polynomial.h"

#include <numeric>
#include <sstream>

namespace Algebra {
	Sequence::Sequence() : mElements() {
		
	}

	Sequence::Sequence(Sequence& other) {
		*this = other;
	}

	Sequence& Sequence::operator=(Sequence& other) {
		if (mIsLoaded = other.mIsLoaded)
			mElements = other.mElements;
		else
			mElements.clear();
		return *this;
	}

	Sequence::Sequence(Sequence&& other) {
		*this = std::move(other);
	}

	Sequence& Sequence::operator=(Sequence&& other) {
		if (mIsLoaded = other.mIsLoaded)
			mElements = other.mElements;
		else
			mElements.clear();
		return *this;
	}

	std::vector<int>::iterator Sequence::begin() {
		return mElements.begin();
	}

	std::vector<int>::const_iterator Sequence::begin() const {
		return mElements.begin();
	}

	std::vector<int>::iterator Sequence::end() {
		return mElements.end();
	}

	std::vector<int>::const_iterator Sequence::end() const {
		return mElements.end();
	}

	void Sequence::clear() {
		mElements.clear();
	}

	void Sequence::generateFrom(int start, int end, int step) {
		mIsLoaded = true;
		for (int i = start; i <= end; i += step)
			mElements.push_back(i);
	}

	bool Sequence::parseFrom(std::string seqExpression) {
		if (!isExpressionValid(seqExpression)) {
			mCurrentErrorState = findExpressionError(seqExpression);
			return mIsLoaded = false;
		}
		parseString(seqExpression, mElements);
		return mIsLoaded = true;
	}

	std::string Sequence::toString() const {
		if (mElements.empty()) return "";
		std::ostringstream oss;
		std::copy(mElements.begin(), mElements.end() - 1, std::ostream_iterator<int>(oss, ","));
		oss << mElements.back();
		return oss.str();
	}

	std::string Sequence::getError() {
		return ERROR_MESSAGES.find(mCurrentErrorState)->second;
	}

	bool Sequence::isLoaded() const {
		return mIsLoaded;
	}

	bool Sequence::isExpressionValid(std::string seqExpression) {
		if (!std::regex_match(seqExpression, Regex::Validate::SEQUENCE))
			return false;
		return true;
	}

	Sequence::ParseErrorState Sequence::findExpressionError(std::string seqExpression) const {
		for (const auto& errorCheck : ERROR_CHECKS)
			if (errorCheck.doCheck(seqExpression))
				return errorCheck.state;
		return UnknownError;
	}

	void Sequence::parseString(std::string seqExpression, std::vector<int>& elements) const {
		elements.clear();
		for (std::sregex_iterator i(seqExpression.begin(), seqExpression.end(), Regex::Search::SEQUENCE_ELEMENT); i != std::sregex_iterator(); i++) {
			std::smatch m = *i;
			std::string element = m[Regex::Search::ELEMENT_MATCH].str();
			elements.push_back(stoi(element));
		}
	}

	Polynomial::Polynomial() : mCoefficients() {
		clear();
	}

	Polynomial::Polynomial(Polynomial& other) {
		*this = other;
	}

	Polynomial& Polynomial::operator=(Polynomial& other) {
		if (mIsLoaded = other.mIsLoaded) {
			for (int i = 0; i < Limits::MAX_EXPONENT + 1; i++)
				mCoefficients[i] = other.mCoefficients[i];
		} else {
			std::fill_n(mCoefficients, Limits::MAX_EXPONENT + 1, 0);
		}
		mCurrentErrorState = other.mCurrentErrorState;
		return *this;
	}

	Polynomial::Polynomial(Polynomial&& other) {
		*this = std::move(other);
	}

	Polynomial& Polynomial::operator=(Polynomial&& other) {
		if (mIsLoaded = other.mIsLoaded) {
			for (int i = 0; i < Limits::MAX_EXPONENT + 1; i++)
				mCoefficients[i] = other.mCoefficients[i];
		} else {
			std::fill_n(mCoefficients, Limits::MAX_EXPONENT + 1, 0);
		}
		mCurrentErrorState = other.mCurrentErrorState;
		return *this;
	}

	void Polynomial::clear() {
		std::fill_n(mCoefficients, Limits::MAX_EXPONENT + 1, 0);
		mCurrentErrorState = NoError;
		mIsLoaded = false;
	}

	bool Polynomial::parseFrom(std::string expression) {
		if (!isExpressionValid(expression)) {
			mCurrentErrorState = findExpressionError(expression);
			return mIsLoaded = false;
		}
		calculateCoefficients(expression, mCoefficients);
		mCurrentErrorState = NoError;
		return mIsLoaded = true;
	}

	std::string Polynomial::toString() const {
		int exp = Limits::MAX_EXPONENT + 1;
		return std::accumulate(std::rbegin(mCoefficients), std::rend(mCoefficients), std::string(),
			[&exp](const std::string l, const int coeff) {
				exp--;
				return (coeff == 0) ? l : (l +
					((l == "") ? ((coeff < 0) ? "-" : "") : ((coeff < 0) ? " - " : " + ")) +
					((std::abs(coeff) == 1 && exp != 0) ? "" : std::to_string(std::abs(coeff))) +
					((exp == 0) ? "" : ("x" + ((exp == 1) ? "" : ("^" + std::to_string(exp))))));
			}
		);
	}

	std::string Polynomial::getError() const {
		return ERROR_MESSAGES.find(mCurrentErrorState)->second;
	}

	bool Polynomial::isLoaded() const {
		return mIsLoaded;
	}

	void Polynomial::apply(Sequence& sequence) const {
		for (auto& y : sequence) {
			int x = y;
			y = 0;
			for (int exp = 0; exp <= Limits::MAX_EXPONENT; exp++)
				y += mCoefficients[exp] * std::pow(x, exp);
		}
	}

	bool Polynomial::isExpressionValid(std::string expression) {
		if (!std::regex_match(expression, Regex::Validate::POLYNOMIAL))
			return false;
		return !doCoefficientsExeedMax(expression);
	}

	bool Polynomial::doCoefficientsExeedMax(std::string expression) {
		int coeffs[Limits::MAX_EXPONENT + 1]{};
		calculateCoefficients(expression, coeffs);
		for (const auto& c : coeffs)
			if (c > Limits::MAX_COEFFICIENT || c < -Limits::MAX_COEFFICIENT) return true;
		return false;
	}

	Polynomial::ParseErrorState Polynomial::findExpressionError(std::string expression) const {
		for (const auto& errorCheck : ERROR_CHECKS)
			if (errorCheck.doCheck(expression))
				return errorCheck.state;
		return UnknownError;
	}

	void Polynomial::calculateCoefficients(std::string expression, int(&coeffs)[Limits::MAX_EXPONENT + 1]) {
		std::fill_n(coeffs, Limits::MAX_EXPONENT + 1, 0);
		for (std::sregex_iterator i(expression.begin(), expression.end(), Regex::Search::POLYNOMIAL_COMPONENT); i != std::sregex_iterator(); i++) {
			std::smatch m = *i;
			std::string constValue = m[Regex::Search::COMPONENT_MATCH_CONSTANT].str();
			std::string exponent = m[Regex::Search::COMPONENT_MATCH_EXPONENT].str();
			std::string coefficient = m[Regex::Search::COMPONENT_MATCH_COEFFICIENT].str();
			std::string sign = m[Regex::Search::COMPONENT_MATCH_SIGN].str();
			coeffs[(constValue == "") ? (exponent == "") ? 1 : stoi(exponent) : 0] += ((coefficient == "" && constValue == "") ? 1 : stoi(constValue == "" ? coefficient : constValue)) * (sign == "-" ? -1 : 1);
		}
	}
}
