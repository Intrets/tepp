// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include <vector>

#include "tepp/integers.h"

namespace te
{
	template<class T>
	struct switched_vector
	{
	private:
		std::vector<T> data{};

		integer_t index = 0;

	public:
		switched_vector() = default;
		switched_vector(std::vector<T>&& es) {
			this->data = std::move(es);
		}
		switched_vector(std::vector<T> const& es) {
			this->data = es;
		}
		switched_vector(std::initializer_list<T> init)
		    : data{ init } {
		}
		switched_vector(T&& e) {
			this->add(std::forward<T>(e));
		}
		~switched_vector() = default;

		switched_vector& operator=(std::vector<T>&& es) {
			this->data = std::move(es);
			return *this;
		}

		T& get() {
			return this->data[this->index];
		}

		T const& get() const {
			return this->data[this->index];
		}

		void add(T&& e) {
			this->data.push_back(std::move(e));
		}

		integer_t size() const {
			return isize(this->data);
		}

		bool empty() const {
			return this->data.empty();
		}

		bool not_empty() const {
			return !this->empty();
		}

		void swap(integer_t i, integer_t j) {
			if (this->get_selected_index() == i) {
				this->select(j);
			}
			else if (this->get_selected_index() == j) {
				this->select(i);
			}

			std::swap(this->data[i], this->data[j]);
		}

		void erase(integer_t i) {
			this->data.erase(this->data.begin() + i);
			tassert(this->data.size() != 0);
			if (this->index == this->size() - 1) {
				this->index--;
			}
		}

		T extract(integer_t i) {
			auto result = std::move(this->data[i]);
			this->erase(i);
			return result;
		}

		void insert(integer_t i, T&& object) {
			if (this->index >= i) {
				this->index++;
			}
			this->data.insert(this->data.begin() + i, std::forward<T>(object));
		}

		template<class F>
		bool select_if(F&& f) {
			auto i = 0;
			for (auto& d : this->data) {
				if (f(d)) {
					this->index = i;
					return true;
				}

				i++;
			}

			return false;
		}

		bool select(std::integral auto i) {
			if (i < this->size()) {
				bool changed = this->index != i;
				this->index = i;
				return changed;
			}

			return false;
		}

		integer_t get_selected_index() const {
			return this->index;
		}

		void selectLast() {
			tassert(this->data.size() > 0);
			this->index = isize(this->data) - 1;
		}

		auto begin() const {
			return this->data.begin();
		}

		auto end() const {
			return this->data.end();
		}

		auto begin() {
			return this->data.begin();
		}

		auto end() {
			return this->data.end();
		}

		auto const& operator[](std::integral auto i) const {
			return this->data[i];
		}

		auto& operator[](std::integral auto i) {
			return this->data[i];
		}

		auto& advance() {
			if (!this->empty()) {
				this->index = (this->index + 1) % this->size();
			}

			return *this;
		}

		void clear() {
			this->data.clear();
			this->index = 0;
		}
	};
}
