#include "polynomial.h"

#include <numeric>
#include <ranges>
#include <sstream>

namespace Algebra {

	// Matrix operations based on: https://www.geeksforgeeks.org/adjoint-inverse-matrix/
	Matrix getCofactor(Matrix m, int p, int q) {
		Matrix cofactor(m.n);
		int i = 0, j = 0;
		for (int row = 0; row < m.n; row++) {
			for (int col = 0; col < m.n; col++) {
				if (row != p && col != q) {
					cofactor.matrix[i][j++] = m.matrix[row][col];
					if (j == m.n - 1) {
						j = 0;
						i++;
					}
				}
			}
		}
		return cofactor;
	}

	int getDeterminant(Matrix m, int n) {
		int D = 0;
		if (n == 1)
			return m.matrix[0][0];
		int sign = 1;
		for (int f = 0; f < n; f++) {
			Matrix cofactor = getCofactor(m, 0, f);
			D += sign * m.matrix[0][f] * getDeterminant(cofactor, n - 1);
			sign *= -1;
		}

		return D;
	}

	Matrix getAdjoint(Matrix m) {
		Matrix adj(m.n);
		if (m.n == 1) {
			adj.matrix[0][0] = 1;
			return adj;
		}
		int sign = 1;
		for (int i = 0; i < m.n; i++) {
			for (int j = 0; j < m.n; j++) {
				Matrix cofactor = getCofactor(m, i, j);
				sign = ((i + j) % 2 == 0) ? 1 : -1;
				adj.matrix[j][i] = (sign) * (getDeterminant(cofactor, m.n - 1));
			}
		}
		return adj;
	}

	Matrixf::Matrixf(int n_) : n(n_), matrix(n, std::vector<float>(n, 0.0f)) {

	}

	Matrix::Matrix(int n_) : n(n_), matrix(n, std::vector<int>(n, 0)) {
		
	}

	Matrixf Matrix::getInverse() {
		Matrix adjoint = getAdjoint(*this);
		int det = getDeterminant(*this, n);
		Matrixf inverse(n);
		for (int row = 0; row < n; row++)
			for (int col = 0; col < n; col++)
				inverse.matrix[row][col] = (float) adjoint.matrix[row][col] / det;
		return inverse;
	}

	Sequence::Sequence() : elements() {
		
	}

	Sequence::Sequence(std::vector<int> elements_) : elements(elements_), mIsLoaded(true) {

	}

	Sequence::Sequence(Sequence& other) {
		*this = other;
	}

	Sequence& Sequence::operator=(Sequence& other) {
		if (mIsLoaded = other.mIsLoaded)
			elements = other.elements;
		else
			elements.clear();
		return *this;
	}

	Sequence::Sequence(Sequence&& other) {
		*this = std::move(other);
	}

	Sequence& Sequence::operator=(Sequence&& other) {
		if (mIsLoaded = other.mIsLoaded)
			elements = other.elements;
		else
			elements.clear();
		return *this;
	}

	void Sequence::clear() {
		elements.clear();
	}

	void Sequence::generateFrom(int start, int end, int step) {
		mIsLoaded = true;
		for (int i = start; i <= end; i += step)
			elements.push_back(i);
	}

	bool Sequence::parseFrom(std::string seqExpression) {
		if (!isExpressionValid(seqExpression)) {
			mCurrentErrorState = findExpressionError(seqExpression);
			return mIsLoaded = false;
		}
		parseString(seqExpression, elements);
		return mIsLoaded = true;
	}

	Sequence Sequence::differentiate() const {
		std::vector<int> newElements{};
		for (int i = 0; i < elements.size() - 1; i++)
			newElements.push_back(elements[i + 1] - elements[i]);
		return Sequence(newElements);
	}

	int Sequence::getDegree() const {
		Sequence s = Sequence(elements);
		int degree = 0;
		while (true) {
			int v = s.elements.front();
			bool same = true;
			for (int i = 1; i < s.elements.size(); i++)
				if (same &= (s.elements[i] == v))
					break;
			if (same)
				break;
			if (degree++ > Limits::MAX_EXPONENT)
				return INT_MAX;
			s = s.differentiate();
			if (s.elements.empty())
				return INT_MAX;
		}
		return degree;
	}

	std::string Sequence::toString() const {
		if (elements.empty()) return "";
		std::ostringstream oss;
		std::copy(elements.begin(), elements.end() - 1, std::ostream_iterator<int>(oss, ","));
		oss << elements.back();
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

	std::vector<int> deriveEquations(const int degree, Sequence& sequence) {
		Matrix simultaniousLHS(degree + 1);
		std::vector<int> simultaniousRHS(degree + 1, 0);
		for (int i = 0; i < degree + 1; i++) {
			simultaniousRHS[i] = sequence.elements[i + 1];
			for (int exp = 0; exp < degree + 1; exp++)
				simultaniousLHS.matrix[i][exp] = std::pow(i + 1, exp);
		}
		std::vector<float> coeffs(degree + 1, 0);
		Matrixf inverse = simultaniousLHS.getInverse();
		for (int row = 0; row < degree + 1; row++)
			for (int col = 0; col < degree + 1; col++)
				coeffs[row] += simultaniousRHS[col] * inverse.matrix[row][col];
		std::vector<int> intCoeffs(degree + 1, 0);
		for (int i = 0; i < degree + 1; i++)
			intCoeffs[i] = (int)std::round(coeffs[i]);
		return intCoeffs;
	}

	bool Polynomial::deriveFrom(Sequence& sequence) {
		int degree = sequence.getDegree();
		if (degree > Limits::MAX_EXPONENT)
			return false;
		std::vector<int> coeffs = deriveEquations(degree, sequence);
		for (int i = 0; i < degree + 1; i++)
			mCoefficients[i] = coeffs[i];
		mIsLoaded = true;
		return true;
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
		for (auto& y : sequence.elements) {
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
		for (const auto& c : coeffs | std::views::drop(1))
			if (c > Limits::MAX_COEFFICIENT || c < -Limits::MAX_COEFFICIENT)
				return true;
		return coeffs[0] > Limits::MAX_CONSTANT;
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
