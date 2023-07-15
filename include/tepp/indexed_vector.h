// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include "string_literal.h"

#include <vector>

namespace te
{
	template<string_literal name, class T = size_t>
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
