#pragma once

#include <functional>
#include <map>
#include <regex>
#include <string>

namespace Algebra {
	namespace Regex {
		namespace Validate {
			const std::regex POLYNOMIAL("^-?(([0-9]?x(\\^[0-4])?)|([1-9]|[1-9][0-9]|[1-9][0-9][0-9]|1000))((\\+|-)(([0-9]?x(\\^[0-4])?)|([1-9]|[1-9][0-9]|[1-9][0-9][0-9]|1000)))*$");
			const std::regex SEQUENCE("^(-?[0-9]+(,-?[0-9])+)?$");
		}
		namespace Search {
			const std::regex POLYNOMIAL_COMPONENT("(?=(^|\\+|-)(([1-9]|[1-9][0-9]|[1-9][0-9][0-9]|1000)|(([1-9])?x(\\^([0-4]))?))($|\\+|-))");
			const int COMPONENT_MATCH_SIGN        = 1;
			const int COMPONENT_MATCH_COEFFICIENT = 5;
			const int COMPONENT_MATCH_EXPONENT    = 7;
			const int COMPONENT_MATCH_CONSTANT    = 3;
			const std::regex SEQUENCE_ELEMENT("(^|,)(-?[0-9])(,|$)");
			const int ELEMENT_MATCH = 2;
		}
		namespace Error {
			const std::regex POLYNOMIAL_UNKNOWN_SYMBOL = std::regex("[^x1-9\\^+-]");
			const std::regex SEQUENCE_UNKNOWN_SYMBOL = std::regex("[^1-9,-]");
		}
	}

	namespace Limits {
		const int MAX_CONSTANT = 1000;
		const int MAX_COEFFICIENT = 9;
		const int MAX_EXPONENT = 4;
	}

	class Sequence {
	public:
		Sequence();
		Sequence(Sequence& other);
		Sequence& operator=(Sequence& other);
		Sequence(Sequence&& other);
		Sequence& operator=(Sequence&& other);

		std::vector<int>::iterator begin();
		std::vector<int>::const_iterator begin() const;
		std::vector<int>::iterator end();
		std::vector<int>::const_iterator end() const;

		void clear();
		void generateFrom(int start, int end, int step);
		bool parseFrom(std::string seqExpression);
		std::string toString() const;

		std::string getError();
		bool isLoaded() const;
	private:
		enum ParseErrorState {
			NoError,
			UnknownError
		};
		bool isExpressionValid(std::string seqExpression);
		ParseErrorState findExpressionError(std::string seqExpression) const;

		void parseString(std::string seqExpression, std::vector<int>& elements) const;

		std::vector<int> mElements;
		bool mIsLoaded = false;

		ParseErrorState mCurrentErrorState = NoError;

		const std::regex UNKNOWN_SYMBOL = std::regex("[^x1-9\\^+-]");
		const std::map<ParseErrorState, std::string> ERROR_MESSAGES = {
			{NoError, ""},
			{UnknownError, "Unknown Error"}
		};

		typedef std::function<bool(std::string)> error_check_t;
		struct ErrorCheck {
			ParseErrorState state;
			error_check_t doCheck;
		};
		const std::vector<ErrorCheck> ERROR_CHECKS = {
			
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
		std::string toString() const;

		std::string getError() const;
		bool isLoaded() const;

		void apply(Sequence& sequence) const;
	private:
		enum ParseErrorState {
			NoError,
			UnknownSymbol,
			UnknownError
		};
		bool isExpressionValid(std::string expression);
		bool doCoefficientsExeedMax(std::string expression);
		ParseErrorState findExpressionError(std::string expression) const;

		void calculateCoefficients(std::string expression, int (&coeffs)[Limits::MAX_EXPONENT + 1]);

		int mCoefficients[Limits::MAX_EXPONENT + 1];
		bool mIsLoaded = false;

		ParseErrorState mCurrentErrorState = NoError;

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
			{UnknownSymbol, [this](std::string expression) { return std::regex_match(expression, Regex::Error::POLYNOMIAL_UNKNOWN_SYMBOL); }}
		};
	};
}
