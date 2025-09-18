#pragma once

#include "tepp/rt_base.h"

namespace te
{
	template<class T>
	struct rt_move_value2_storage
	{
		T value{};
		bool ready{};
	};

	template<class T>
	struct rt_move_value2_non_rt
	{
		T value{};
	};

	template<class T>
	struct rt_move_value2_rt
	{
		rt_move_value2_storage<T> storage{};
	};

	template<class T>
	struct rt_move_value2 : rt_base_template<rt_move_value2<T>, rt_move_value2_non_rt<T>, rt_move_value2_rt<T>>
	{
		using rt = rt_move_value2_rt<T>;
		using non_rt = rt_move_value2_non_rt<T>;

		struct Clear
		{
			rt_move_value2_storage<T> storage;

			void run_rt(rt& rt) {
				std::swap(this->storage, rt.storage);
			}
		};

		struct Swap
		{
			rt_move_value2_storage<T> storage{};

			void run_rt(rt& rt) {
				if (rt.storage.ready) {
					std::swap(this->storage, rt.storage);
				}
			}

			void run_non_rt(non_rt& non_rt) {
				if (this->storage.ready) {
					std::swap(this->storage.value, non_rt.value);
				}
			}
		};

		void send_rt() {
			this->get_rt().storage.ready = true;
		}

		void swap(T&& t) {
			this->addOperation(Swap{ .storage = { .value = std::move(t) } });
		}

		void clear() override {
			this->addOperation(Clear{});
		}
	};
}