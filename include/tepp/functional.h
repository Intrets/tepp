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

}