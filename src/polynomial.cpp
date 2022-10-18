#include "polynomial.h"

namespace Algebra {
	Polynomial::Polynomial() : mCurrentState(NotParsed), mCoefficients() {
		clear();
	}

	void Polynomial::clear() {
		std::fill_n(mCoefficients, Limits::MAX_EXPONENT + 1, 0);
		mCurrentState = NotParsed;
	}

	void Polynomial::parseFrom(std::string expression) {
		if (!isExpressionValid(expression)) {
			mCurrentState = findExpressionError(expression);
			return;
		}
		calculateCoefficients(expression, mCoefficients);
		mCurrentState = ParseSuccessful;
	}

	std::string Polynomial::toString() {
		std::vector<std::pair<std::string, bool>> components;
		for (int exponent = Limits::MAX_EXPONENT; exponent >= 0; exponent--) {
			const int coefficient = std::abs(mCoefficients[exponent]);
			const bool isNegative = mCoefficients[exponent] < 0;
			if (coefficient != 0)
				components.emplace_back(((coefficient == 1) ? "" : std::to_string(coefficient)) + ((exponent == 0) ? "" : (std::string("x") + ((exponent == 1) ? "" : ("^" + std::to_string(exponent))))), isNegative);
		}
		std::string expression{};
		for (const auto& component : components)
			expression += (component.second ? "-" : (&component == &components.front()) ? "" : "+") + component.first;
		return expression;
	}

	Polynomial::State Polynomial::getState() {
		return mCurrentState;
	}

	bool Polynomial::isExpressionValid(std::string expression) {
		if (!std::regex_match(expression, Regex::Validate::POLYNOMIAL)) {
			return false;
		}
		return !doCoefficientsExeedMax(expression);
	}

	bool Polynomial::doCoefficientsExeedMax(std::string expression) {
		int coeffs[Limits::MAX_EXPONENT + 1]{};
		calculateCoefficients(expression, coeffs);
		for (auto& c : coeffs) {
			if (c > Limits::MAX_COEFFICIENT || c < -Limits::MAX_COEFFICIENT) return true;
		}
		return false;
	}

	Polynomial::State Polynomial::findExpressionError(std::string expression) {
		return UnknownError;
	}

	void Polynomial::calculateCoefficients(std::string expression, int(&coeffs)[Limits::MAX_EXPONENT + 1]) {
		std::fill_n(coeffs, Limits::MAX_EXPONENT + 1, 0);
		for (std::sregex_iterator i(expression.begin(), expression.end(), Regex::Search::COMPONENT); i != std::sregex_iterator(); i++) {
			std::smatch m = *i;
			std::string constValue = m[Regex::Search::COMPONENT_MATCH_CONSTANT].str();
			std::string exponent = m[Regex::Search::COMPONENT_MATCH_EXPONENT].str();
			std::string coefficient = m[Regex::Search::COMPONENT_MATCH_COEFFICIENT].str();
			std::string sign = m[Regex::Search::COMPONENT_MATCH_SIGN].str();
			coeffs[(constValue == "") ? (exponent == "") ? 1 : stoi(exponent) : 0] += ((coefficient == "" && constValue == "") ? 1 : stoi(constValue == "" ? coefficient : constValue)) * (sign == "-" ? -1 : 1);
		}
	}
}
