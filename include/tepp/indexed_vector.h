// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include "string_literal.h"

#include <vector>

namespace te
{
	template<string_literal name, class T = int64_t>
	struct index
	{
		T value = 0;

		index() = default;
		explicit index(T v)
		    : value(v) {
		}
		~index() = default;

		operator T() const {
			return this->value;
		}

		bool operator==(index other) const {
			return this->value == other.value;
		}

		bool operator<(index other) const {
			return this->value < other.value;
		}

		index& operator++() {
			++this->value;
			return *this;
		}

		index operator++(int) {
			return index(this->value++);
		}
	};

	template<class T, string_literal name>
	struct indexed_vector_pair_iterator
	{
		using iterator_category = std::random_access_iterator_tag;
		using value_type = std::pair<index<name>, T>;
		using difference_type = std::ptrdiff_t;
		using pointer = std::pair<index<name>, T*>;
		using reference = std::pair<index<name>, T&>;

	private:
		using wrapped_iterator_type = std::vector<T>::iterator;

		index<name> i;
		wrapped_iterator_type it;

	public:
		indexed_vector_pair_iterator(wrapped_iterator_type it_, index<name> i_) noexcept
		    : it(it_),
		      i(i_) {
		}

		indexed_vector_pair_iterator operator++(int) noexcept {
			auto r = *this;
			this->it++;
			this->i++;
			return r;
		}

		indexed_vector_pair_iterator& operator++() noexcept {
			this->it++;
			this->i++;
			return *this;
		}

		indexed_vector_pair_iterator operator--(int) noexcept {
			auto r = *this;
			this->it--;
			this->i--;
			return r;
		}

		indexed_vector_pair_iterator& operator--() noexcept {
			this->it--;
			this->i--;
			return *this;
		}

		bool operator==(indexed_vector_pair_iterator const& other) const noexcept {
			return this->it == other.it;
		}

		reference operator*() noexcept {
			return { this->i, *this->it };
		}
	};

	template<class, string_literal>
	struct indexed_vector;

	template<class T, string_literal name>
	struct pair_iterator_helper
	{
		indexed_vector<T, name>& vector;

		indexed_vector_pair_iterator<T, name> begin() {
			return indexed_vector_pair_iterator<T, name>(this->vector.begin(), index<name>(0));
		}

		indexed_vector_pair_iterator<T, name> end() {
			return indexed_vector_pair_iterator<T, name>(this->vector.end(), index<name>(this->vector.size()));
		}
	};

	template<class T_, string_literal name>
	struct indexed_vector
	{
	private:
		using T = std::conditional_t<std::same_as<T_, bool>, int8_t, T_>;

		using data_type = std::vector<T>;
		data_type data{};

	public:
		indexed_vector() = default;
		template<class S>
		indexed_vector(S const& other) {
			this->data.resize(other.size());
		}
		template<class S>
		indexed_vector(S const& other, T const& value) {
			this->data.resize(other.size(), value);
		}
		~indexed_vector() = default;

		indexed_vector empty_copy() const {
			indexed_vector result{};
			result.data.resize(this->size());
			return result;
		}

		T& operator[](index<name> index) {
			return this->data[index.value];
		}

		T const& operator[](index<name> index) const {
			return this->data[index.value];
		}

		index<name> size() const {
			return index<name>(this->data.size());
		};

		index<name> push_back(T const& v) {
			auto index = this->size();
			this->data.push_back(v);
			return index;
		}

		index<name> push_back(T&& v) {
			auto index = this->size();
			this->data.push_back(std::move(v));
			return index;
		}

		index<name> begin_index() const {
			return te::index<name>(0);
		}

		auto begin() {
			return this->data.begin();
		}

		auto begin() const {
			return this->data.begin();
		}

		auto end() {
			return this->data.end();
		}

		auto end() const {
			return this->data.end();
		}

		pair_iterator_helper<T, name> items() {
			return { .vector = *this };
		}

		void clear() {
			this->data.clear();
		}
	};
}

template<te::string_literal name, class T>
struct std::hash<te::index<name, T>>
{
	T operator()(te::index<name> const& index) const {
		return std::hash<T>{}(index.value);
	}
};
