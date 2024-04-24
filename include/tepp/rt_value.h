// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include <atomic>
#include <vector>

#include "tepp.h"
#include "variant.h"

namespace te
{
	template<class T, class S>
	concept has_rt_value_run = requires(T v, S s) {
		s.run(v);
	};

	template<class T, class... Extended>
	struct rt_value
	{
		struct Retrieve
		{
			T storage{};
		};

		struct Send
		{
			T storage{};
		};

		struct Swap
		{
			T storage{};
			T temp{};
		};

		using Update = te::variant<Retrieve, Send, Swap, Extended...>;

		struct rt
		{
			T value{};

			void processUpdates(std::vector<Update>& updates) {
				for (auto& update : updates) {
					te::visit(update, [this]<class S_>(S_&& update) {
						using S = std::remove_cvref_t<S_>;

						if constexpr (std::same_as<S, Send>) {
							this->value = std::move(update.storage);
						}
						else if constexpr (std::same_as<S, Retrieve>) {
							update.storage = std::move(this->value);
						}
						else if constexpr (std::same_as<S, Swap>) {
							update.temp = std::move(this->value);
							this->value = std::move(update.storage);
							update.storage = std::move(update.temp);
						}
						else if constexpr (has_rt_value_run<T, S>) {
							update.run(this->value);
						}
					});
				}
			}
		};

		struct nonrt
		{
			T value{};

			size_t capacity = 10;
			size_t size = 0;

			std::vector<Update>* queue{};
			void clear();

			template<te::member_of<Update> S>
			void addUpdate(S&& s) {
				this->queue->push_back(std::forward<S>(s));
			}

			void send(T&& t) {
				this->addUpdate(Send{ std::forward<T>(t) });
			}

			void swap(T&& t) {
				this->addUpdate(Swap{ .storage = std::forward<T>(t) });
			}

			void send(T const& t) {
				this->addUpdate(Send{ t });
			}

			void retrieve() {
				this->addUpdate(Retrieve{});
			}

			using process_tag = void;
			void processUpdates(std::vector<Update>& updates) {
				for (auto& update : updates) {
					te::visit(update, [this]<class S_>(S_&& update) {
						using S = std::remove_cvref_t<S_>;

						if constexpr (std::same_as<S, Retrieve>) {
							this->value = std::move(update.storage);
						}
					});
				}
			}
		};
	};

	template<class T, class... Extended>
	inline void rt_value<T, Extended...>::nonrt::clear() {
		this->addUpdate(Swap{});
	}
}
