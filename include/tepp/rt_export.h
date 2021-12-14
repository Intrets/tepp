// tepp - A template library for C++
// Copyright (C) 2021  Intrets
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

#pragma once

#include <vector>
#include <atomic>
#include <cassert>

#define DEFAULT_COPY(T) T(const T&) = default; T& operator=(const T&) = default;
#define NO_COPY(T) T(const T&) = delete; T& operator=(const T&) = delete;
#define DEFAULT_MOVE(T) T(T&&) = default; T& operator=(T&&) = default;
#define NO_MOVE(T) T(T&&) = delete; T& operator=(T&&) = delete;
#define DEFAULT_COPY_MOVE(T) DEFAULT_COPY(T) DEFAULT_MOVE(T)
#define NO_COPY_MOVE(T) NO_COPY(T) NO_MOVE(T)

namespace te
{
	template<class T>
	struct rt_export
	{
		struct const_iterator : std::input_iterator_tag
		{
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T const&;

		private:
			int index;
			rt_export& parent;

		public:
			const_iterator(rt_export& parent_, int index_) noexcept :
				parent(parent_),
				index(index_) {
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
		};

		struct rt_export_access
		{
			rt_export_access(int begin_, int end_, rt_export& parent_) :
				beginIndex(begin_),
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
		};

	private:
		std::vector<T> data;
		std::atomic<int> writeI;
		std::atomic<int> readI;
		bool currentlyAccessed = false;

	public:
		rt_export(size_t size) : data(size) {};

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

		~rt_export() {
			assert(!this->currentlyAccessed);
		}
	};

}
