#pragma once

#include <memory_resource>
#include <new>
#include <vector>

#include "tepp/misc.h"

namespace te
{
	template<class T>
	struct pmr_container
	{
		alignas(T) std::byte data[sizeof(T)];

		void newFrame(std::pmr::memory_resource& resource) {
			new (this->data) T(&resource);
		}

		void endFrame() {
			this->get().~T();
		}

		T& get() {
			return *std::launder<T>(reinterpret_cast<T*>(this->data));
		}

		T const& get() const {
			return *std::launder<T const>(reinterpret_cast<T const*>(this->data));
		}

		T* operator->() {
			return &this->get();
		}

		T const* operator->() const {
			return &this->get();
		}

		pmr_container() = default;
		~pmr_container() = default;

		NO_COPY_MOVE(pmr_container);
	};

	namespace pmr
	{
		template<class T>
		using vector = pmr_container<std::vector<T, std::pmr::polymorphic_allocator<T>>>;
	}
}
