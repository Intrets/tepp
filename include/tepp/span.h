#pragma once

#include <span>

#include "tepp/integers.h"

namespace te
{
	namespace detail
	{
		template<class T>
		concept can_be_converted_to_span = requires(T t) { std::span(t); };
	}

	auto subspan(detail::can_be_converted_to_span auto&& span_, std::optional<integer_t> start_, std::optional<integer_t> size_) {
		auto span = std::span(span_);

		auto start = start_.value_or(0_i);
		start = std::clamp(start, 0_i, isize(span));

		auto size = size_.value_or(isize(span) - start);
		size = std::clamp(size, 0_i, isize(span) - start);

		return span.subspan(start, size);
	}

	auto slice(detail::can_be_converted_to_span auto&& span_, std::optional<integer_t> start_, std::optional<integer_t> end_) {
		auto span = std::span(span_);

		using R = decltype(span);

		auto start = start_.value_or(0_i);
		auto end = end_.value_or(isize(span));

		if (span.empty()) {
			return R();
		}

		if (start < 0) {
			start += isize(span);
		}
		if (end < 0) {
			end += isize(span);
		}

		start = std::clamp(start, 0_i, isize(span));
		end = std::clamp(end, 0_i, isize(span));

		if (end < start) {
			return R();
		}

		return span.subspan(start, end - start);
	}
}
