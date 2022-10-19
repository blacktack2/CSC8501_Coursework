#pragma once

#include <functional>
#include <map>
#include <regex>
#include <string>

namespace Algebra {
	namespace Regex {
		namespace Validate {
			const std::regex POLYNOMIAL("^-?(([0-9]?x(\\^[0-4])?)|([1-9]|[1-9][0-9]|[1-9][0-9][0-9]|1000))((\\+|-)(([0-9]?x(\\^[0-4])?)|([1-9]|[1-9][0-9]|[1-9][0-9][0-9]|1000)))*$");
		}
		namespace Search {
			const std::regex COMPONENT("(?=(^|\\+|-)(([1-9]|[1-9][0-9]|[1-9][0-9][0-9]|1000)|(([1-9])?x(\\^([0-4]))?))($|\\+|-))");
			const int COMPONENT_MATCH_SIGN        = 1;
			const int COMPONENT_MATCH_COEFFICIENT = 5;
			const int COMPONENT_MATCH_EXPONENT    = 7;
			const int COMPONENT_MATCH_CONSTANT    = 3;
		}
		namespace Error {
			const std::regex UNKNOWN_SYMBOL("[^x1-9\\^+-]");

			enum State {
				NoError,
				UnknownSymbol,
				UnknownError
			};
			const std::map<State, std::string> ERROR_MESSAGES = {
				{NoError, ""},
				{UnknownSymbol, "Unknown Symbol - One or more characters not recognized"},
				{UnknownError, "Unknown Error"}
			};

			typedef std::function<bool(std::string)> error_check_t;
			struct ErrorCheck {
				State state;
				error_check_t doCheck;
			};
			const std::vector<ErrorCheck> ERROR_CHECKS = {
				{
					UnknownSymbol,
					[](std::string expression) { return std::regex_match(expression, UNKNOWN_SYMBOL); }
				}
			};
		}
	}

	typedef std::vector<int> set_t;

	namespace Limits {
		const int MAX_CONSTANT = 1000;
		const int MAX_COEFFICIENT = 9;
		const int MAX_EXPONENT = 4;
	}

	class Polynomial {
	public:
		Polynomial();

		void clear();
		void parseFrom(std::string expression);
		std::string toString();

		Regex::Error::State getErrorState();
		bool isLoaded();

		set_t apply(set_t range);
	private:
		bool isExpressionValid(std::string expression);
		bool doCoefficientsExeedMax(std::string expression);
		Regex::Error::State findExpressionError(std::string expression);

		void calculateCoefficients(std::string expression, int (&coeffs)[Limits::MAX_EXPONENT + 1]);

		Regex::Error::State mCurrentErrorState = Regex::Error::NoError;
		int mCoefficients[Limits::MAX_EXPONENT + 1];
		bool mIsLoaded = false;
	};
}
