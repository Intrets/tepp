#pragma once

#include <tepp/integers.h>
#include <tepp/misc.h>
#include <tepp/span.h>

#include <cstdlib>

namespace te
{
	template<class T, integer_t alignment = alignof(T)>
	struct trivial_buffer
	{
		T* data = nullptr;
		integer_t size = 0;

		T& operator[](integer_t i) {
			return this->data[i];
		}

		T const& operator[](integer_t i) const {
			return this->data[i];
		}

		te::span<T> getSpan() {
			return te::span(this->data, this->data + size);
		}

		void fill(T const& t) {
			std::ranges::fill(this->getSpan(), t);
		}

		trivial_buffer() = default;
		trivial_buffer(trivial_buffer&& other) {
			this->data = other.data;
			this->size = other.size;

			other.data = nullptr;
			other.size = 0;
		}
		trivial_buffer& operator=(trivial_buffer&& other) {
			this->free();

			this->data = other.data;
			this->size = other.size;

			other.data = nullptr;
			other.size = 0;

			return *this;
		}

		NO_COPY(trivial_buffer);

		trivial_buffer(integer_t size_) {
			this->size = size_;
#ifdef WIN32
			this->data = std::launder(reinterpret_cast<T*>(_aligned_malloc(size_ * sizeof(T), alignment)));
#else
			this->data = std::launder(reinterpret_cast<T*>(std::aligned_alloc(alignment, size_ * sizeof(T))));
#endif

			std::memset(this->data, 0, size_ * sizeof(T));
		}

	private:
		void free() {
			if (this->data != nullptr && this->size != 0) {
#ifdef WIN32
				_aligned_free(this->data);
#else
				std::free(this->data);
#endif
			}
		}

	public:
		~trivial_buffer() {
			this->free();
		}
	};
}
