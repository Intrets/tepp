// tepp - a template library for c++
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

#include <utility>

namespace te
{
	template<class T>
	struct heap_object
	{
	private:
		T* object;

	public:
		T* get() {
			return this->object;
		}

		T* operator->() {
			return this->get();
		}

		operator T& () {
			return *this->object;
		}

		T* copy() const {
			return new T(this->object);
		}

		heap_object() : object(new T) {
		}

		template<class... Args>
		heap_object(Args&&... args) : object(new T(std::forward<Args>(args)...)) {
		}


		heap_object(heap_object const& other) {
			this->object = other.copy();
		}

		heap_object(heap_object&& other) {
			this->object = new T(std::move(*other.object));
		}


		heap_object& operator=(heap_object const& other) {
			*this->object = *other.object;
		}

		heap_object& operator=(heap_object&& other) {
			*this->object = std::move(*other.object);
		}


		~heap_object() {
			delete this->object;
		}
	};
}
