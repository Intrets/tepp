// tepp - A template library for C++
// Copyright (C) 2022  Intrets
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
		}

		optional_ref() = default;
		optional_ref(te::nullopt_t) {
		}
		~optional_ref() = default;
	};
}
