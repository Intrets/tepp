#pragma once

#include "tepp/assert.h"
#include "tepp/intrusive_list.h"
#include "tepp/tepp.h"

#include <concepts>
#include <cstdint>
#include <memory>

namespace te
{
	struct rt_base_operation;
	struct rt_base;

	struct rt_update
	{
		integer_t index{};
		std::unique_ptr<rt_base_operation> operation{};
	};

	struct rt_aggregate_dynamic
	{
		std::vector<rt_base*> elements{};
		using Updates = std::vector<rt_update>;
		Updates queue{};

		// non-rt
		integer_t inTransit = 0;
		std::atomic<intrusive_list<Updates>*> cleanup{};

		void handleCleanup(void* context);
		void sendQueue();
		void clear();

		// rt
		std::atomic<intrusive_list<Updates>*> updates{};

		bool processOperations();

		void addUpdate(integer_t index, std::unique_ptr<rt_base_operation> operation);
		void register_rt(rt_base& base);

		template<class T>
		void addOperation(integer_t index, T&& operation);
	};

	struct rt_base
	{
		rt_aggregate_dynamic* aggregate = nullptr;
		integer_t aggregateIndex = -1;

		void* rtPointer = nullptr;
		void* nonrtPointer = nullptr;

		rt_aggregate_dynamic& getAggregate();

		template<class T>
		void addOperation(T&& operation) {
			this->getAggregate().addOperation(this->aggregateIndex, std::forward<T>(operation));
		}

		virtual void clear() = 0;
	};

	template<class T, class non_rt_, class rt_>
	struct alignas(64) rt_base_template : rt_base
	{
		using rt = rt_;
		using non_rt = non_rt_;

		alignas(64) rt rtStorage{};
		alignas(64) non_rt non_rtStorage{};

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

	struct context_flag
	{
	};
	template<class T>
	struct context
	{
		T* t;
		T* operator->() {
			return this->t;
		}
		T& operator*() {
			return *this->t;
		}
		using type = T;
		using flag = context_flag;
	};
	template<class T>
	concept is_context = std::same_as<context_flag, typename T::flag>;

	struct self_flag
	{
	};
	template<class T>
	struct self
	{
		T* t;
		T* operator->() {
			return this->t;
		}
		T& operator*() {
			return *this->t;
		}
		using type = T;
		using flag = self_flag;
	};
	template<class T>
	concept is_self = std::same_as<self_flag, typename T::flag>;

	struct rt_base_operation
	{
		virtual void runrt_impl(rt_base& base) = 0;

		virtual void runnonrt_impl(void* context, rt_base& base) = 0;

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
	concept has_no_arg_run_rt = requires(T t) { { &T::run_rt } -> std::same_as<void(T::*)()>; };

	template<class T>
	struct rt_type_erased_operation : rt_base_operation
	{
		T operation{};

		void runrt_impl(rt_base& base) override {
			if constexpr (has_no_arg_run_rt<T>) {
				this->operation.run_rt();
			}
			else {
				using rt_type = detail::extract_first_arg_member_function<decltype(&T::run_rt)>::type;

				auto rt_pointer = std::launder(reinterpret_cast<rt_type*>(base.rtPointer));

				this->operation.run_rt(*rt_pointer);
			}
		}

		void runnonrt_impl(void* context, rt_base& base) override {
			if constexpr (has_run_non_rt<T>) {
				using arg_list = te::arguments_list_t<decltype(&T::run_non_rt)>;

				if constexpr (te::list_size_v<arg_list> == 1) {
					using nonrt_type = std::remove_cvref_t<te::list_element_t<0, arg_list>>;

					auto nonrt_pointer = std::launder(
					    reinterpret_cast<nonrt_type*>(base.nonrtPointer)
					);

					this->operation.run_non_rt(*nonrt_pointer);
				}
				else if constexpr (te::list_size_v<arg_list> == 2) {
					using arg_type = std::remove_cvref_t<te::list_element_t<0, arg_list>>;
					using nonrt_type = std::remove_cvref_t<te::list_element_t<1, arg_list>>;

					auto nonrt_pointer = std::launder(
					    reinterpret_cast<nonrt_type*>(base.nonrtPointer)
					);

					if constexpr (is_context<arg_type>) {
						using context_type = arg_type::type;

						auto context_pointer = reinterpret_cast<context_type*>(context);

						this->operation.run_non_rt(arg_type{ context_pointer }, *nonrt_pointer);
					}
					else if constexpr (is_self<arg_type>) {
						using self_type = arg_type::type;

						auto self_pointer = reinterpret_cast<self_type*>(&base);

						this->operation.run_non_rt(arg_type{ self_pointer }, *nonrt_pointer);
					}
					else {
						tassert(0);
					}
				}
			}
		}

		rt_type_erased_operation(T&& operation_)
		    : operation(std::move(operation_)) {};

		rt_type_erased_operation() = delete;

		virtual ~rt_type_erased_operation() = default;
	};

	template<class T>
	inline void rt_aggregate_dynamic::addOperation(integer_t index, T&& operation) {
		this->addUpdate(index, std::make_unique<rt_type_erased_operation<T>>(std::move(operation)));
	}
}
