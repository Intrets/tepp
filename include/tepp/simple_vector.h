// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include <algorithm>
#include <cassert>
#include <utility>

namespace te
{
	template<class T>
	struct simple_vector
	{
		T* data = nullptr;
		int64_t capacity = 0;
		int64_t size = 0;

		inline T& operator[](int64_t i) {
			assert(i >= 0);
			assert(i < this->size);

			return data[i];
		}

		auto begin() {
			return &data[0];
		}

		auto end() {
			return &data[this->size];
		}

		operator std::span<T>() {
			return std::span(this->begin(), this->end());
		}

		operator std::span<T const>() const {
			return std::span(this->begin(), this->end());
		}

		inline T const& front() const {
			assert(this->size > 0);
			return data[0];
		}

		inline T const& back() const {
			assert(this->size > 0);
			return data[this->size - 1];
		}

		inline T pop() {
			assert(this->size > 0);
			this->size--;
			auto test = std::move(this->data[this->size]);
			return test;
		}

	private:
		inline void ensure_capacity() {
			if (this->size == this->capacity) {
				this->capacity = std::max(int64_t(1), this->capacity * 2);
				auto newBuffer = new T[this->capacity];

				auto it = this->data;
				auto jt = newBuffer;
				for (int64_t i = 0; i < this->size; i++, it++, jt++) {
					*jt = std::move(*it);
				}
				delete[] this->data;

				this->data = newBuffer;
			}
		}

	public:
		inline void add_back(T const& v) {
			assert(this->size < this->capacity);

			data[this->size] = v;
			this->size++;
		}

		inline void add_back(T&& v) {
			assert(this->size < this->capacity);

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
			assert(this->data == nullptr);

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
		simple_vector(int64_t capacity_) {
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
	};
}
