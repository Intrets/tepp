#pragma once

#include "tepp/assert.h"

#include <format>
#include <stdexcept>
#include <typeinfo>
#include <utility>

namespace te
{
	template<class To, class From>
	To safety_cast(From const& from) {
		if (!std::in_range<To>(from)) {
			tassert(0);
			std::abort();
		}

		return static_cast<To>(from);
	}

	template<class To, class From>
	To cast_or_default(From const& from, From const& default_) {
		if (!std::in_range<To>(from)) {
			return default_;
		}
		else {
			return static_cast<To>(from);
		}
	}

	template<class To, class From>
	To throw_cast(From const& from) {
		if (!std::in_range<To>(from)) {
			throw std::out_of_range(std::format("Value {} of type {} out of range of type {}", from, typeid(From).name(), typeid(To).name()));
		}

		return static_cast<To>(from);
	}

	template<class To, class From>
	void throw_assign(To& to, From const& from) {
		if (!std::in_range<To>(from)) {
			throw std::out_of_range(std::format("Value {} of type {} out of range of type {}", from, typeid(From).name(), typeid(To).name()));
		}

		to = static_cast<To>(from);
	}

	template<class To, class From>
	bool try_assign(To& to, From const& from) {
		if (!std::in_range<To>(from)) {
			return false;
		}

		to = static_cast<To>(from);

		return true;
	}
}
