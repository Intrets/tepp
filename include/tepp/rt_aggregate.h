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

#include <tuple>
#include <vector>
#include <atomic>
#include <cassert>

#include <tepp/tepp.h>

namespace te
{
	template<class... Args>
	struct rt_aggregate
	{
		using Updates = std::tuple<std::vector<typename Args::Update>...>;


		// non-rt
		Updates queue;
		std::atomic<Updates*> cleanup;
		std::tuple<typename Args::nonrt...> nonrt_objects;

		template<size_t N>
		auto& modify();

		void clean();
		bool sendQueue();


		// rt
		std::tuple<typename Args::rt...> rt_objects;
		std::atomic<Updates*> updates;

		bool processUpdates();

		template<size_t N>
		auto& use();


		rt_aggregate();
	};

	template<class... Args>
	inline void rt_aggregate<Args...>::clean() {
		delete this->cleanup.exchange(nullptr);
	}

	template<class... Args>
	inline bool rt_aggregate<Args...>::sendQueue() {
		auto current = this->updates.load();

		if (current == nullptr) {
			auto payload = new Updates;

			*payload = std::move(this->queue);

			this->updates.store(payload);

			return true;
		}
		else {
			return false;
		}
	}

	template<class... Args>
	inline bool rt_aggregate<Args...>::processUpdates() {
		auto maybeCleanup = this->cleanup.load();

		// No space in the clean up channel,
		// wont have space to deposit heap allocated storage from a potential update
		if (maybeCleanup != nullptr) {
			return false;
		}

		auto maybeUpdates = this->updates.exchange(nullptr);

		if (maybeUpdates == nullptr) {
			return false;
		}

		te::tuple_for_each(
			[](auto&& e) {
				auto&& [object, updates] = e;
				object.processUpdates(updates);
			},
			te::tuple_zip(
				te::tie_tuple_elements(this->rt_objects),
				te::tie_tuple_elements(*maybeUpdates)
			));

		this->cleanup.store(maybeUpdates);

		return true;
	}

	template<class... Args>
	inline rt_aggregate<Args...>::rt_aggregate() {
		te::tuple_for_each(
			[](auto&& e) {
				[[maybe_unused]]
				auto&& [nonrt, updates] = e;
				nonrt.queue = &updates;
			},
			te::tuple_zip(
				te::tie_tuple_elements(this->nonrt_objects),
				te::tie_tuple_elements(this->queue)
			));
	}

	template<class... Args>
	template<size_t N>
	inline auto& rt_aggregate<Args...>::modify() {
		return std::get<N>(this->nonrt_objects);
	}

	template<class... Args>
	template<size_t N>
	inline auto& rt_aggregate<Args...>::use() {
		return std::get<N>(this->rt_objects);
	}
}
