#pragma once

#include "tepp/polyant.h"
#include "tepp/rt_aggregate_dynamic.h"
#include "tepp/rt_base.h"
#include "tepp/simple_vector.h"

#include <algorithm>

namespace te
{
	struct rt_vector2_non_rt
	{
		int64_t capacity = 10;
		int64_t size = 0;
	};

	template<class T>
	struct rt_vector2_rt
	{
		simple_vector<T> data{ 10 };
	};

	template<class T>
	struct rt_vector2 : rt_base_template<rt_vector2<T>, rt_vector2_non_rt, rt_vector2_rt<T>>
	{
		using rt = rt_vector2_rt<T>;
		using non_rt = rt_vector2_non_rt;

		struct Add
		{
			T datum{};

			void run_rt(rt& rt) {
				rt.data.add_back(std::move(this->datum));
			}
			void run_non_rt(non_rt& nonrt) {
			}
		};

		struct ReplaceStorage
		{
			simple_vector<T> storage;

			ReplaceStorage(int64_t size)
			    : storage(size) {
			}

			void run_rt(rt& rt) {
				assert(this->storage.capacity >= rt.data.capacity);
				this->storage.size = rt.data.size;

				if constexpr (std::copyable<T>) {
					std::copy_n(rt.data.begin(), rt.data.size, this->storage.begin());
				}
				else {
					std::copy_n(std::move_iterator(rt.data.begin()), rt.data.size, this->storage.begin());
				}

				std::swap(rt.data, this->storage);
			}
			void run_non_rt(non_rt& nonrt) {
			}
		};

		struct Swap
		{
			int64_t from;
			int64_t to;
			T storage;

			void run_rt(rt& rt) {
				assert(this->from < rt.data.size);
				assert(this->to < rt.data.size);
				this->storage = std::move(rt.data[this->from]);
				rt.data[this->from] = std::move(rt.data[this->to]);
				rt.data[this->to] = std::move(this->storage);
			}
			void run_non_rt(non_rt& nonrt) {
			}
		};

		struct Pop
		{
			T storage{};

			void run_rt(rt& rt) {
				assert(rt.data.size > 0);
				this->storage = std::move(rt.data[rt.data.size - 1]);
			}
			void run_non_rt(non_rt& nonrt) {
			}
		};

		struct Clear
		{
			simple_vector<T> storage;

			Clear(int64_t size)
			    : storage(size) {
			}

			void run_rt(rt& rt) {
				std::swap(this->storage, rt.data);
			}
			void run_non_rt(non_rt& nonrt) {
			}
		};

		struct OverwriteClear
		{
			void run_rt(rt& rt) {
				rt.data.size = 0;
			}
			void run_non_rt(non_rt& rt) {
			}
		};

		void add(T&& value) {
			auto& non_rt = this->get_non_rt();
			if (non_rt.size == non_rt.capacity) {
				non_rt.capacity *= 2;
				this->addOperation(ReplaceStorage(non_rt.capacity));
			}
			non_rt.size++;
			this->addOperation(Add{ .datum = std::forward<T>(value) });
		}

		void swap(int64_t from, int64_t to) {
			assert(from < this->get_non_rt().size);
			assert(to < this->get_non_rt().size);
			assert(from >= 0);
			assert(to >= 0);
			this->addOperation(Swap{ .from = from, .to = to });
		}

		void pop() {
			assert(this->get_non_rt().size > 0);
			this->addOperation(Pop{});
		}

		void clear() {
			this->addOperation(Clear(this->get_non_rt().capacity));
			this->get_non_rt().size = 0;
		}

		void overwriteClear() {
			this->addOperation(OverwriteClear{});
		}
	};
}
