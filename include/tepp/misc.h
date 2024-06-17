// tepp - A template library for C++
// Copyright (C) 2022 intrets

#include <cstdint>

#include <tepp/integers.h>

#pragma once

#define DEFAULT_COPY(T) \
	T(const T&) = default; \
	T& operator=(const T&) = default;
#define NO_COPY(T) \
	T(const T&) = delete; \
	T& operator=(const T&) = delete;
#define DEFAULT_MOVE(T) \
	T(T&&) = default; \
	T& operator=(T&&) = default;
#define NO_MOVE(T) \
	T(T&&) = delete; \
	T& operator=(T&&) = delete;
#define DEFAULT_COPY_MOVE(T) DEFAULT_COPY(T) DEFAULT_MOVE(T)
#define NO_COPY_MOVE(T) NO_COPY(T) NO_MOVE(T)

namespace te
{
	inline integer_t isize(auto&& a) {
		return static_cast<integer_t>(a.size());
	}
}
