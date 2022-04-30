// tepp - A template library for C++
// Copyright (C) 2022  Intrets
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <vector>
#include <variant>
#include <atomic>
#include <cassert>

#include "tepp.h"

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

		inline T pop() {
			assert(this->size > 0);
			this->size--;
			return std::move(this->data[this->size]);
		}

		inline void push_back(T&& v) {
			assert(this->size < this->capacity);

			data[this->size] = std::move(v);
			this->size++;
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

		simple_vector& operator=(simple_vector&& other) {
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

		~simple_vector() {
			if (this->data != nullptr) {
				delete[] this->data;
			}
		}
	};

	template<class T, class S>
	concept has_rt_vector_run = requires (te::simple_vector<T> v, S s) {
		s.run(v);
	};

	enum class rt_vector_tag
	{
		swap,
		add,
		pop,
		replaceStorage,
		clear,
		copy
	};

	template<class T, rt_vector_tag tag>
	concept has_rt_vector_tag = requires (T t) {
		T::tag;
	}&& T::tag == tag;

	template<class T, class... Extended>
	struct rt_vector
	{
		struct Swap
		{
			static constexpr auto tag = rt_vector_tag::swap;

			size_t from{};
			size_t to{};

			T storage{};
		};

		struct Add
		{
			static constexpr auto tag = rt_vector_tag::add;

			T datum{};
		};

		struct Pop
		{
			static constexpr auto tag = rt_vector_tag::pop;

			T storage{};
		};

		struct ReplaceStorage
		{
			static constexpr auto tag = rt_vector_tag::replaceStorage;

			simple_vector<T> storage;

			ReplaceStorage(size_t size) : storage(size) {
			}
		};

		struct Clear
		{
			static constexpr auto tag = rt_vector_tag::clear;

			simple_vector<T> storage;

			Clear(size_t size) : storage(size) {
			}
		};

		struct Copy
		{
			static constexpr auto tag = rt_vector_tag::copy;

			simple_vector<T> storage;

			Copy(size_t size) : storage(size) {
			}
		};

		using Update = std::variant<Swap, Add, Pop, ReplaceStorage, Clear, Copy, Extended...>;

		struct nonrt
		{
			size_t capacity = 10;
			size_t size = 0;

			std::vector<Update>* queue{};
			void add(T&& value);
			void swap(size_t from, size_t to);
			void pop();
			void clear();
			void getCopy();

			template<te::member_of<Update> S>
			void addUpdate(S&& s) {
				this->queue->push_back(std::forward<S>(s));
			}
		};


		struct rt
		{
			simple_vector<T> data{ 10 };

			auto begin() {
				return this->data.begin();
			}

			auto end() {
				return this->data.begin() + this->data.size;
			}

			void processUpdates(std::vector<Update>& updates) {
				for (auto& update : updates) {
					std::visit([this]<class S_>(S_ && update) {
						using S = std::remove_cvref_t<S_>;

						if constexpr (std::same_as<S, Add>) {
							this->data.push_back(std::move(update.datum));
						}
						else if constexpr (std::same_as<S, Swap>) {
							assert(update.to < this->data.size);
							assert(update.from < this->data.size);
							update.storage = std::move(this->data[update.to]);
							this->data[update.to] = std::move(this->data[update.from]);
							this->data[update.from] = std::move(update.storage);
						}
						else if constexpr (std::same_as<S, Pop>) {
							update.storage = std::move(this->data.pop());
						}
						else if constexpr (std::same_as<S, ReplaceStorage>) {
							assert(this->data.size <= update.storage.capacity);
							update.storage.size = this->data.size;

							if constexpr (std::copyable<T>) {
								std::copy_n(this->data.begin(), this->data.size, update.storage.begin());
							}
							else {
								std::copy_n(std::move_iterator(this->data.begin()), this->data.size, update.storage.begin());
							}

							std::swap(this->data, update.storage);
						}
						else if constexpr (std::same_as<S, Clear>) {
							std::swap(this->data, update.storage);
						}
						else if constexpr (std::same_as<S, Copy> && std::copyable<T>) {
							assert(this->data.size <= update.storage.capacity);
							std::copy_n(this->data.begin(), this->data.size, update.storage.begin());
							update.storage.size = this->data.size;
						}
						else if constexpr (has_rt_vector_run<T, S>) {
							update.run(this->data);
						}
						else {
							rand();
						}
					}, update);
				}
			}
		};
	};

	template<class T, class... Extended>
	inline void rt_vector<T, Extended...>::nonrt::clear() {
		Clear c{ this->capacity };
		this->size = 0;
		this->queue->push_back(std::move(c));
	}

	template<class T, class... Extended>
	inline void rt_vector<T, Extended...>::nonrt::getCopy() {
		static_assert(std::is_trivially_copyable_v<T>);
		Copy c{ this->size };
		this->queue->push_back(std::move(c));
	}

	template<class T, class... Extended>
	inline void rt_vector<T, Extended...>::nonrt::add(T&& v) {
		if (this->size == this->capacity) {
			this->capacity *= 2;

			ReplaceStorage replacement(this->capacity);

			this->queue->push_back(std::move(replacement));
		}

		this->size++;
		this->queue->push_back(Add{ std::move(v) });
	}

	template<class T, class... Extended>
	inline void rt_vector<T, Extended...>::nonrt::swap(size_t from, size_t to) {
		this->queue->push_back(Swap{ .from = from, .to = to });
	}

	template<class T, class... Extended>
	inline void rt_vector<T, Extended...>::nonrt::pop() {
		this->size--;
		this->queue->push_back(Pop{});
	}
}
