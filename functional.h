#pragma once

namespace te
{
	struct first_
	{
		auto& operator()(auto& pair) {
			return pair.first;
		}

		auto const& operator()(auto const& pair) {
			return pair.first;
		}
	};

	constexpr auto first = first_{};

	template<class F, class G>
	auto on2(F&& f, G&& g) {
		return [=](auto&& left, auto&& right) {
			return std::invoke(
			    f,
			    std::invoke(g, std::forward<decltype(left)>(left)),
			    std::invoke(g, std::forward<decltype(right)>(right))
			);
		};
	}

	template<class F>
	auto copy(F&& f) {
		return [=](auto&& val) {
			return auto(std::invoke(f, std::forward<decltype(val)>(val)));
		};
	}
}
