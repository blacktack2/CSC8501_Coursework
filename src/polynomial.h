#pragma once

#include <string>
#include <regex>

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
			const std::regex UNKNOWN_SYMBOL("");
		}
	}

	namespace Limits {
		const int MAX_CONSTANT = 1000;
		const int MAX_COEFFICIENT = 9;
		const int MAX_EXPONENT = 4;
	}

	class Polynomial {
	public:
		enum State {
			NotParsed,
			ParseSuccessful,
			UnknownError
		};
		Polynomial();

		void clear();
		void parseFrom(std::string expression);
		std::string toString();

		State getState();
	private:
		bool isExpressionValid(std::string expression);
		bool doCoefficientsExeedMax(std::string expression);
		State findExpressionError(std::string expression);

		void calculateCoefficients(std::string expression, int (&coeffs)[Limits::MAX_EXPONENT + 1]);

		State mCurrentState;
		int mCoefficients[Limits::MAX_EXPONENT + 1];
	};
}
