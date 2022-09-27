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

		size_t index = 0;

	public:
		switched_vector() = delete;
		switched_vector(std::vector<T>&& es) {
			this->data = std::move(es);
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

		auto size() const {
			return this->data.size();
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

		void select(std::integral auto i) {
			if (i < this->size()) {
				this->index = i;
			}
		}

		auto get_selected_index() const{
			return this->index;
		}

		void selectLast() {
			assert(this->data.size() > 0);
			this->index = this->data.size() - 1;
		}
	};
}
