// tepp - A template library for C++
// Copyright (C) 2021 intrets

#pragma once

#include <atomic>
#include <utility>
#include <vector>

#include "misc.h"

#include "tepp/assert.h"
#include "tepp/rt_export_size.h"
#include "tepp/span.h"

namespace te
{
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
			integer_t index;
			rt_export& parent;

		public:
			const_iterator(rt_export& parent_, integer_t index_) noexcept
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

			const_iterator& operator+=(integer_t inc) noexcept {
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
				tassert(&this->parent == &other.parent);

				this->index = other.index;
				return *this;
			}
		};

		struct rt_export_access
		{
			rt_export_access(integer_t begin_, integer_t end_, rt_export& parent_)
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
			integer_t beginIndex;
			integer_t endIndex;

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

			integer_t size() const noexcept {
				if (this->endIndex > this->beginIndex) {
					return this->endIndex - this->beginIndex;
				}
				else {
					return this->parent.getBufferSize() - (this->endIndex - this->beginIndex);
				}
			}

			bool empty() const noexcept {
				return this->beginIndex == this->endIndex;
			}
		};

	private:
		std::vector<T> data{};
		integer_t batchWriteIndex = 1;
		std::atomic<integer_t> writeI = 1;
		std::atomic<integer_t> readI{};
		bool currentlyAccessed = false;

		integer_t mod(integer_t i) {
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

		integer_t getBufferSize() const noexcept {
			return isize(this->data);
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

		void batchTrim(integer_t size) noexcept {
			auto currentSize = this->batchWriteIndex - this->readI.load();
			auto difference = currentSize - size;

			if (difference > 0) {
				this->writeI.fetch_add(difference);
				this->readI.fetch_add(difference);
			}
		}

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
			tassert(!this->currentlyAccessed);
			this->currentlyAccessed = true;

			auto begin = this->readI.load();
			auto end = this->writeI.load();

			return rt_export_access(begin, end, *this);
		}

		void write_into(std::vector<T>& destination) {
			tassert(!this->currentlyAccessed);
			this->currentlyAccessed = true;

			auto begin = this->readI.load();
			auto end = this->writeI.load();

			auto size = end - begin;

			if (std::cmp_less(destination.capacity(), size)) {
				destination.reserve(size * 2);
			}

			if (size == 0) {
				destination.resize(size);
			}
			else if (size > 0) {
				destination.resize(size);

				auto beginModded = this->mod(begin);
				auto endModded = this->mod(end);

				if (beginModded < endModded) {
					std::ranges::copy(this->data.begin() + beginModded, this->data.begin() + endModded, destination.begin());
				}
				else {
					auto it = std::ranges::copy(this->data.begin() + beginModded, this->data.end(), destination.begin());
					std::ranges::copy(this->data.begin(), this->data.begin() + endModded, it.out);
				}
			}
			else {
				tassert(0);
			}

			this->reset_buffer(end);
		}

		void reset_buffer(integer_t newBegin) {
			tassert(this->currentlyAccessed);
			this->currentlyAccessed = false;

			if (newBegin > this->getBufferSize() * 2) {
				auto newRead = newBegin = this->getBufferSize();
				this->readI.store(newBegin - this->getBufferSize());
				auto newWrite = this->writeI.fetch_sub(this->getBufferSize());
				if (newWrite >= newRead) {
					this->writeI.fetch_sub(this->getBufferSize());
				}
			}
			else {
				this->readI.store(newBegin);
			}
		}

		integer_t getCurrentSize() const {
			auto writeIndex = this->writeI.load();
			auto readIndex = this->readI.load();

			return writeIndex - readIndex;
		}

		bool empty() const {
			return this->getCurrentSize() == 0;
		}

		~rt_export() {
			tassert(!this->currentlyAccessed);
		}
	};

	template<class T>
	struct rt_export2
	{
		std::vector<T> data{};
		integer_t batchWriteI = 1;
		std::atomic<integer_t> writeI = 1;
		std::atomic<integer_t> readI{};

		integer_t mod(integer_t index) const {
			return index & (this->getBufferSize() - 1);
		}

		integer_t getBufferSize() const {
			return isize(this->data);
		}

		bool write(T const& value) {
			auto nextBatchWriteI = this->mod(this->batchWriteI + 1);
			if (nextBatchWriteI != this->readI) {
				this->data[this->batchWriteI] = value;
				this->batchWriteI = nextBatchWriteI;
				return true;
			}
			else {
				return false;
			}
		}

		void send() {
			this->writeI.store(this->batchWriteI, std::memory_order_release);
		}

		bool empty() const {
			return this->writeI.load(std::memory_order_acquire) == this->readI.load(std::memory_order_acquire);
		}

		void write_into(std::vector<T>& destination) {
			auto const begin = this->readI.load(std::memory_order_acquire);
			auto const end = this->writeI.load(std::memory_order_acquire);

			if (begin <= end) {
				destination.resize(end - begin);
				std::ranges::copy(te::slice(this->data, begin, end), destination.begin());
			}
			else {
				destination.resize(this->getBufferSize() - (begin - end));
				auto span1 = te::slice(this->data, begin, {});
				auto span2 = te::slice(this->data, {}, end);
				std::ranges::copy(span1, destination.begin());
				std::ranges::copy(span2, destination.begin() + isize(span1));
			}

			this->readI.store(end, std::memory_order_release);
		}

		rt_export2(rt_export_size::power_of_two size)
		    : data(size.getSize()) {
		}

		rt_export2() = delete;
		~rt_export2() = default;
	};
}
