#pragma once

#include <type_traits>

#include <cassert>
#include <new>

#if DEBUG_BUILD
#include <mutex>

namespace te::debug
{
	int& typeCounter();

	template<class T>
	static int getTypeIndex() {
		static std::mutex mutex;

		auto lock = std::scoped_lock(mutex);

		static int typeIndex = typeCounter()++;
		return typeIndex;
	}
}
#endif // DEBUG_BUILD

namespace te
{
	template <size_t size, size_t align = alignof(max_align_t)>
	struct cheated_storage
	{
		alignas(align) std::byte storage[size];

#if DEBUG_BUILD
		int typeIndex = debug::getTypeIndex<void>();
#endif // DEBUG_BUILD

		template<class T, class... Args>
		void init(Args&&... args) {
			static_assert(sizeof(T) <= size);
			static_assert(alignof(T) == align);

#if DEBUG_BUILD
			assert(this->typeIndex == debug::getTypeIndex<void>());
			this->typeIndex = debug::getTypeIndex<T>();
#endif // DEBUG_BUILD

			::new(&this->storage) T(std::forward<Args>(args)...);
		}

		template<class T>
		T& access() {
			static_assert(sizeof(T) <= size);
			static_assert(alignof(T) == align);

#if DEBUG_BUILD
			assert(this->typeIndex == debug::getTypeIndex<T>());
#endif // DEBUG_BUILD

			return *std::launder(reinterpret_cast<T*>(&this->storage));
		}

		template<class T>
		T const& access() const {
			static_assert(sizeof(T) <= size);
			static_assert(alignof(T) == align);

#if DEBUG_BUILD
			assert(this->typeIndex == debug::getTypeIndex<T>());
#endif // DEBUG_BUILD

			return *std::launder(reinterpret_cast<T const*>(&this->storage));
		}
	};
}