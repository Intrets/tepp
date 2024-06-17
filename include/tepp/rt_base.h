#pragma once

#include "tepp/intrusive_list.h"
#include "tepp/tepp.h"

#include <cassert>
#include <cstdint>
#include <memory>

#ifdef ANDROID
#define ALIGN_CORE_AUDIO_DATA
#else
#define ALIGN_CORE_AUDIO_DATA alignas(std::hardware_destructive_interference_size)
#endif

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

	template<class T, class non_rt, class rt>
	struct ALIGN_CORE_AUDIO_DATA rt_base_template : rt_base
	{
		ALIGN_CORE_AUDIO_DATA rt rtStorage{};
		ALIGN_CORE_AUDIO_DATA non_rt non_rtStorage{};

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
	struct rt_type_erased_operation : rt_base_operation
	{
		T operation{};

		void runrt_impl(rt_base& base) override {
			using rt_type = detail::extract_first_arg_member_function<decltype(&T::run_rt)>::type;

			auto rt_pointer = std::launder(reinterpret_cast<rt_type*>(base.rtPointer));

			this->operation.run_rt(*rt_pointer);
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
					using context_type = std::remove_cvref_t<te::list_element_t<0, arg_list>>;
					using nonrt_type = std::remove_cvref_t<te::list_element_t<1, arg_list>>;

					auto context_pointer = reinterpret_cast<context_type*>(context);
					auto nonrt_pointer = std::launder(
					    reinterpret_cast<nonrt_type*>(base.nonrtPointer)
					);

					this->operation.run_non_rt(*context_pointer, *nonrt_pointer);
				}
			}
		}

		rt_type_erased_operation(T&& operation_)
		    : operation(std::move(operation_)){};

		rt_type_erased_operation() = delete;

		virtual ~rt_type_erased_operation() = default;
	};

	template<class T>
	inline void rt_aggregate_dynamic::addOperation(integer_t index, T&& operation) {
		this->addUpdate(index, std::make_unique<rt_type_erased_operation<T>>(std::move(operation)));
	}
}
