// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include <vector>

#include "intrusive_list.h"

namespace te
{
	template<class T>
	struct rt_queue
	{
		using Update = T;

		struct rt
		{
			void processUpdates(std::vector<Update>& updates) {
				for (auto& update : updates) {
					update.run();
				}
			};
		};

		struct nonrt
		{
			std::vector<Update>* queue{};

			void add(T&& v) {
				this->queue->push_back(std::move(v));
			}

			void processUpdates(std::vector<Update>& updates) {
			}

			void clear() {
			};
		};
	};
}
