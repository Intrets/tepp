#pragma once

#include <utility>

#include <tepp/optional_ref.h>
#include <tepp/tepp.h>

namespace te
{
	namespace detail
	{
		template<class T, class Arg, class... Args>
		constexpr integer_t argumentIndex() {
			if constexpr (std::same_as<T, Arg>) {
				return 0;
			}
			else {
				return 1 + argumentIndex<T, Args...>();
			}
		}
	}
	template<class B, class... Args>
	struct alignas(te::max_v<alignof(Args)...>) polyant
	{
		static_assert(sizeof...(Args) > 0);

		static constexpr integer_t storage_size = te::max_v<sizeof(Args)...>;
		static constexpr integer_t alignment = te::max_v<alignof(Args)...>;

		using First = te::head_t<te::list_type<Args...>>;

		alignas(alignment) std::byte storage[storage_size];
		integer_t type = -1;

		template<te::member_of2<Args...> T>
		static consteval integer_t getType() {
			return detail::argumentIndex<T, Args...>();
		}

	private:
		template<class T>
		T* getImpl() {
			return std::launder(reinterpret_cast<T*>(this->storage));
		}

	public:
		B& getBase() {
			return *this->getImpl<B>();
		}

		B* operator->() {
			return &this->getBase();
		}

		template<te::member_of2<Args...> T>
		te::optional_ref<T> getIf() {
			if (this->type == getType<T>()) {
				return *this->getImpl<T>();
			}
			else {
				return te::nullopt;
			}
		}

		template<te::member_of2<Args...> T, class... TArgs>
		T& emplace(TArgs&&... targs) noexcept(noexcept(T(std::forward<TArgs>(targs)...))) {
			this->clear();
			auto ptr = new (this->storage) T(std::forward<TArgs>(targs)...);
			this->type = getType<T>();
			return *ptr;
		}

		void clear() {
			if (this->type != -1) {
				this->getBase().~B();
				this->type = -1;
			}
		}

		template<te::member_of2<Args...> T>
		polyant(T&& other) noexcept(noexcept(this->emplace<T>(std::forward<T>(other)))) {
			this->emplace<T>(std::forward<T>(other));
		}

		template<te::member_of2<Args...> T>
		polyant(T const& other) noexcept(noexcept(this->emplace<T>(other))) {
			this->emplace<T>(other);
		}

		template<te::member_of2<Args...> T>
		polyant& operator=(T&& other) {
			this->clear();
			this->emplace<T>(std::forward<T>(other));

			return *this;
		}

		template<te::member_of2<Args...> T>
		polyant& operator=(T const& other) {
			this->clear();
			this->emplace<T>(other);

			return *this;
		}

		polyant() {
			this->emplace<First>();
		}

		~polyant() {
			this->clear();
		}
	};

}
