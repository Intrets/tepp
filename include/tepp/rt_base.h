#pragma once

#include <tepp/polyant.h>

namespace te
{
	struct rt_aggregate_dynamic;

	struct alignas(std::hardware_destructive_interference_size) rt_base
	{
		rt_aggregate_dynamic* aggregate = nullptr;
		int64_t aggregateIndex = -1;

		void* rtPointer = nullptr;
		void* nonrtPointer = nullptr;

		rt_aggregate_dynamic& getAggregate();

		template<class T>
		void addOperation(T&& operation) {
			this->getAggregate().addOperation(this->aggregateIndex, std::forward<T>(operation));
		}

		virtual void clear() = 0;
	};

	template<class T, class non_rt, class rt>
	struct rt_base_template : rt_base
	{
		rt rtStorage{};
		non_rt non_rtStorage{};

		rt& get_rt() {
			return this->rtStorage;
		}

		non_rt& get_non_rt() {
			return this->non_rtStorage;
		}

		rt_base_template() {
			this->rtPointer = &this->rtStorage;
			this->nonrtPointer = &this->non_rtStorage;
		}
		~rt_base_template() = default;
	};

	struct rt_base_operation
	{
		virtual void runrt_impl(rt_base& base) = 0;
		virtual void runnonrt_impl(rt_base& base) = 0;

		rt_base_operation() = default;
		virtual ~rt_base_operation() = default;
	};

	namespace detail
	{
		template<class F>
		struct extract_first_arg_member_function;

		template<class B, class Arg>
		struct extract_first_arg_member_function<void (B::*)(Arg)>
		{
			using type = std::remove_cvref_t<Arg>;
		};
	}

	template<class T>
	concept has_run_non_rt = requires(T t) { &T::run_non_rt; };

	template<class T>
	struct rt_base_operation_template : rt_base_operation
	{
		void runrt_impl(rt_base& base) override {
			using rt_type = detail::extract_first_arg_member_function<decltype(&T::run_rt)>::type;

			auto rt_pointer = std::launder(reinterpret_cast<rt_type*>(base.rtPointer));

			reinterpret_cast<T*>(this)->run_rt(*rt_pointer);
		}

		void runnonrt_impl(rt_base& base) override {
			if constexpr (has_run_non_rt<T>) {
				using nonrt_type = detail::extract_first_arg_member_function<decltype(&T::run_non_rt)>::type;

				auto nonrt_pointer = std::launder(reinterpret_cast<nonrt_type*>(base.nonrtPointer));

				reinterpret_cast<T*>(this)->run_non_rt(*nonrt_pointer);
			}
		}

		rt_base_operation_template() = default;
		virtual ~rt_base_operation_template() = default;
	};

	template<class T>
	struct rt_type_erased_operation : rt_base_operation_template<T>
	{
		T operation{};

		void runrt_impl(rt_base& base) override {
			using rt_type = detail::extract_first_arg_member_function<decltype(&T::run_rt)>::type;

			auto rt_pointer = std::launder(reinterpret_cast<rt_type*>(base.rtPointer));

			this->operation.run_rt(*rt_pointer);
		}

		void runnonrt_impl(rt_base& base) override {
			if constexpr (has_run_non_rt<T>) {
				using nonrt_type = detail::extract_first_arg_member_function<decltype(&T::run_non_rt)>::type;

				auto nonrt_pointer = std::launder(reinterpret_cast<nonrt_type*>(base.nonrtPointer));

				this->operation.run_non_rt(*nonrt_pointer);
			}
		}

		rt_type_erased_operation(T&& operation_)
		    : operation(std::move(operation_)){};
		rt_type_erased_operation() = delete;
		virtual ~rt_type_erased_operation() = default;
	};
}
