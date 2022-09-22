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

#include "intrusive_list.h"

namespace te
{
	template<class T>
	struct rt_queue
	{
		using Update = T;

		struct rt
		{
			void processUpdates(std::vector<Update>& updates) {
				for (auto& update : updates) {
					update.run();
				}
			};
		};

		struct nonrt
		{
			std::vector<Update>* queue{};

			void add(T&& v) {
				this->queue->push_back(std::move(v));
			}

			void clear() {
			};
		};
	};
}
