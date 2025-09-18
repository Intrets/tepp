#pragma once

#include <array>
#include <format>

#include "tepp/integers.h"

namespace te
{
	template<integer_t N = 512>
	class format_exception : public std::exception
	{
		std::array<char, N> message{};

	public:
		template<class... Args>
		format_exception(std::format_string<Args...> const fmt, Args&&... args) {
			auto result = std::format_to_n(message.begin(), isize(message) - 1, fmt, std::forward<Args>(args)...);
			*result.out = '\0';
		}

		virtual char const* what() const noexcept override {
			return message.data();
		}
	};
}
