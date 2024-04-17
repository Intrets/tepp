#pragma once

namespace te
{
	template<class T>
	struct ring_buffer
	{
		template<class value_type_>
		struct iterator
		{
			using iterator_category = std::random_access_iterator_tag;
			using value_type = value_type_;
			using difference_type = std::ptrdiff_t;
			using pointer = value_type*;
			using reference = value_type&;

		private:
			int64_t index{};
			ring_buffer<T>& buffer{};

		public:
			iterator(int64_t index_, ring_buffer<T>& buffer_)
			    : index(index_),
			      buffer(buffer_) {
			}

			iterator& operator++() noexcept {
				this->buffer.advanceIndex(this->index);
				return *this;
			}

			iterator operator++(int) noexcept {
				auto r = *this;
				this->buffer.advanceIndex(r.index);
				return r;
			}

			iterator& operator--() noexcept {
				this->buffer.calculateIndex(this->index, -1);
				return *this;
			}

			iterator operator--(int) noexcept {
				auto r = *this;
				this->buffer.calculateIndex(r.index, -1);
				return r;
			}

			bool operator==(iterator const& other) const noexcept {
				return this->index == other.index;
			}

			reference operator*() const noexcept {
				return this->buffer.data[this->index];
			}
		};

		std::vector<T> data{};
		int64_t beginIndex{};
		int64_t endIndex{};


		auto begin() {
			return iterator<T>(this->beginIndex, *this);
		}

		auto end() {
			return iterator<T>(this->endIndex, *this);
		}

		auto begin() const {
			return iterator<T const>(this->beginIndex, *this);
		}

		auto end() const {
			return iterator<T const>(this->endIndex, *this);
		}

		ring_buffer(std::integral auto size) {
			this->data.resize(size + 1);
		}

		template<class... Args>
		T& emplace(Args&&... args) {
			auto& result = this->data[this->endIndex];
			result = T(std::forward<Args>(args)...);
			this->advanceIndex(this->endIndex);
			return result;
		}

	private:
		int64_t modIndex(int64_t index) {
			return index % static_cast<int64_t>(this->data.size());
		}

		void advanceIndex(int64_t& index) {
			index = this->modIndex(index + 1);
		}

		void calculateIndex(int64_t& index, int64_t offset) {
			index = this->modIndex(index + offset);
		}

		void advance() {
			this->advanceIndex(this->endIndex);
			if (this->endIndex == this->beginIndex) {
				this->advanceIndex(this->beginIndex);
			}
		}
	};
}
