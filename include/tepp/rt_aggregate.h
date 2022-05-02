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
	template<class T>
	concept has_process_updates = requires (T t) {
		std::decay_t<T>::process_tag;
	};

	template<class Enum, class... Args>
	struct rt_aggregate
	{
		using Updates = std::tuple<std::vector<typename Args::Update>...>;


		// non-rt
		Updates queue;
		std::atomic<intrusive_list<Updates>*> cleanup;
		std::tuple<typename Args::nonrt...> nonrt_objects;

		template<Enum e>
		auto& nonrtAccess();

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

		template<Enum e>
		auto& use();


		rt_aggregate();
		~rt_aggregate() = default;

		NO_COPY_MOVE(rt_aggregate);
	};

	template<class Enum, class... Args>
	inline std::optional<intrusive_list_owned<typename rt_aggregate<Enum, Args...>::Updates>> rt_aggregate<Enum, Args...>::handleCleanup() {
		auto ptr = this->cleanup.exchange(nullptr);
		if (ptr == nullptr) {
			return std::nullopt;
		}
		else {
			using T = std::decay_t<decltype(*ptr)>::data_type;
			ptr->front().for_each_forward(
				[&](T& updates) {
					te::tuple_for_each(
						[](auto&& e) {
							auto&& [object, update] = e;
							if constexpr (has_process_updates<decltype(object)>) {
								object.processUpdates(update);
							}
						},
						te::tuple_zip(
							te::tie_tuple_elements(this->nonrt_objects),
							te::tie_tuple_elements(updates)
						));
				}
			);
			return std::make_optional<intrusive_list_owned<typename rt_aggregate<Enum, Args...>::Updates>>(ptr);
		}
	}

	template<class Enum, class... Args>
	inline void rt_aggregate<Enum, Args...>::clean() {
		delete this->cleanup.exchange(nullptr);
	}

	template<class Enum, class... Args>
	inline void rt_aggregate<Enum, Args...>::sendQueue() {
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

	template<class Enum, class... Args>
	inline void rt_aggregate<Enum, Args...>::clear() {
		te::tuple_for_each([](auto& t) { t.clear(); }, this->nonrt_objects);
		this->sendQueue();
	}

	template<class Enum, class... Args>
	inline bool rt_aggregate<Enum, Args...>::processUpdates() {
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

	template<class Enum, class... Args>
	inline rt_aggregate<Enum, Args...>::rt_aggregate() {
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

	template<class Enum, class... Args>
	template<Enum e>
	inline auto& rt_aggregate<Enum, Args...>::nonrtAccess() {
		constexpr auto index = static_cast<int>(e);
		static_assert(index >= 0);
		static_assert(index < sizeof...(Args));
		return std::get<index>(this->nonrt_objects);
	}

	template<class Enum, class... Args>
	template<Enum e>
	inline auto& rt_aggregate<Enum, Args...>::use() {
		constexpr auto index = static_cast<int>(e);
		static_assert(index >= 0);
		static_assert(index < sizeof...(Args));
		return std::get<index>(this->rt_objects);
	}
}
