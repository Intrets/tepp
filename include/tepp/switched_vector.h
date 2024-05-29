// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include <vector>

namespace te
{
	template<class T>
	struct switched_vector
	{
	private:
		std::vector<T> data{};

		int64_t index = 0;

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
			this->data.push_back(std::forward<T>(e));
		}

		int64_t size() const {
			return static_cast<int64_t>(this->data.size());
		}

		bool empty() const {
			return this->data.empty();
		}

		bool not_empty() const {
			return !this->empty();
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

		int64_t get_selected_index() const {
			return this->index;
		}

		void selectLast() {
			assert(this->data.size() > 0);
			this->index = this->data.size() - 1;
		}

		auto begin() const {
			return this->data.begin();
		}

		auto end() const {
			return this->data.begin();
		}

		auto begin() {
			return this->data.begin();
		}

		auto end() {
			return this->data.begin();
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
	};
}
