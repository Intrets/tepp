// tepp - A template library for C++
// Copyright (C) 2021 intrets

#pragma once

#include <atomic>
#include <cassert>
#include <vector>

#include "misc.h"

namespace te
{
	namespace rt_export_size
	{
		struct normal
		{
			size_t size;
		};

		struct power_of_two
		{
			size_t power;

			size_t getSize() {
				assert(power < 64);
				return 1ULL << power;
			}

			static constexpr power_of_two at_least(int64_t size) {
				if (size <= 0) {
					return { 0 };
				}

				size_t power = 0;

				size--;

				while (size) {
					size >>= 1;
					power++;
				}

				return { power };
			}
		};

		enum class type
		{
			normal,
			restricted
		};
	}

	template<class T, rt_export_size::type type = rt_export_size::type::normal>
	struct rt_export
	{
		struct const_iterator
		{
			using iterator_category = std::random_access_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T const&;

		private:
			int64_t index;
			rt_export& parent;

		public:
			const_iterator(rt_export& parent_, int64_t index_) noexcept
			    : index(index_),
			      parent(parent_) {
			}

			const_iterator& operator++() noexcept {
				this->index++;
				return *this;
			}

			const_iterator operator++(int) noexcept {
				auto r = *this;
				this->index++;
				return r;
			}

			difference_type operator-(const_iterator const& other) const noexcept {
				return this->index - other.index;
			}

			const_iterator& operator--() noexcept {
				this->index--;
				return *this;
			}

			const_iterator& operator+=(int inc) noexcept {
				this->index += inc;
				return *this;
			}

			const_iterator operator--(int) noexcept {
				auto r = *this;
				this->index--;
				return r;
			}

			bool operator==(const_iterator const& other) const noexcept {
				return this->index == other.index && &this->parent == &other.parent;
			}

			bool operator!=(const_iterator const& other) const noexcept {
				return !(*this == other);
			}

			reference operator*() const noexcept {
				auto i = this->parent.mod(this->index);
				return this->parent.data[i];
			}

			const_iterator& operator=(const_iterator const& other) {
				assert(&this->parent == &other.parent);

				this->index = other.index;
				return *this;
			}
		};

		struct rt_export_access
		{
			rt_export_access(int64_t begin_, int64_t end_, rt_export& parent_)
			    : beginIndex(begin_),
			      endIndex(end_),
			      parent(parent_) {
			}

			rt_export_access() = delete;
			~rt_export_access() {
				this->parent.reset_buffer(this->endIndex);
			};

			NO_COPY_MOVE(rt_export_access);

		private:
			int64_t beginIndex;
			int64_t endIndex;

			rt_export& parent;

		public:
			const_iterator begin() const noexcept {
				return const_iterator(this->parent, this->beginIndex);
			}

			const_iterator end() const noexcept {
				return const_iterator(this->parent, this->endIndex);
			}

			const_iterator middle() const noexcept {
				return const_iterator(this->parent, (this->beginIndex + this->endIndex) / 2);
			}

			bool empty() const noexcept {
				return this->beginIndex == this->endIndex;
			}
		};

	private:
		std::vector<T> data{};
		int64_t batchWriteIndex = 1;
		std::atomic<int64_t> writeI = 1;
		std::atomic<int64_t> readI{};
		bool currentlyAccessed = false;

		int64_t mod(int64_t i) {
			if constexpr (type == rt_export_size::type::normal) {
				return i % this->getBufferSize();
			}
			else {
				return i & (this->getBufferSize() - 1);
			}
		}

	public:
		rt_export(rt_export_size::normal size)
		    : data(size.size) {
		}

		rt_export(rt_export_size::power_of_two size)
		    : data(size.getSize()) {
		}

		int64_t getBufferSize() const noexcept {
			return static_cast<int64_t>(this->data.size());
		}

		// Silently fails on writing to full buffer
		bool write(T const& val) noexcept {
			auto writeIndex = this->writeI.load();
			auto readIndex = this->readI.load();

			if (writeIndex == readIndex + this->getBufferSize()) {
				return false;
			}

			this->data[this->mod(writeIndex)] = val;

			this->writeI.store(writeIndex + 1);

			return true;
		};

		bool batchWrite(T const& val) noexcept {
			auto readIndex = this->readI.load();

			if (this->batchWriteIndex == readIndex + this->getBufferSize()) {
				return false;
			}

			this->data[this->mod(this->batchWriteIndex)] = val;

			this->batchWriteIndex++;

			return true;
		}

		void sendBatch() noexcept {
			this->writeI.store(this->batchWriteIndex);
			if (this->batchWriteIndex > this->getBufferSize() * 2) {
				this->batchWriteIndex -= this->getBufferSize();
			}
		}

		T const& peek() const noexcept {
			// can do this when writeI always starts out as at least 1
			auto const i = this->mod(this->writeI.load() - 1);
			return this->data[i];
		}

		T const& peekBatch() const noexcept {
			// can do this when writeI always starts out as at least 1
			auto const i = this->mod(this->batchWriteIndex - 1);
			return this->data[i];
		}

		rt_export<T, type>::rt_export_access consume_buffer() {
			assert(!this->currentlyAccessed);
			this->currentlyAccessed = true;

			auto begin = this->readI.load();
			auto end = this->writeI.load();

			return rt_export_access(begin, end, *this);
		}

		void reset_buffer(int64_t newBegin) {
			assert(this->currentlyAccessed);
			this->currentlyAccessed = false;

			if (newBegin > this->getBufferSize() * 2) {
				this->readI.store(newBegin - this->getBufferSize());
				this->writeI.fetch_sub(this->getBufferSize());
			}
			else {
				this->readI.store(newBegin);
			}
		}

		int64_t getCurrentSize() const {
			auto writeIndex = this->writeI.load();
			auto readIndex = this->readI.load();

			return writeIndex - readIndex;
		}

		bool empty() const {
			return this->getCurrentSize() == 0;
		}

		~rt_export() {
			assert(!this->currentlyAccessed);
		}
	};

	template<class T>
	rt_export(rt_export_size::normal) -> rt_export<T, rt_export_size::type::normal>;

	template<class T>
	rt_export(rt_export_size::power_of_two) -> rt_export<T, rt_export_size::type::restricted>;
}
