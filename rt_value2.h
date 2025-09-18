#pragma once

#include "tepp/assert.h"
#include "tepp/rt_base.h"

namespace te
{
	template<class T>
	struct rt_value2_non_rt
	{
		T value{};
	};

	template<class T>
	struct rt_value2_rt
	{
		T value{};
	};

	template<class T>
	struct rt_value2 : rt_base_template<rt_value2<T>, rt_value2_non_rt<T>, rt_value2_rt<T>>
	{
		using rt = rt_value2_rt<T>;
		using non_rt = rt_value2_non_rt<T>;

		struct Swap
		{
			T storage{};
			T temp{};

			void run_rt(rt& rt) {
				this->temp = std::move(rt.value);
				rt.value = std::move(this->storage);
				this->storage = std::move(this->temp);
			}
		};

		struct Send
		{
			T storage{};

			void run_rt(rt& rt) {
				rt.value = std::move(storage);
			}
		};

		struct Copy
		{
			T storage{};

			void run_rt(rt& rt) {
				if constexpr (std::copyable<T>) {
					this->storage = rt.value;
				}
				else {
					tassert(0);
				}
			}

			void run_non_rt(rt& rt) {
				if constexpr (std::copyable<T>) {
					rt.value = this->storage;
				}
				else {
					tassert(0);
				}
			}
		};
		struct Retrieve
		{
			T storage{};

			void run_rt(rt& rt) {
				this->storage = std::move(rt.value);
			}

			void run_non_rt(rt& rt) {
				rt.value = std::move(this->storage);
			}
		};

		T& get_rt_value() {
			return this->get_rt().value;
		}

		T& get_non_rt_value() {
			return this->get_non_rt().value;
		}

		void swap(T&& value) {
			this->addOperation(Swap{ .storage = std::forward<T>(value) });
		}

		void clear() override {
			this->addOperation(Swap{});
		}

		void send(T&& value) {
			this->addOperation(Send{ .storage = std::forward<T>(value) });
		}

		void send(T const& value) {
			this->addOperation(Send{ .storage = value });
		}

		void copy() {
			this->addOperation(Copy{});
		}

		void retrieve() {
			this->addOperation(Retrieve{});
		}
	};
}
