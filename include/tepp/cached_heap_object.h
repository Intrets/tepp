// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include "misc.h"
#include "heap_object.h"

#include <optional>

namespace te
{
	template<class T>
	struct cached_heap_object
	{
		std::optional<heap_object<T>> object;

		std::optional<T*> tryGet() {
			if (this->has_value()) {
				return this->object.value().get();
			}
			else {
				return std::nullopt;
			}
		}

		bool has_value() {
			return this->object.has_value();
		};

		void reset() {
			this->object.reset();
		}

		template<class... Args>
		T& emplace(Args&&... args) {
			return this->object.emplace(std::forward<Args>(args)...);
		}
	};
}
