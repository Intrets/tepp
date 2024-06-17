#pragma once

#include "tepp/rt_aggregate_dynamic.h"
#include "tepp/rt_base.h"
#include "tepp/simple_vector.h"

#include <algorithm>

namespace te
{
	struct rt_vector2_non_rt
	{
		integer_t capacity = 10;
		integer_t size = 0;
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
		};

		struct ReplaceStorage
		{
			simple_vector<T> storage;

			ReplaceStorage(integer_t size)
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
		};

		struct Swap
		{
			integer_t from;
			integer_t to;
			T storage{};

			void run_rt(rt& rt) {
				assert(this->from < rt.data.size);
				assert(this->to < rt.data.size);
				this->storage = std::move(rt.data[this->from]);
				rt.data[this->from] = std::move(rt.data[this->to]);
				rt.data[this->to] = std::move(this->storage);
			}
		};

		struct Pop
		{
			T storage{};

			void run_rt(rt& rt) {
				assert(rt.data.size > 0);
				this->storage = std::move(rt.data.pop());
			}
		};

		struct Clear
		{
			simple_vector<T> storage;

			Clear(integer_t size)
			    : storage(size) {
			}

			void run_rt(rt& rt) {
				std::swap(this->storage, rt.data);
			}
		};

		struct OverwriteClear
		{
			void run_rt(rt& rt) {
				rt.data.size = 0;
			}
		};

		struct Set
		{
			simple_vector<T> storage;

			Set(simple_vector<T>&& storage_)
			    : storage(std::move(storage_)) {
			}

			void run_rt(rt& rt) {
				std::swap(rt.data, this->storage);
			}
		};

		simple_vector<T>& get_rt_data() {
			return this->get_rt().data;
		}

		integer_t& get_non_rt_size() {
			return this->get_non_rt().size;
		}

		integer_t& get_non_rt_capacity() {
			return this->get_non_rt().capacity;
		}

		void add(T&& value) {
			auto& non_rt = this->get_non_rt();
			if (non_rt.size == non_rt.capacity) {
				non_rt.capacity *= 2;
				this->addOperation(ReplaceStorage(non_rt.capacity));
			}
			non_rt.size++;
			this->addOperation(Add{ .datum = std::forward<T>(value) });
		}

		void swap(integer_t from, integer_t to) {
			assert(from < this->get_non_rt_size());
			assert(to < this->get_non_rt_size());
			assert(from >= 0);
			assert(to >= 0);
			this->addOperation(Swap{ .from = from, .to = to });
		}

		void pop() {
			assert(this->get_non_rt_size() > 0);
			this->get_non_rt_size()--;
			this->addOperation(Pop{});
		}

		void set(simple_vector<T>&& v) {
			this->get_non_rt_size() = v.size;
			this->get_non_rt_capacity() = v.capacity;
			this->addOperation(Set(std::forward<simple_vector<T>>(v)));
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
