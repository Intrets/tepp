// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include <cassert>
#include <utility>
#include <algorithm>

namespace te
{
	template<class T>
	struct simple_vector
	{
		T* data = nullptr;
		size_t capacity = 0;
		size_t size = 0;

		inline T& operator[](size_t i) {
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
			return std::move(this->data[this->size]);
		}

		inline void push_back(T const& v) {
			assert(this->size < this->capacity);

			data[this->size] = v;
			this->size++;
		}

		inline void push_back(T&& v) {
			assert(this->size < this->capacity);

			data[this->size] = std::move(v);
			this->size++;
		}

		inline void fill_to_capacity(T const& v) {
			std::fill(this->data, this->data + this->capacity, v);
			this->size = this->capacity;
		}

		inline bool is_at_capacity() const {
			return this->size == this->capacity;
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
		simple_vector(size_t capacity_) {
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
