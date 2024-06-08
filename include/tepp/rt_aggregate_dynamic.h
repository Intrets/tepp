#pragma once

#include "tepp/intrusive_list.h"
#include "tepp/rt_base.h"

namespace te
{
	struct rt_update
	{
		int64_t index{};
		std::unique_ptr<rt_base_operation> operation{};
	};

	struct rt_aggregate_dynamic
	{
		std::vector<rt_base*> elements{};
		using Updates = std::vector<rt_update>;
		Updates queue{};

		// non-rt
		int64_t inTransit = 0;
		std::atomic<intrusive_list<Updates>*> cleanup{};

		void handleCleanup(void* context);
		void sendQueue();
		void clear();

		// rt
		std::atomic<intrusive_list<Updates>*> updates{};

		bool processOperations();

		void addUpdate(int64_t index, std::unique_ptr<rt_base_operation> operation);
		void register_rt(rt_base& base);

		template<class T>
		void addOperation(int64_t index, T&& operation);
	};

	template<class T>
	inline void rt_aggregate_dynamic::addOperation(int64_t index, T&& operation) {
		this->addUpdate(index, std::make_unique<rt_type_erased_operation<T>>(std::move(operation)));
	}
}
