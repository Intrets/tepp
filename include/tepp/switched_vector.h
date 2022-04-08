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

#include <vector>

namespace te
{
	template<class T>
	struct switched_vector
	{
	private:
		std::vector<T> data{};

		size_t index = 0;

	public:
		switched_vector() = delete;
		switched_vector(std::vector<T>&& es) {
			this->data = std::move(es);
		}
		switched_vector(T&& e) {
			this->add(std::forward<T>(e));
		}
		~switched_vector() = default;

		switched_vector& operator=(std::vector<T>&& es) {
			this->data = std::move(es);
			return *this;
		}

		T& get() {
			return this->data[this->index];
		}

		T const& get() const {
			return this->data[this->index];
		}

		void add(T&& e) {
			this->data.push_back(std::forward<T>(e));
		}

		void selectLast() {
			assert(this->data.size() > 0);
			this->index = this->data.size() - 1;
		}
	};
}
