#pragma once

#include <algorithm>
#include <ranges>
#include <vector>

namespace te
{
	template<class T>
	auto make_unique(std::vector<T>& data) {
		std::ranges::sort(data);
		auto range = std::ranges::unique(data);
		data.erase(range.begin(), range.end());
	}
}