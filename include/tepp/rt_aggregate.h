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
#include <optional>
#include <list>
#include <memory>

#include "intrusive_list.h"
#include "tepp.h"
#include "misc.h"

namespace te
{
	template<class... Args>
	struct rt_aggregate
	{
		using Updates = std::tuple<std::vector<typename Args::Update>...>;


		// non-rt
		Updates queue;
		std::atomic<intrusive_list<Updates>*> cleanup;
		std::tuple<typename Args::nonrt...> nonrt_objects;

		template<size_t N>
		auto& modify();

		[[nodiscard]]
		std::optional<intrusive_list_owned<Updates>> handleCleanup();
		void clean();
		void sendQueue();
		void clear();


		// rt
		std::tuple<typename Args::rt...> rt_objects;
		std::atomic<intrusive_list<Updates>*> updates;

		// returns true if there were updates, and always processes the updates if there are any
		bool processUpdates();

		template<size_t N>
		auto& use();


		rt_aggregate();
		~rt_aggregate() = default;

		NO_COPY_MOVE(rt_aggregate);
	};

	template<class... Args>
	inline std::optional<intrusive_list_owned<typename rt_aggregate<Args...>::Updates>> rt_aggregate<Args...>::handleCleanup() {
		auto ptr = this->cleanup.exchange(nullptr);
		if (ptr == nullptr) {
			return std::nullopt;
		}
		else {
			return std::make_optional<intrusive_list_owned<typename rt_aggregate<Args...>::Updates>>(ptr);
		}
	}

	template<class... Args>
	inline void rt_aggregate<Args...>::clean() {
		delete this->cleanup.exchange(nullptr);
	}

	template<class... Args>
	inline void rt_aggregate<Args...>::sendQueue() {
		auto current = this->updates.exchange(nullptr);

		if (current == nullptr) {
			current = new te::intrusive_list<Updates>(std::move(this->queue));
		}
		else {
			current = current->back().insert_after(std::move(this->queue));
		}

		[[maybe_unused]]
		auto old = this->updates.exchange(current);
		assert(old == nullptr);
	}

	template<class... Args>
	inline void rt_aggregate<Args...>::clear() {
		te::tuple_for_each([](auto& t) { t.clear(); }, this->nonrt_objects);
		this->sendQueue();
	}

	template<class... Args>
	inline bool rt_aggregate<Args...>::processUpdates() {
		auto maybeUpdates = this->updates.exchange(nullptr);

		if (maybeUpdates == nullptr) {
			return false;
		}

		auto maybeCleanup = this->cleanup.exchange(nullptr);

		using T = std::decay_t<decltype(*maybeCleanup)>::data_type;
		maybeUpdates->front().for_each_forward(
			[&](T& updates) {
				te::tuple_for_each(
					[](auto&& e) {
						auto&& [object, updates] = e;
						object.processUpdates(updates);
					},
					te::tuple_zip(
						te::tie_tuple_elements(this->rt_objects),
						te::tie_tuple_elements(updates)
					));
			}
		);

		if (maybeCleanup == nullptr) {
			maybeCleanup = maybeUpdates;
		}
		else {
			maybeCleanup->back().insert_after(maybeUpdates);
		}

		[[maybe_unused]]
		auto oldCleanup = this->cleanup.exchange(maybeCleanup);
		assert(oldCleanup == nullptr);

		return true;
	}

	template<class... Args>
	inline rt_aggregate<Args...>::rt_aggregate() {
		static_assert(this->cleanup.is_always_lock_free);
		static_assert(this->updates.is_always_lock_free);

		te::tuple_for_each(
			[](auto&& e) {
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
