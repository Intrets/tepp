// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include <vector>

#include "tepp/tepp.h"
#include "tepp/variant.h"

namespace te
{
	template<class T>
	struct rt_move_value
	{
		struct Storage
		{
			T value{};
			bool ready{};
		};

		struct Swap
		{
			Storage storage{};
		};

		struct Clear
		{
			Storage storage{};
		};

		using Update = te::variant<Swap, Clear>;

		struct rt
		{
			Storage storage{};

			void send() {
				this->storage.ready = true;
			}

			T& getValue() {
				return this->storage.value;
			}

			T const& getValue() const {
				return this->storage.value;
			}

			void processUpdates(std::vector<Update>& updates) {
				for (auto& update : updates) {
					te::visit(update,
						[this](Swap& swap_) {
							if (this->storage.ready) {
								std::swap(this->storage, swap_.storage);
							}
						},
						[&](Clear& clear) {
							std::swap(this->storage, clear.storage);
						});
				}
			}
		};

		struct nonrt
		{
			T value{};

			std::vector<Update>* queue{};
			void clear() {
				this->queue->push_back(Clear{});
			};

			void swap(T&& t) {
				this->queue->push_back(Swap{ .storage = {.value = std::move(t) } });
			}

			using process_tag = void;
			void processUpdates(std::vector<Update>& updates) {
				for (auto& update : updates) {
					te::visit(update,
						[this](Swap& swap_) {
							if (swap_.storage.ready) {
								std::swap(this->value, swap_.storage.value);
							}
						},
						[&](Clear& clear) {
						});
				}
			}
		};
	};
}
