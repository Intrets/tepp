#pragma once

#include <tepp/integers.h>

#include <array>

namespace te
{
	namespace rc_object_detail
	{
		struct incomplete
		{
		};

		template<class T>
		struct storage
		{
			integer_t referenceCount = 0;
			integer_t referenceReferenceCount = 0;

			alignas(T) std::array<std::byte, sizeof(T)> storage;

			bool alive() const {
				return this->referenceCount != 0;
			}

			void destroy() {
				auto ptr = std::launder(reinterpret_cast<T*>(this->getMemory()));
				ptr->~T();
			}

			std::byte* getMemory() {
				return this->storage.data();
			}

			template<class... Args>
			void init(Args&&... args) {
				new (this->getMemory()) T(std::forward<Args>(args)...);
				this->referenceCount++;
			}

			T* get() {
				if (this->referenceCount > 0) {
					return std::launder(reinterpret_cast<T*>(this->getMemory()));
				}
				else {
					return nullptr;
				}
			}

			void incrementReferenceCount() {
				this->referenceCount++;
			}

			void incrementReferenceReferenceCount() {
				this->referenceReferenceCount++;
			}

			bool decrementReferenceCount() {
				assert(this->referenceCount > 0);
				this->referenceCount--;
				if (this->referenceCount == 0) {
					this->destroy();
				}

				return this->referenceCount == 0 && this->referenceReferenceCount == 0;
			}

			bool decrementReferenceReferenceCount() {
				assert(this->referenceReferenceCount > 0);
				this->referenceReferenceCount--;

				return this->referenceCount == 0 && this->referenceReferenceCount == 0;
			}
		};
	}

	template<class T, class S>
	concept exclude_self = !std::same_as<std::remove_cvref_t<T>, std::remove_cvref_t<S>>;

	template<class T>
	struct rc_object
	{
		rc_object_detail::storage<T>* storage;

		T* operator->() {
			return this->storage->get();
		}

		T const* operator->() const {
			return this->storage->get();
		}

		operator bool() const {
			return this->storage != nullptr && this->storage->alive();
		}

		void initStorage() {
			this->storage = new rc_object_detail::storage<T>;
		}

		rc_object(rc_object_detail::incomplete) {
		}

		rc_object(rc_object const& other) {
			this->storage = other.storage;
			if (this->storage != nullptr) {
				this->storage->incrementReferenceCount();
			}
		}

		rc_object(rc_object&& other) {
			this->storage = other.storage;
			other.storage = nullptr;
		}

		rc_object& operator=(rc_object const& other) {
			this->destroy();
			this->storage = other.storage;
			if (this->storage != nullptr) {
				this->storage->incrementReferenceCount();
			}

			return *this;
		}

		rc_object& operator=(rc_object&& other) {
			this->destroy();
			this->storage = other.storage;

			return *this;
		}

		template<exclude_self<rc_object<T>>... Args>
		rc_object(Args&&... args) {
			this->initStorage();
			this->storage->init(std::forward<Args>(args)...);
		}

		void destroy() {
			if (this->storage != nullptr) {
				if (this->storage->decrementReferenceCount()) {
					delete this->storage;
					this->storage = nullptr;
				}
			}
		}

		~rc_object() {
			this->destroy();
		}
	};

	template<class T>
	struct rc_object_ref
	{
		rc_object_detail::storage<T>* storage = nullptr;

		rc_object<T> access() {
			rc_object<T> result{ rc_object_detail::incomplete{} };
			result.storage = storage;
			if (result.storage != nullptr) {
				result.storage->incrementReferenceCount();
			}
			return result;
		}

		rc_object_ref(rc_object<T> const& other) {
			this->storage = other.storage;
			if (this->storage != nullptr) {
				this->storage->incrementReferenceReferenceCount();
			}
		}

		rc_object_ref() {
		}

		rc_object_ref(rc_object_ref const& other) {
			this->storage = other.storage;
			if (this->storage != nullptr) {
				this->storage->incrementReferenceReferenceCount();
			}
		}

		rc_object_ref(rc_object_ref&& other) {
			this->storage = other.storage;
			other.storage = nullptr;
		}

		rc_object_ref& operator=(rc_object_ref const& other) {
			this->destroy();
			this->storage = other.storage;
			if (this->storage != nullptr) {
				this->storage->incrementReferenceReferenceCount();
			}

			return *this;
		}

		rc_object_ref& operator=(rc_object_ref&& other) {
			this->destroy();
			this->storage = other.storage;
			other.storage = nullptr;

			return *this;
		}

		void destroy() {
			if (this->storage != nullptr) {
				if (this->storage->decrementReferenceReferenceCount()) {
					delete this->storage;
					this->storage = nullptr;
				}
			}
		}

		~rc_object_ref() {
			this->destroy();
		}
	};
}
