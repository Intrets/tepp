// tepp - a template library for c++
// Copyright (C) 2022 intrets

#pragma once

#include <utility>

namespace te
{
	template<class T>
	struct heap_object
	{
	private:
		T* object;

	public:
		T* get() {
			return this->object;
		}

		T* operator->() {
			return this->get();
		}

		operator T& () {
			return *this->object;
		}

		T* copy() const {
			return new T(this->object);
		}

		heap_object() : object(new T) {
		}

		template<class... Args>
		heap_object(Args&&... args) : object(new T(std::forward<Args>(args)...)) {
		}


		heap_object(heap_object const& other) {
			this->object = other.copy();
		}

		heap_object(heap_object&& other) {
			this->object = new T(std::move(*other.object));
		}


		heap_object& operator=(heap_object const& other) {
			*this->object = *other.object;
		}

		heap_object& operator=(heap_object&& other) {
			*this->object = std::move(*other.object);
		}


		~heap_object() {
			delete this->object;
		}
	};
}
