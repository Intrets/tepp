// tepp - A template library for C++
// Copyright (C) 2022 intrets

#pragma once

#include "optional_ref.h"

#include <type_traits>
#include <unordered_map>

namespace te
{
	struct bi_map_value
	{
	};

	struct bi_map_reference
	{
	};

	template<class K1, class K2, class LeftType = bi_map_value, class RightType = bi_map_value>
	struct bi_map
	{
		template<bool b, class T>
		struct WrappedValue
		{
			using value_type = std::conditional_t<b, T, T const*>;

			value_type value;

			template<std::same_as<T> S>
			    requires(!b)
			WrappedValue& operator=(S const* value_) {
				this->value = value_;
				return *this;
			}

			T& get() {
				if constexpr (b) {
					return value;
				}
				else {
					return *value;
				}
			}

			T const& get() const {
				if constexpr (b) {
					return value;
				}
				else {
					return *value;
				}
			}
		};

		static constexpr auto is_value1 = std::same_as<LeftType, bi_map_value>;
		static constexpr auto is_value2 = std::same_as<RightType, bi_map_value>;
		using V1 = WrappedValue<is_value1, K1>;
		using V2 = WrappedValue<is_value2, K2>;

		std::unordered_map<K1, V2> left;
		std::unordered_map<K2, V1> right;

		te::optional_ref<K1 const> getRight(K2&& k2);
		te::optional_ref<K2 const> getLeft(K1&& k1);

		te::optional_ref<K1 const> getRight(K2 const& k2);
		te::optional_ref<K2 const> getLeft(K1 const& k1);

		void removeLeft(K1&& k1);
		void removeRight(K2&& k2);

		bool insert(K1 const& k1, K2 const& k2);
		bool insert(K1&& k1, K2&& k2);
	};

	template<class K1, class K2, class LeftType, class RightType>
	inline te::optional_ref<K1 const> bi_map<K1, K2, LeftType, RightType>::getRight(K2 const& k2) {
		auto it = this->right.find(k2);

		if (it != this->right.end()) {
			return it->second.get();
		}
		else {
			return te::nullopt;
		}
	}

	template<class K1, class K2, class LeftType, class RightType>
	inline te::optional_ref<K1 const> bi_map<K1, K2, LeftType, RightType>::getRight(K2&& k2) {
		auto it = this->right.find(k2);

		if (it != this->right.end()) {
			return it->second.get();
		}
		else {
			return te::nullopt;
		}
	}

	template<class K1, class K2, class LeftType, class RightType>
	inline te::optional_ref<K2 const> bi_map<K1, K2, LeftType, RightType>::getLeft(K1 const& k1) {
		auto it = this->left.find(k1);

		if (it != this->left.end()) {
			return it->second.get();
		}
		else {
			return te::nullopt;
		}
	}

	template<class K1, class K2, class LeftType, class RightType>
	inline te::optional_ref<K2 const> bi_map<K1, K2, LeftType, RightType>::getLeft(K1&& k1) {
		auto it = this->left.find(k1);

		if (it != this->left.end()) {
			return it->second.get();
		}
		else {
			return te::nullopt;
		}
	}

	template<class K1, class K2, class LeftType, class RightType>
	inline void bi_map<K1, K2, LeftType, RightType>::removeLeft(K1&& k1) {
#ifdef DEBUG_BUILD
		if (auto l = this->getLeft(std::forward<K1>(k1))) {
			tassert(this->right.contains(l.value()));
		}
#endif

		auto it = this->left.find(k1);

		if (it != this->left.end()) {
			this->right.erase(it->second.get());
		}

		this->left.erase(it);
	}

	template<class K1, class K2, class LeftType, class RightType>
	inline void bi_map<K1, K2, LeftType, RightType>::removeRight(K2&& k2) {
#ifdef DEBUG_BUILD
		if (auto l = this->getRight(std::forward<K2>(k2))) {
			tassert(this->left.contains(l.value()));
		}
#endif

		auto it = this->right.find(k2);

		if (it != this->right.end()) {
			this->left.erase(it->second.get());
		}

		this->right.erase(it);
	}

	template<class K1, class K2, class LeftType, class RightType>
	inline bool bi_map<K1, K2, LeftType, RightType>::insert(K1 const& k1, K2 const& k2) {
		auto&& [it1, b1] = this->left.insert({ k1, {} });
		if (!b1) {
			return false;
		}

		auto&& [it2, b2] = this->right.insert({ k2, {} });
		if (!b2) {
			this->left.erase(k1);
			return false;
		}

		 if constexpr (is_value1) {
			it2->second.value = it1->first;
		 }
		 else {
			it2->second = &it1->first;
		 }

		 if constexpr (is_value2) {
			it1->second.value = it2->first;
		 }
		 else {
			it1->second = &it2->first;
		 }

		return true;
	}

	template<class K1, class K2, class LeftType, class RightType>
	inline bool bi_map<K1, K2, LeftType, RightType>::insert(K1&& k1, K2&& k2) {
		auto&& [it1, b1] = this->left.insert({ k1, {} });
		if (!b1) {
			return false;
		}

		auto&& [it2, b2] = this->right.insert({ k2, {} });
		if (!b2) {
			this->left.erase(k1);
			return false;
		}

		if constexpr (is_value1) {
			it2->second.value = it1->first;
		}
		else {
			it2->second = &it1->first;
		}

		if constexpr (is_value2) {
			it1->second.value = it2->first;
		}
		else {
			it1->second = &it2->first;
		}

		return true;
	}
}
