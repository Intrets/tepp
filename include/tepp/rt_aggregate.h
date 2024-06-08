// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include <atomic>
#include <cassert>
#include <list>
#include <memory>
#include <optional>
#include <tuple>
#include <vector>

#include "intrusive_list.h"
#include "misc.h"
#include "tepp.h"

namespace te
{
	template<class T>
	concept has_process_updates = requires(T t) {
		std::decay_t<T>::process_tag;
	};

	template<class Enum, class... Args>
	struct rt_aggregate
	{
		using Updates = std::tuple<std::vector<typename Args::Update>...>;

		// non-rt
		Updates queue;
		int64_t inQueueCount = 0;
		std::atomic<intrusive_list<Updates>*> cleanup;
		std::tuple<typename Args::nonrt...> nonrt_objects;

		template<Enum e>
		auto& nonrtAccess();

		[[nodiscard]] std::optional<intrusive_list_owned<Updates>> handleCleanup();
		void sendQueue();
		void clear();

		// rt
		std::tuple<typename Args::rt...> rt_objects;
		std::atomic<intrusive_list<Updates>*> updates;

		// returns true if there were updates, and always processes the updates if there are any
		bool processUpdates();

		template<Enum e>
		auto& rtAccess();

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
			using T = typename std::decay_t<decltype(*ptr)>::data_type;
			ptr->front().for_each_forward(
			    [&](T& updates) {
				    te::tuple_for_each(
				        [](auto&& e) {
					        auto&& [object, update] = e;
					        object.processUpdates(update);
				        },
				        te::tuple_zip(
				            te::tie_tuple_elements(this->nonrt_objects),
				            te::tie_tuple_elements(updates)
				        )
				    );

				    this->inQueueCount--;
			    }
			);
			return std::make_optional<intrusive_list_owned<typename rt_aggregate<Enum, Args...>::Updates>>(ptr);
		}
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

		[[maybe_unused]] auto old = this->updates.exchange(current);
		assert(old == nullptr);

		this->inQueueCount++;
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

		using T = typename std::decay_t<decltype(*maybeCleanup)>::data_type;
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
			        )
			    );
		    }
		);

		if (maybeCleanup == nullptr) {
			maybeCleanup = maybeUpdates;
		}
		else {
			maybeCleanup->back().insert_after(maybeUpdates);
		}

		[[maybe_unused]] auto oldCleanup = this->cleanup.exchange(maybeCleanup);
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
		    )
		);
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
	inline auto& rt_aggregate<Enum, Args...>::rtAccess() {
		constexpr auto index = static_cast<int>(e);
		static_assert(index >= 0);
		static_assert(index < sizeof...(Args));
		return std::get<index>(this->rt_objects);
	}
}
