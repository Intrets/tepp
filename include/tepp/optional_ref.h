// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include <cassert>

namespace te
{
	struct nullopt_t
	{
	};
	inline constexpr nullopt_t nullopt{};

	template<class T>
	struct optional_ref
	{
		T* ptr = nullptr;

		T& value() const {
			assert(ptr != nullptr);
			return *ptr;
		}

		T& operator*() const {
			return this->value();
		}

		T* operator->() const {
			return &this->value();
		}

		bool has_value() const {
			return ptr != nullptr;
		}

		bool has_no_value() const {
			return ptr == nullptr;
		}

		void reset() {
			this->ptr = nullptr;
		}

		operator bool() const {
			return this->has_value();
		}

		optional_ref& operator=(T& object) {
			this->ptr = &object;
			return *this;
		}

		optional_ref(T& object) {
			this->ptr = &object;
		}

		optional_ref& operator=(te::nullopt_t) {
			this->ptr = nullptr;

			return *this;
		}

		optional_ref() = default;
		optional_ref(te::nullopt_t) {
		}
		~optional_ref() = default;
	};
}
