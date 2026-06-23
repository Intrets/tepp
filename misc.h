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

#if defined(COMPILER_CLANG) || defined(COMPILER_CLANGCL)
#define HAS_ADDRESS_SANITIZER __has_feature(address_sanitizer)
#define HAS_THREAD_SANITIZER __has_feature(thread_sanitizer)
#define HAS_UNDEFINED_SANITIZER __has_feature(undefined_sanitizer)
#elifdef COMPILER_MSVC
#define HAS_ADDRESS_SANITIZER __SANITIZE_ADDRESS__
#define HAS_THREAD_SANITIZER __SANITIZER_THREAD__
#define HAS_UNDEFINED_SANITIZER 0
#else
#error unsupported compiler
#endif
