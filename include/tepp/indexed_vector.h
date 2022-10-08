// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include "string_literal.h"

#include <vector>

namespace te
{
	template<string_literal name>
	struct index
	{
		size_t value = 0;

		index() = default;
		explicit index(size_t v) : value(v) {
		}
		~index() = default;

		operator size_t() const {
			return this->value;
		}

		bool operator==(index other) const {
			return this->value == other.value;
		}

		bool operator<(index other) const {
			return this->value < other.value;
		}

		index& operator++() {
			return index(++this->value);
		}

		index operator++(int) {
			return index(this->value++);
		}
	};

	template<class T_, string_literal name>
	struct indexed_vector
	{
	private:
		using T = std::conditional_t<std::same_as<T_, bool>, int, T_>;

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
	};
}

template<te::string_literal name>
struct std::hash<te::index<name>>
{
	size_t operator()(te::index<name> const& index) const {
		return std::hash<size_t>{}(index.value);
	}
};
