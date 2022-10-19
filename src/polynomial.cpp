#include "polynomial.h"

namespace Algebra {
	Polynomial::Polynomial() : mCoefficients() {
		clear();
	}

	void Polynomial::clear() {
		std::fill_n(mCoefficients, Limits::MAX_EXPONENT + 1, 0);
		mCurrentErrorState = Regex::Error::NoError;
		mIsLoaded = false;
	}

	void Polynomial::parseFrom(std::string expression) {
		mIsLoaded = false;
		if (!isExpressionValid(expression)) {
			mCurrentErrorState = findExpressionError(expression);
			return;
		}
		calculateCoefficients(expression, mCoefficients);
		mCurrentErrorState = Regex::Error::NoError;
		mIsLoaded = true;
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

	Regex::Error::State Polynomial::getErrorState() {
		return mCurrentErrorState;
	}

	bool Polynomial::isLoaded() {
		return mIsLoaded;
	}

	Algebra::set_t Polynomial::apply(Algebra::set_t range) {
		for (auto& y : range) {
			int x = y;
			y = 0;
			for (int exp = 0; exp <= Limits::MAX_EXPONENT; exp++)
				y += mCoefficients[exp] * std::pow(x, exp);
		}
		return range;
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

	Regex::Error::State Polynomial::findExpressionError(std::string expression) {
		for (const auto& errorCheck : Regex::Error::ERROR_CHECKS)
			if (errorCheck.doCheck(expression))
				return errorCheck.state;
		return Regex::Error::UnknownError;
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
