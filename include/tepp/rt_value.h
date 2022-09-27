// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include <atomic>
#include <vector>
#include <variant>

#include <tepp/tepp.h>

namespace te
{
	template<class T>
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

		using Update = std::variant<Retrieve, Send>;

		struct rt
		{
			T value{};

			void processUpdates(std::vector<Update>& updates) {
				for (auto& update : updates) {
					std::visit([this]<class S_>(S_ && update) {
						using S = std::remove_cvref_t<S_>;

						if constexpr (std::same_as<S, Send>) {
							this->value = update.storage;
						}
						else if constexpr (std::same_as<S, Retrieve>) {
							update.storage = this->value;
						}
					}, update);
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

			void send(T const& t) {
				this->addUpdate(Send{ t });
			}

			void retrieve() {
				this->addUpdate(Retrieve{});
			}

			using process_tag = void;
			void processUpdates(std::vector<Update>& updates) {
				for (auto& update : updates) {
					std::visit([this]<class S_>(S_ && update) {
						using S = std::remove_cvref_t<S_>;

						if constexpr (std::same_as<S, Retrieve>) {
							this->value = update.storage;
						}
					}, update);
				}
			}
		};
	};

	template<class T>
	inline void rt_value<T>::nonrt::clear() {
		this->addUpdate(Send());
	}
}
