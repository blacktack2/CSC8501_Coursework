#pragma once

#include <functional>
#include <map>
#include <regex>
#include <string>

namespace Algebra {
	namespace Regex {
		namespace Validate {
			const std::regex POLYNOMIAL("^-?(([0-9]?x(\\^[0-4])?)|([1-9]|[1-9][0-9]|[1-9][0-9][0-9]|1000))((\\+|-)(([0-9]?x(\\^[0-4])?)|([1-9]|[1-9][0-9]|[1-9][0-9][0-9]|1000)))*$");
			const std::regex SEQUENCE("^(-?[0-9]+(,-?[0-9]+)+)?$");
		}
		namespace Search {
			const std::regex POLYNOMIAL_COMPONENT("(?=(^|\\+|-)(([0-9]+)|(([0-9]*)x(\\^([0-9]+))?))($|\\+|-))");
			const int COMPONENT_MATCH_SIGN        = 1;
			const int COMPONENT_MATCH_COEFFICIENT = 5;
			const int COMPONENT_MATCH_EXPONENT    = 7;
			const int COMPONENT_MATCH_CONSTANT    = 3;
			const std::regex SEQUENCE_ELEMENT("(?=(^|,)(-?[0-9]+)(,|$))");
			const int ELEMENT_MATCH = 2;
		}
		namespace Error {
			const std::regex POLYNOMIAL_UNKNOWN_SYMBOL = std::regex("[^x0-9\\^+-]");
			const std::regex SEQUENCE_UNKNOWN_SYMBOL = std::regex("[^0-9,-]");
		}
	}

	namespace Limits {
		const int MAX_CONSTANT = 1000;
		const int MAX_COEFFICIENT = 9;
		const int MAX_EXPONENT = 4;
	}

	struct Matrix {
		Matrix(int n_);
		std::vector<float> operator*(std::vector<float> vec);

		int getDeterminant();
		Matrix getCofactor(int p, int q);
		Matrix getAdjoint();
		Matrix getInverse();
		
		int n;
		std::vector<std::vector<float>> matrix;
	};

	class Sequence {
	public:
		Sequence();
		explicit Sequence(std::vector<int> elements_);
		Sequence(Sequence& other);
		Sequence& operator=(Sequence& other);
		Sequence(Sequence&& other);
		Sequence& operator=(Sequence&& other);

		void clear();
		void generateFrom(int start, int end, int step);
		bool parseFrom(std::string seqExpression);

		Sequence differentiate() const;
		int getDegree() const;
		std::string toString() const;

		std::string getError();
		bool isLoaded() const;

		std::vector<int> elements;
	private:
		enum ParseErrorState {
			NoError,
			UnknownSymbol,
			UnknownError
		};
		bool isExpressionValid(std::string seqExpression);
		ParseErrorState findExpressionError(std::string seqExpression) const;

		void parseString(std::string seqExpression, std::vector<int>& elements) const;

		bool mIsLoaded = false;

		ParseErrorState mCurrentErrorState = NoError;

		const std::regex UNKNOWN_SYMBOL = std::regex("[^x1-9\\^+-]");
		const std::map<ParseErrorState, std::string> ERROR_MESSAGES = {
			{NoError, ""},
			{UnknownSymbol, "Unknown Symbol - One or more characters not recognized"},
			{UnknownError, "Unknown Error"}
		};

		typedef std::function<bool(std::string)> error_check_t;
		struct ErrorCheck {
			ParseErrorState state;
			error_check_t doCheck;
		};
		const std::vector<ErrorCheck> ERROR_CHECKS = {
			{UnknownSymbol, [this](std::string expression) { return std::regex_match(expression, Regex::Error::SEQUENCE_UNKNOWN_SYMBOL); }},
		};
	};

	class Polynomial {
	public:
		Polynomial();
		Polynomial(Polynomial& other);
		Polynomial& operator=(Polynomial& other);
		Polynomial(Polynomial&& other);
		Polynomial& operator=(Polynomial&& other);

		void clear();
		bool parseFrom(std::string expression);
		bool deriveFrom(Sequence& sequence);
		std::string toString() const;

		std::string getError() const;
		bool isLoaded() const;

		void apply(Sequence& sequence) const;
	private:
		enum ParseErrorState {
			NoError,
			UnknownSymbol,
			CoefficientTooLarge,
			ConstantTooLarge,
			UnknownError
		};
		bool isExpressionValid(std::string expression);
		bool doCoefficientsExeedMax(const int (&coeffs)[Limits::MAX_EXPONENT + 1]) const;
		bool doCoefficientsExeedMax(std::string expression) const;
		ParseErrorState findExpressionError(std::string expression) const;

		void calculateCoefficients(std::string expression, int (&coeffs)[Limits::MAX_EXPONENT + 1]) const;

		std::vector<int> deriveEquations(const int degree, Sequence& sequence, int offset, int step);

		int mCoefficients[Limits::MAX_EXPONENT + 1];
		bool mIsLoaded = false;

		ParseErrorState mCurrentErrorState = NoError;

		const int MAX_DERIVATION_OFFSET = 500;
		const int MAX_DERIVATION_STEP = 20;

		const std::map<ParseErrorState, std::string> ERROR_MESSAGES = {
			{NoError, ""},
			{UnknownSymbol, "Unknown Symbol - One or more characters not recognized"},
			{CoefficientTooLarge, "Coefficient Too Large - One or more coefficients are larger than the maximum"},
			{UnknownError, "Unknown Error"}
		};

		typedef std::function<bool(std::string)> error_check_t;
		struct ErrorCheck {
			ParseErrorState state;
			error_check_t doCheck;
		};
		const std::vector<ErrorCheck> ERROR_CHECKS = {
			{UnknownSymbol, [this](std::string expression) { return std::regex_match(expression, Regex::Error::POLYNOMIAL_UNKNOWN_SYMBOL); }},
			{CoefficientTooLarge, [this](std::string expression) { return doCoefficientsExeedMax(expression); }},
		};
	};
}
