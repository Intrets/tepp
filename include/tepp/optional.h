#pragma once

#include <optional>

#include "tepp/nullopt.h"

#define UNWRAP_OPTIONAL(Y, Z) \
	if (auto&& Y##optional123456789 = Z) \
		for (auto& Y : te::detail::iterator_wrapper(Y##optional123456789))

#define UNWRAP_OPTIONAL2(Y, Z) \
	if (auto&& Y##optional123456789 = Z) { \
		auto& Y = *Y##optional123456789;

#define END_UNWRAP }

namespace te
{
	namespace detail
	{
		template<class T>
		struct iterator_wrapper
		{
			using value_type = typename T::value_type;

			T& data;

			value_type const* begin() const {
				if (this->data.has_value()) {
					return &this->data.value();
				}
				else {
					return nullptr;
				}
			}

			value_type const* end() const {
				if (this->data.has_value()) {
					return &this->data.value() + 1;
				}
				else {
					return nullptr;
				}
			}

			value_type* begin() {
				if (this->data.has_value()) {
					return &this->data.value();
				}
				else {
					return nullptr;
				}
			}

			value_type* end() {
				if (this->data.has_value()) {
					return &this->data.value() + 1;
				}
				else {
					return nullptr;
				}
			}
		};

		template<class T>
		concept is_std_optional = requires(T t) {
			typename T::value_type;
			std::same_as<std::optional<typename T::value_type>, T>;
		};

		template<class T_>
		decltype(auto) wrap(T_& t) {
			using T = std::remove_cvref_t<T_>;

			if constexpr (is_std_optional<T>) {
				return iterator_wrapper<typename T::value_type>{ .data = t };
			}
			else {
				return t;
			}
		}
	}

	template<class T>
	struct optional
	{
		using value_type = T;

		std::optional<T> data{};

		T& operator*() {
			return *this->data;
		}

		T const& operator*() const {
			return *this->data;
		}

		T& value() {
			return this->data.value();
		}

		T const& value() const {
			return this->data.value();
		}

		operator bool() const {
			return this->has_value();
		}

		bool has_value() const {
			return this->data.has_value();
		}

		T* begin() {
			if (data.has_value()) {
				return &data.value();
			}
			else {
				return nullptr;
			}
		}

		T* end() {
			if (data.has_value()) {
				return &data.value() + 1;
			}
			else {
				return nullptr;
			}
		}

		T const* begin() const {
			if (data.has_value()) {
				return &data.value();
			}
			else {
				return nullptr;
			}
		}

		T const* end() const {
			if (data.has_value()) {
				return &data.value() + 1;
			}
			else {
				return nullptr;
			}
		}

		optional(optional const& other)
		    : data(other.data) {
		}

		optional& operator=(optional const& other) {
			this->data = other.data;
			return *this;
		}

		optional(optional&& other)
		    : data(std::move(other.data)) {
		}

		optional& operator=(optional&& other) {
			this->data = std::move(other.data);
			return *this;
		}

		optional(nullopt_t)
		    : data(std::nullopt) {
		}

		template<class... Args>
		optional(Args&&... args)
		    : data(std::forward<Args>(args)...) {
		}
	};

	template<class T, class F>
	inline bool optional_equal_with(std::optional<T> const& left, std::optional<T> const& right, F&& cmp) {
		if (left.has_value() != right.has_value()) {
			return false;
		}
		if (!left.has_value()) {
			return true;
		}
		return cmp(left.value(), right.value());
	}
}
