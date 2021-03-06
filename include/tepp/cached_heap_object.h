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

#include "misc.h"
#include "heap_object.h"

#include <optional>

namespace te
{
	template<class T>
	struct cached_heap_object
	{
		std::optional<heap_object<T>> object;

		std::optional<T*> tryGet() {
			if (this->has_value()) {
				return this->object.value().get();
			}
			else {
				return std::nullopt;
			}
		}

		bool has_value() {
			return this->object.has_value();
		};

		void reset() {
			this->object.reset();
		}

		template<class... Args>
		T& emplace(Args&&... args) {
			return this->object.emplace(std::forward<Args>(args)...);
		}
	};
}
