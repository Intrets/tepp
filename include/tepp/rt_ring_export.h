#pragma once

#include <atomic>
#include <cassert>
#include <vector>

#include "misc.h"

#include "tepp/rt_export_size.h"

namespace te
{
	template<class T, rt_export_size::type type = rt_export_size::type::normal>
	struct rt_ring_export
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
			rt_ring_export& parent;

		public:
			const_iterator(rt_ring_export& parent_, int64_t index_) noexcept
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

			const_iterator& operator+=(int64_t inc) noexcept {
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

			const_iterator(const_iterator const& other)
			    : parent(other.parent) {
				this->index = other.index;
			}
		};

		struct rt_ring_export_access
		{
			rt_ring_export_access(int64_t begin_, int64_t end_, rt_ring_export& parent_)
			    : beginIndex(begin_),
			      endIndex(end_),
			      parent(parent_) {
			}

			rt_ring_export_access() = delete;
			~rt_ring_export_access() {
				this->parent.readI.store(-1);
				this->parent.currentlyAccessed = false;
			};

			NO_COPY_MOVE(rt_ring_export_access);

		private:
			int64_t beginIndex;
			int64_t endIndex;

			rt_ring_export& parent;

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

			int64_t size() const noexcept {
				if (this->endIndex >= this->beginIndex) {
					return this->endIndex - this->beginIndex;
				}
				else {
					return this->parent.getBufferSize() - (this->beginIndex - this->endIndex);
				}
			}

			bool empty() const noexcept {
				return this->beginIndex == this->endIndex;
			}
		};

	private:
		std::vector<T> data{};
		std::atomic<int64_t> writeI = 1;
		std::atomic<int64_t> readI = -1;
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
		rt_ring_export(rt_export_size::normal size)
		    : data(size.size) {
		}

		rt_ring_export(rt_export_size::power_of_two size)
		    : data(size.getSize()) {
		}

		int64_t getBufferSize() const noexcept {
			return static_cast<int64_t>(this->data.size());
		}

		// Silently fails on writing to full buffer
		bool write(T const& val) noexcept {
			auto writeIndex = this->writeI.load();
			auto readIndex = this->readI.load();

			if (writeIndex == readIndex) {
				return false;
			}

			this->data[this->mod(writeIndex)] = val;
			this->writeI.store(this->mod(writeIndex + 1));

			return true;
		};

		T const& peek() const noexcept {
			// can do this when writeI always starts out as at least 1
			auto const i = this->mod(this->writeI.load() - 1);
			return this->data[i];
		}

		rt_ring_export<T, type>::rt_ring_export_access access_buffer(int64_t size) {
			assert(!this->currentlyAccessed);
			this->currentlyAccessed = true;

			auto end = this->writeI.load();
			auto begin = this->mod(end + this->getBufferSize() - size);

			this->readI.store(begin);

			return rt_ring_export_access(begin, end, *this);
		}

		int64_t getCurrentSize() const {
			auto writeIndex = this->writeI.load();
			auto readIndex = this->readI.load();

			if (writeIndex > readIndex) {
				return writeIndex = readIndex;
			}
			else {
				return this->getBufferSize() - (writeIndex - readIndex);
			}
		}

		bool empty() const {
			return this->getCurrentSize() == 0;
		}

		~rt_ring_export() {
			assert(!this->currentlyAccessed);
		}
	};
}
