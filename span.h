#pragma once

#include "tepp/assert.h"
#include "tepp/integers.h"
#include "tepp/misc.h"
#include "tepp/tepp.h"

namespace te
{
	template<class T>
	concept can_dereference = requires(T t) { *t; };

	template<class T>
	concept can_index = requires(T t, integer_t i) { t[i]; };

	template<class T>
	auto get_type() {
		if constexpr (can_index<T>) {
			return Type<std::remove_reference_t<decltype(std::declval<T>()[0])>>;
		}
		else if constexpr (can_dereference<T>) {
			return Type<std::remove_reference_t<decltype(*std::declval<T>())>>;
		}
	}

	template<class R>
	auto get_pointer(R&& r) {
		if constexpr (can_index<R>) {
			return &r[0];
		}
		else if constexpr (can_dereference<R>) {
			return &*r;
		}
	}

	template<class T>
	struct span
	{
	private:
		T* data_ = nullptr;
		integer_t size_ = 0;

	public:
		span subspan(integer_t offset) const {
			tassert(offset <= this->size());
			return span(this->data() + offset, this->size() - offset);
		}

		span subspan(integer_t offset, integer_t count) const {
			tassert(offset + count <= this->size());
			return span(this->data() + offset, count);
		}

		bool empty() const {
			return this->size() == 0;
		}

		T* data() const {
			return this->data_;
		}

		T& operator[](integer_t i) const {
			tassert(i >= 0 && i < this->size());
			return this->data_[i];
		}

		T& front() const {
			return *this->data_;
		}

		T& back() const {
			tassert(!this->empty());
			return *(this->data_ + this->size() - 1);
		}

		T* begin() const {
			return this->data_;
		}

		T* end() const {
			return this->data_ + this->size_;
		}

		integer_t size() const {
			return this->size_;
		}

		integer_t size_bytes() const {
			return this->size_ * sizeof(T);
		}

		span(T* data, T* data_end)
		    : data_(data),
		      size_(data_end - data) {
		}

		span(T* data, integer_t size)
		    : data_(data),
		      size_(size) {
		}

		DEFAULT_COPY_MOVE(span);

		template<can_index R>
		span(R&& r)
		    : span(isize(r) == 0 ? nullptr : &r[0], isize(r)) {
		}

		template<can_dereference R>
		span(R&& r)
		    : span(isize(r) == 0 ? nullptr : &*r, isize(r)) {
		}

		template<class R1, class R2>
		span(R1&& r1, R2&& r2) {
			this->data_ = get_pointer(r1);
			if constexpr (std::integral<std::remove_cvref_t<R2>>) {
				this->size_ = r2;
			}
			else {
				this->size_ = get_pointer(r2) - this->data_;
			}
		}

		span() = default;
		~span() = default;
	};

	template<class T>
	auto as_bytes(span<T> span) {
		if constexpr (std::is_const_v<T>) {
			return te::span<std::byte const>(reinterpret_cast<std::byte const*>(span.data()), span.size() * sizeof(T));
		}
		else {
			return te::span<std::byte>(reinterpret_cast<std::byte*>(span.data()), span.size() * sizeof(T));
		}
	}

	template<class R>
	span(R&&) -> span<Gettype(get_type<R>())>;

	template<class R1, class R2>
	span(R1&&, R2&&) -> span<Gettype(get_type<R1>())>;

	namespace detail
	{
		template<class T>
		concept can_be_converted_to_span = requires(T t) { te::span(t); };
	}

	auto subspan(detail::can_be_converted_to_span auto&& span_, std::optional<integer_t> start_, std::optional<integer_t> size_) {
		auto span = te::span(span_);

		auto start = start_.value_or(0_i);
		start = std::clamp(start, 0_i, isize(span));

		auto size = size_.value_or(isize(span) - start);
		size = std::clamp(size, 0_i, isize(span) - start);

		return span.subspan(start, size);
	}

	auto slice(detail::can_be_converted_to_span auto&& span_, std::optional<integer_t> start_, std::optional<integer_t> end_) {
		auto span = te::span(span_);

		using R = decltype(span);

		auto start = start_.value_or(0_i);
		auto end = end_.value_or(isize(span));

		if (span.empty()) {
			return R();
		}

		if (start < 0) {
			start += isize(span);
		}
		if (end < 0) {
			end += isize(span);
		}

		start = std::clamp(start, 0_i, isize(span));
		end = std::clamp(end, 0_i, isize(span));

		if (end < start) {
			return R();
		}

		return span.subspan(start, end - start);
	}

}
