#pragma once

#include <tepp/misc.h>
#include <tepp/optional_ref.h>

#include <new>
#include <typeinfo>

namespace te
{
	struct any
	{
		void* object = nullptr;
		std::type_info const* type = nullptr;

		using destructor_function = void (*)(void*);
		destructor_function destructor = nullptr;

		template<class T>
		optional_ref<T> access() {
			if (this->type != nullptr && typeid(T) == *this->type) {
				return *std::launder(reinterpret_cast<T*>(this->object));
			}
			else {
				return te::nullopt;
			}
		}

		template<class T, class... Args>
		T& emplace(Args&&... args) {
			this->destruct();

			this->destructor = [](void* object_) {
				auto object = std::launder(reinterpret_cast<T*>(object_));
				delete object;
			};

			auto newObject = new T(std::forward<Args>(args)...);
			this->object = newObject;
			this->type = &typeid(T);

			return *newObject;
		}

		NO_COPY(any);
	private:
		void move_from(any&& other) {
			this->object = other.object;
			this->type = other.type;
			this->destructor = other.destructor;

			other.object = nullptr;
			other.type = {};
			other.destructor = nullptr;
		}

	public:
		any(any&& other) {
			this->move_from(std::move(other));
		}

		any& operator=(any&& other) {
			this->destruct();

			this->move_from(std::move(other));

			return *this;
		}

		void destruct() {
			if (this->object != nullptr) {
				tassert(this->destructor != nullptr);
				this->destructor(this->object);
			}

			this->object = nullptr;
			this->destructor = nullptr;
		}

		template<class T>
		static any wrap(T&& object) {
			any result{};
			result.emplace<T>(std::forward<T>(object));
			return result;
		}

		template<class T, class... Args>
		static any make(Args&&... args) {
			any result{};
			result.emplace<T>(std::forward<Args>(args)...);
			return result;
		}

		any() = default;
		~any() {
			this->destruct();
		}
	};

}
