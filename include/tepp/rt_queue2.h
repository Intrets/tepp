#pragma once

#include "tepp/rt_base.h"

namespace te
{
	template<class T>
	struct rt_queue2_non_rt
	{
		T value{};
	};

	template<class T>
	struct rt_queue2_rt
	{
		T value{};
	};

	template<class T>
	struct rt_queue2 : rt_base_template<rt_queue2<T>, rt_queue2_non_rt<T>, rt_queue2_rt<T>>
	{
		using rt = rt_queue2_rt<T>;
		using non_rt = rt_queue2_non_rt<T>;

		struct Message
		{
			T value{};

			void run_rt(rt& rt) {
				value.run();
			}
		};

		void add(T&& value) {
			this->addOperation(Message{ .value = std::forward<T>(value) });
		}

		void clear() override {
		}
	};
}
