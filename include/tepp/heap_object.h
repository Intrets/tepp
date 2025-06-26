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

		T const* get() const {
			return this->object;
		}

		T& getObject() {
			return *this->get();
		}

		T const& getObject() const {
			return *this->get();
		}

		T* operator->() {
			return this->get();
		}

		T const* operator->() const {
			return this->get();
		}

		operator T&() {
			return *this->object;
		}

		operator T const&() const {
			return *this->object;
		}

		T& operator*() {
			return *this->object;
		}

		T const& operator*() const {
			return *this->object;
		}

		template<class... Args>
		requires(std::constructible_from<T, Args...>)
		heap_object(Args&&... args)
		    : object(new T(std::forward<Args>(args)...)) {
		}

		heap_object(heap_object const& other)
		    : object(new T(*other.object)) {
		}

		heap_object(heap_object&& other)
		    : object(new T(std::move(*other.object))) {
		}

		heap_object& operator=(T const& other) {
			*this->object = other;

			return *this;
		}

		heap_object& operator=(T&& other) {
			*this->object = std::move(other);

			return *this;
		}

		heap_object& operator=(heap_object const& other) {
			*this->object = *other.object;

			return *this;
		}

		heap_object& operator=(heap_object&& other) {
			*this->object = std::move(*other.object);

			return *this;
		}

		~heap_object() {
			delete this->object;
		}
	};
}
