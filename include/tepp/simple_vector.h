// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include <algorithm>
#include <span>
#include <utility>

#include "tepp/assert.h"
#include "tepp/integers.h"

namespace te
{
	template<class T>
	struct simple_vector
	{
		T* data = nullptr;
		integer_t capacity = 0;
		integer_t size = 0;

		inline T& operator[](integer_t i) {
			tassert(i >= 0);
			tassert(i < this->size);

			return data[i];
		}

		inline T const& operator[](integer_t i) const {
			tassert(i >= 0);
			tassert(i < this->size);

			return data[i];
		}

		bool empty() const {
			return this->size == 0;
		}

		auto begin() {
			return &data[0];
		}

		auto begin() const {
			return &data[0];
		}

		auto end() {
			return &data[this->size];
		}

		auto end() const {
			return &data[this->size];
		}

		operator std::span<T>() {
			return std::span(this->begin(), this->end());
		}

		operator std::span<T const>() const {
			return std::span(this->begin(), this->end());
		}

		inline T const& front() const {
			tassert(this->size > 0);
			return data[0];
		}

		inline T& front() {
			tassert(this->size > 0);
			return data[0];
		}

		inline T const& back() const {
			tassert(this->size > 0);
			return data[this->size - 1];
		}

		inline T& back() {
			tassert(this->size > 0);
			return data[this->size - 1];
		}

		inline T pop() {
			tassert(this->size > 0);
			this->size--;
			auto test = std::move(this->data[this->size]);
			return test;
		}

	private:
		inline void ensure_capacity() {
			if (this->size == this->capacity) {
				this->reserve(std::max(1_i, this->capacity * 2));
			}
		}

	public:
		inline void add_back(T const& v) {
			tassert(this->size < this->capacity);

			data[this->size] = v;
			this->size++;
		}

		inline void add_back(T&& v) {
			tassert(this->size < this->capacity);

			data[this->size] = std::move(v);
			this->size++;
		}

		inline void push_back(T const& v) {
			this->ensure_capacity();

			this->add_back(v);
		}

		inline void push_back(T&& v) {
			this->ensure_capacity();

			this->add_back(std::forward<T>(v));
		}

		inline bool try_push_back(T const& v) {
			if (this->size == this->capacity) {
				return false;
			}
			else {
				this->add_back(v);

				return true;
			}
		}

		inline bool try_push_back(T&& v) {
			if (this->size == this->capacity) {
				return false;
			}
			else {
				this->add_back(std::forward<T>(v));

				return true;
			}
		}

		inline void fill(T const& t) {
			std::fill(this->data, this->data + this->size, t);
		}

		inline void fill_to_capacity(T const& v) {
			std::fill(this->data, this->data + this->capacity, v);
			this->size = this->capacity;
		}

		inline bool is_at_capacity() const {
			return this->size == this->capacity;
		}

		inline bool valid_index(std::integral auto i) const {
			return std::cmp_greater_equal(i, 0) && std::cmp_less(i, this->size);
		}

		inline void clear() {
			this->size = 0;
		}

		inline void resize(integer_t size_) {
			if (size_ != this->size) {
				this->reserve(size_);
				this->size = size_;
			}
		}

		inline void reserve(integer_t size_) {
			if (size_ <= this->capacity) {
				tassert(0);
				return;
			}

			this->capacity = size_;

			auto newBuffer = new T[this->capacity];

			if constexpr (std::is_trivially_copyable_v<T[]>) {
				auto sourceSpan = std::span(this->data, this->size);
				auto destinationSpan = std::span(newBuffer, size_);

				std::memcpy(destinationSpan.data(), sourceSpan.data(), sourceSpan.size_bytes());
			}
			else {
				auto it = this->data;
				auto jt = newBuffer;
				for (integer_t i = 0; i < this->size; i++, it++, jt++) {
					*jt = std::move(*it);
				}
			}

			this->data = newBuffer;
		}

		inline simple_vector copy() const {
			simple_vector result{};
			result.reserve(this->size);
			for (auto const& e : *this) {
				result.push_back(e);
			}
			return result;
		}

		simple_vector(simple_vector&& other) noexcept {
			this->data = other.data;
			other.data = nullptr;

			this->capacity = other.capacity;
			other.capacity = 0;

			this->size = other.size;
			other.size = 0;
		}

		simple_vector(simple_vector const& other) = delete;

		simple_vector& operator=(simple_vector&& other) noexcept {
			tassert(this->data == nullptr);

			this->data = other.data;
			other.data = nullptr;

			this->capacity = other.capacity;
			other.capacity = 0;

			this->size = other.size;
			other.size = 0;

			return *this;
		}

		simple_vector& operator=(simple_vector const& other) = delete;

		simple_vector() = default;
		simple_vector(integer_t capacity_) {
			this->capacity = capacity_;
			this->data = new T[capacity];
		}

		void delete_buffer() {
			this->size = 0;
			this->capacity = 0;
			delete[] this->data;
			this->data = nullptr;
		}

		~simple_vector() {
			if (this->data != nullptr) {
				delete[] this->data;
			}
		}

		static simple_vector make(integer_t size) {
			simple_vector result{};
			result.resize(size);
			return result;
		}
	};
}
