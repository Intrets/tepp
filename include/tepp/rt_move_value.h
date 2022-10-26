// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include <vector>

#include "tepp.h"
#include "variant.h"

namespace te
{
	template<class T>
	struct rt_move_value
	{
		struct Swap
		{
			T storage{};
		};

		using Update = Swap;

		struct rt
		{
			T value{};

			void processUpdates(std::vector<Update>& updates) {
				for (auto& update : updates) {
					std::swap(this->value, update.storage);
				}
			}
		};

		struct nonrt
		{
			T value{};

			std::vector<Update>* queue{};
			void clear();

			void swap(T&& t) {
				this->queue->push_back(Swap{ .storage = std::move(t) });
			}

			using process_tag = void;
			void processUpdates(std::vector<Update>& updates) {
				for (auto& update : updates) {
					std::swap(this->value, update.storage);
				}
			}
		};
	};

	template<class T>
	inline void rt_move_value<T>::nonrt::clear() {
		this->swap({});
	}
}
