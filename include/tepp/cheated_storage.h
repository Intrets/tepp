#pragma once

#include <type_traits>

#include <cassert>

namespace te
{
	template <size_t size, size_t align = alignof(max_align_t)>
	struct cheated_storage
	{
		alignas(align) std::byte storage[size];

		template<class T, class... Args>
		void init(Args&&... args) {
			static_assert(sizeof(T) <= size);
			static_assert(alignof(T) == align);

			::new(&this->storage) T(std::forward<Args>(args)...);
		}

		template<class T>
		T& access() {
			static_assert(sizeof(T) <= size);
			static_assert(alignof(T) == align);

			return *reinterpret_cast<T*>(&this->storage);
		}

		template<class T>
		T const& access() const {
			static_assert(sizeof(T) <= size);
			static_assert(alignof(T) == align);

			return *reinterpret_cast<T const*>(&this->storage);
		}
	};
}