#pragma once

#include <type_traits>
#include <functional>

#include <cassert>
#include <new>

#include "tepp/misc.h"
#include "tepp/tepp.h"

#if DEBUG_BUILD
#include <mutex>

namespace te::debug
{
	integer_t& typeCounter();

	template<class T>
	static integer_t getTypeIndex() {
		static std::mutex mutex;

		auto lock = std::scoped_lock(mutex);

		static integer_t typeIndex = typeCounter()++;
		return typeIndex;
	}
}
#endif // DEBUG_BUILD

namespace te
{
	struct trivial_destructor {};
	struct custom_destructor
	{
		std::function<void(void*)> f;
	};
	struct automatic_destructor
	{
		void (*f)(void*) = nullptr;
	};

	namespace detail
	{
		template<class T, integer_t size, integer_t align, te::member_of<te::list_type<trivial_destructor, custom_destructor, automatic_destructor>> destructor>
		struct is_valid_storage
		{
			inline constexpr static bool valid_alignnment_and_size = [] {
				return sizeof(T) <= size && alignof(T) == align;
			}();

			inline constexpr static bool valid_destructor = [] {
				if constexpr (std::same_as<trivial_destructor, destructor>) {
					return std::is_trivially_destructible_v<T>;
				}

				return true;
			}();

			inline constexpr static bool value = valid_destructor && valid_alignnment_and_size;
		};
	}

	template<class T, integer_t size, integer_t align, class destructor>
	concept valid_storage = detail::is_valid_storage<T, size, align, destructor>::value;

	template <integer_t size, integer_t align, class destructor = trivial_destructor>
	struct cheated_storage : destructor
	{
		alignas(align) std::byte storage[size];

#if DEBUG_BUILD
		integer_t typeIndex = debug::getTypeIndex<void>();
#endif // DEBUG_BUILD

		template<te::valid_storage<size, align, destructor> T, class... Args>
			requires (!std::same_as<destructor, custom_destructor>)
		void init(Args&&... args) {
#if DEBUG_BUILD
			assert(this->typeIndex == debug::getTypeIndex<void>());
			this->typeIndex = debug::getTypeIndex<T>();
#endif // DEBUG_BUILD

			if constexpr (std::same_as<destructor, automatic_destructor>) {
				this->f = [](void* object) {
					reinterpret_cast<T*>(object)->~T();
				};
			}

			::new(&this->storage) T(std::forward<Args>(args)...);
		}

		template<te::valid_storage<size, align, destructor> T, class F, class... Args>
			requires (std::same_as<destructor, custom_destructor>)
		void init(F&& d, Args&&... args) {
#if DEBUG_BUILD
			assert(this->typeIndex == debug::getTypeIndex<void>());
			this->typeIndex = debug::getTypeIndex<T>();
#endif // DEBUG_BUILD

			this->f = [d = std::forward<F>(d)](void* object) {
				std::invoke(d, *reinterpret_cast<T*>(object));
			};

			::new(&this->storage) T(std::forward<Args>(args)...);
		}

		template<te::valid_storage<size, align, destructor> T>
		T& access() {
#if DEBUG_BUILD
			assert(this->typeIndex == debug::getTypeIndex<T>());
#endif // DEBUG_BUILD

			return *std::launder(reinterpret_cast<T*>(&this->storage));
		}

		template<te::valid_storage<size, align, destructor> T>
		T const& access() const {
#if DEBUG_BUILD
			assert(this->typeIndex == debug::getTypeIndex<T>());
#endif // DEBUG_BUILD

			return *std::launder(reinterpret_cast<T const*>(&this->storage));
		}

		cheated_storage() = default;
		~cheated_storage() {
#if DEBUG_BUILD
			assert(this->typeIndex != debug::getTypeIndex<void>());
#endif // DEBUG_BUILD

			if constexpr (std::same_as<destructor, automatic_destructor>) {
				this->f(this->storage);
			}
			else if constexpr (std::same_as<destructor, custom_destructor>) {
				this->f(this->storage);
			}
		};
	};
}
