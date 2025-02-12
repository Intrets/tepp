#pragma once

#include <atomic>
#include <cassert>
#include <vector>

#include "misc.h"

#include "tepp/optional_ref.h"
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
			integer_t index;
			rt_ring_export& parent;

		public:
			const_iterator(rt_ring_export& parent_, integer_t index_) noexcept
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
			rt_ring_export_access(integer_t begin_, integer_t end_, rt_ring_export& parent_, std::optional<integer_t> setEnd_ = std::nullopt)
			    : beginIndex(begin_),
			      endIndex(end_),
			      setEnd(setEnd_),
			      parent(parent_) {
			}

			rt_ring_export_access() = delete;
			~rt_ring_export_access() {
				if (this->lifetime) {
					if (this->setEnd.has_value()) {
						this->parent.readI.store(this->setEnd.value());
					}
					this->parent.currentlyAccessed = false;
					this->lifetime = false;
				}
			};

			NO_COPY(rt_ring_export_access);

			rt_ring_export_access& operator=(rt_ring_export_access&&) = delete;
			rt_ring_export_access(rt_ring_export_access&& other) noexcept
			    : rt_ring_export_access(
			          other.beginIndex,
			          other.endIndex,
			          other.parent,
			          other.setEnd
			      ) {
				this->lifetime = other.lifetime;
				other.lifetime = false;
			}

		private:
			integer_t beginIndex;
			integer_t endIndex;
			std::optional<integer_t> setEnd{};

			rt_ring_export& parent;

			bool lifetime = true;

		public:
			const_iterator offset_from_end(integer_t count) const noexcept {
				count = std::min(this->size(), count);
				return const_iterator(this->parent, this->parent.mod(this->endIndex - count + this->parent.getBufferSize()));
			}

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
		integer_t writeJ = 1;
		std::atomic<integer_t> writeI = 1;
		std::atomic<integer_t> readI = 0;
		bool currentlyAccessed = false;

		integer_t mod(integer_t i) const {
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

		template<class F>
		void for_each(F&& f) {
			for (auto& e : this->data) {
				f(e);
			}
		}

		integer_t getBufferSize() const noexcept {
			return isize(this->data);
		}

		void reset() {
			this->writeI.store(1);
			this->readI.store(0);
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

		te::optional_ref<T> getWrite() noexcept {
			auto writeIndex = this->writeI.load();
			auto readIndex = this->readI.load();

			if (writeIndex == readIndex) {
				return te::nullopt;
			}
			else {
				return this->data[writeIndex];
			}
		}

		bool advanceWrite() noexcept {
			auto readIndex = this->readI.load();
			auto writeIndex = this->writeI.load();

			if (readIndex == writeIndex) {
				return false;
			}

			this->writeI.store(this->mod(writeIndex + 1));

			return true;
		}

		void consumeRead(integer_t margin = 0) {
			auto readIndex = this->readI.load();
			auto writeIndex = this->writeI.load();

			if (writeIndex <= readIndex) {
				writeIndex += this->getBufferSize();
			}

			auto newReadIndex = std::max(readIndex, writeIndex - margin);

			this->readI.store(this->mod(newReadIndex));
		}

		T& getRead() noexcept {
			return this->data[this->readI.load()];
		}

		bool writeBuffered(T const& val) noexcept {
			auto writeIndex = this->writeJ;
			auto readIndex = this->readI.load();

			if (writeIndex == readIndex) {
				return false;
			}

			this->data[this->mod(writeIndex)] = val;
			this->writeJ = this->mod(writeIndex + 1);

			return true;
		};

		void sendBuffer() noexcept {
			this->writeI.store(this->writeJ);
		}

		T const& peek() const noexcept {
			// can do this when writeI always starts out as at least 1
			auto const i = this->mod(this->writeI.load() - 1);
			return this->data[i];
		}

		rt_ring_export<T, type>::rt_ring_export_access access_buffer(integer_t size) {
			assert(!this->currentlyAccessed);
			this->currentlyAccessed = true;

			auto end = this->writeI.load();
			auto begin = this->mod(end + this->getBufferSize() - size);

			this->readI.store(begin);

			return rt_ring_export_access(begin, end, *this);
		}

		rt_ring_export<T, type>::rt_ring_export_access access_buffer_from_end(integer_t size) {
			assert(!this->currentlyAccessed);
			this->currentlyAccessed = true;

			auto begin = std::max(0_i, this->readI.load());
			auto end = this->mod(begin + size);

			return rt_ring_export_access(begin, end, *this, this->writeI.load());
		}

		integer_t getCurrentSize() const {
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
