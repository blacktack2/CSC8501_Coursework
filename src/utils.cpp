#include "utils.h"

#include <numeric>

namespace Utils {
	std::string join(std::vector<std::string> stringVec) {
		return std::accumulate(
			stringVec.begin(), stringVec.end(), std::string(),
			[](std::string l, std::string r) { return l + (l == "" ? "" : " | ") + r; }
		);
	}
}
