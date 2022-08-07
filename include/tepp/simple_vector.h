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
#include <utility>
#include <algorithm>

namespace te
{
	template<class T>
	struct simple_vector
	{
		T* data = nullptr;
		size_t capacity = 0;
		size_t size = 0;

		inline T& operator[](size_t i) {
			assert(i >= 0);
			assert(i < this->size);

			return data[i];
		}

		auto begin() {
			return &data[0];
		}

		auto end() {
			return &data[this->size];
		}

		inline T pop() {
			assert(this->size > 0);
			this->size--;
			return std::move(this->data[this->size]);
		}

		inline void push_back(T&& v) {
			assert(this->size < this->capacity);

			data[this->size] = std::move(v);
			this->size++;
		}

		inline void fillToCapacity(T const& v) {
			std::fill(this->data, this->data + this->capacity, v);
			this->size = this->capacity;
		}

		simple_vector(simple_vector&& other) noexcept {
			this->data = other.data;
			other.data = nullptr;

			this->capacity = other.capacity;
			other.capacity = 0;

			this->size = other.size;
			other.size = 0;
		}

		simple_vector(simple_vector const& other) = delete;

		simple_vector& operator=(simple_vector&& other) noexcept {
			assert(this->data == nullptr);

			this->data = other.data;
			other.data = nullptr;

			this->capacity = other.capacity;
			other.capacity = 0;

			this->size = other.size;
			other.size = 0;

			return *this;
		}

		simple_vector& operator=(simple_vector const& other) = delete;

		simple_vector() = default;
		simple_vector(size_t capacity_) {
			this->capacity = capacity_;
			this->data = new T[capacity];
		}

		~simple_vector() {
			if (this->data != nullptr) {
				delete[] this->data;
			}
		}
	};

}
