#pragma once

#include "tepp/integers.h"
#include "tepp/misc.h"
#include "tepp/optional_ref.h"

#include <memory>
#include <span>

namespace te
{
	template<class T>
	concept arena_safe = std::is_trivially_destructible_v<T>;

	struct arena
	{
		void* data = nullptr;
		void* ptr = nullptr;
		size_t remaining = 0;
		size_t size = 0;

		integer_t capacityRequest = 0;

		std::optional<integer_t> checkSize();

		arena(arena&& other);
		arena& operator=(arena&& other);

		NO_COPY(arena);

		struct B
		{
			integer_t value{};

			integer_t get() {
				return this->value;
			}
		};

		struct KB
		{
			integer_t value{};

			integer_t get() {
				return this->value * 1024;
			}
		};

		struct MB
		{
			integer_t value{};

			integer_t get() {
				return this->value * 1024 * 1024;
			}
		};

		void advancePtr(integer_t s) {
			this->ptr = static_cast<void*>(static_cast<char*>(this->ptr) + s);
			this->remaining -= s;
		}

		template<arena_safe T, class... Args>
		te::optional_ref<T> make(Args&&... args) {
			auto oldPtr = this->ptr;
			auto oldRemaining = this->remaining;

			auto alignmentResult = std::align(alignof(T), sizeof(T), this->ptr, this->remaining);

			if (alignmentResult == nullptr) {
				this->ptr = oldPtr;
				this->remaining = oldRemaining;

				this->capacityRequest = this->size - this->remaining + sizeof(T);

				return te::nullopt;
			}

			auto result = new (this->ptr) T(std::forward<Args>(args)...);

			this->advancePtr(sizeof(T));

			return *result;
		}

		template<arena_safe T>
		std::optional<std::span<T>> makeSpan(integer_t count) {
			auto oldPtr = this->ptr;
			auto oldRemaining = this->remaining;

			auto alignmentResult = std::align(alignof(T), sizeof(T) * count, this->ptr, this->remaining);

			auto spanSize = sizeof(T) * count;
			constexpr auto overheadSafetyMargin = sizeof(void*) * 4;

			if (alignmentResult == nullptr || this->remaining < spanSize + overheadSafetyMargin) {
				this->ptr = oldPtr;
				this->remaining = oldRemaining;

				this->capacityRequest = this->size - this->remaining + spanSize + overheadSafetyMargin;

				return std::nullopt;
			}

			auto newPtr = new (this->ptr) T[count];
			auto diff = newPtr - reinterpret_cast<T*>(this->ptr);
			if (diff > overheadSafetyMargin) {
				std::abort();
			}

			this->advancePtr(diff + spanSize);

			return std::span(newPtr, count);
		}

		void reset();

		arena() = default;
		arena(B size_);
		arena(KB size_);
		arena(MB size_);
		explicit arena(integer_t size_);

		~arena();
	};
}
