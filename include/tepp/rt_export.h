// tepp - A template library for C++
// Copyright (C) 2021 intrets

#pragma once

#include <atomic>
#include <cassert>
#include <vector>

#include "misc.h"

namespace te
{
	template<class T>
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
			int index;
			rt_export& parent;

		public:
			const_iterator(rt_export& parent_, int index_) noexcept
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
				auto i = this->index % this->parent.getBufferSize();
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
			rt_export_access(int begin_, int end_, rt_export& parent_)
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
			int beginIndex;
			int endIndex;

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
		};

	private:
		std::vector<T> data{};
		int batchWriteIndex{};
		std::atomic<int> writeI{};
		std::atomic<int> readI{};
		bool currentlyAccessed = false;

	public:
		rt_export(size_t size)
		    : data(size){};

		size_t getBufferSize() const noexcept {
			return this->data.size();
		}

		// Silently fails on writing to full buffer
		void write(T const& val) noexcept {
			auto writeIndex = this->writeI.load();
			auto readIndex = this->readI.load();

			if (writeIndex < readIndex) {
				readIndex -= static_cast<int>(this->getBufferSize());
			}

			auto diff = writeIndex - readIndex;

			if (diff == this->getBufferSize() - 1) {
				return;
			}

			this->data[writeIndex] = val;

			int newWriteIndex = (writeIndex + 1) % this->getBufferSize();

			this->writeI.store(newWriteIndex);
		};

		void batchWrite(T const& val) noexcept {
			auto readIndex = this->readI.load();

			if (this->batchWriteIndex < readIndex) {
				readIndex -= static_cast<int>(this->getBufferSize());
			}

			auto diff = this->batchWriteIndex - readIndex;

			if (diff == this->getBufferSize() - 1) {
				return;
			}

			this->data[this->batchWriteIndex] = val;

			this->batchWriteIndex = (this->batchWriteIndex + 1) % this->getBufferSize();
		}

		void sendBatch() noexcept {
			this->writeI.store(this->batchWriteIndex);
		}

		T const& peek() const noexcept {
			auto const i = (this->writeI.load() + this->data.size() - 1) % this->data.size();
			return this->data[i];
		}

		T const& peekBatch() const noexcept {
			auto const i = (this->batchWriteIndex + this->data.size() - 1) % this->data.size();
			return this->data[i];
		}

		rt_export<T>::rt_export_access consume_buffer() {
			assert(!this->currentlyAccessed);
			this->currentlyAccessed = true;

			auto begin = this->readI.load();
			auto end = this->writeI.load();

			if (end < begin) {
				end += static_cast<int>(this->data.size());
			}

			return rt_export_access(begin, end, *this);
		}

		void reset_buffer(int newBegin) {
			assert(this->currentlyAccessed);
			this->currentlyAccessed = false;

			this->readI.store(newBegin % this->getBufferSize());
		}

		int getCurrentSize() const {
			auto writeIndex = this->writeI.load();
			auto readIndex = this->readI.load();

			auto d = writeIndex - readIndex;
			if (d < 0) {
				d += static_cast<int>(this->getBufferSize());
			}

			return d;
		}

		bool empty() const {
			return this->writeI.load() == this->readI.load();
		}

		~rt_export() {
			assert(!this->currentlyAccessed);
		}
	};
}

template<class T>
typename te::rt_export<T>::const_iterator::difference_type distance(typename te::rt_export<T>::const_iterator const& it1, typename te::rt_export<T>::const_iterator const& it2) {
	return it2 - it1;
}
