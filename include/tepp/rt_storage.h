// tepp - A template library for C++
// Copyright (C) 2022  Intrets
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

#include "intrusive_list.h"
#include "simple_vector.h"
#include "tepp.h"

#include <vector>
#include <variant>
#include <numeric>
#include <optional>

namespace te
{
	enum class rt_storage_tag
	{
		add,
		retrieveUpdates,
		replaceStorage,
	};

	template<class T>
	struct qualified
	{
		int64_t qualifier;
		T data;
	};

	template<class T, size_t size_, class IndexType = size_t, class... Extended>
	struct rt_storage
	{
		static constexpr size_t size = size_;

		struct FreeMemory
		{
			IndexType free;
		};

		struct Add
		{
			static constexpr auto tag = rt_storage_tag::add;
			qualified<T> data{};
			IndexType index{};

			intrusive_list_owned<FreeMemory> freeMemory;
		};

		struct RetrieveFrees
		{
			static constexpr auto tag = rt_storage_tag::retrieveUpdates;

			intrusive_list_owned<FreeMemory> frees;
		};

		struct Clear
		{
		};

		using Update = std::variant<Add, RetrieveFrees, Clear, Extended...>;

		struct nonrt
		{
			std::vector<IndexType> freeStorage{};
			std::array<int64_t, size> qualifiers{};
			int64_t qualifier = 1;

			std::vector<Update>* queue{};
			std::optional<qualified<IndexType>> add(T&& value) {
				if (this->freeStorage.empty()) {
					return std::nullopt;
				}

				auto newQualifier = this->qualifier++;
				auto index = this->freeStorage.back();
				this->freeStorage.pop_back();
				this->qualifiers[index] = newQualifier;

				this->queue->push_back(Add{
					.data = qualified<T>{
						.qualifier = newQualifier,
						.data = std::move(value)
					},
					.index = index,
					.freeMemory = intrusive_list_owned<FreeMemory>(new intrusive_list<FreeMemory>({ index }))
					});

				return qualified<IndexType>{
					.qualifier = newQualifier,
						.data = index
				};
			};

			void retrieveFrees() {
				this->addUpdate(RetrieveFrees{});
			}

			void clear() {
				this->addUpdate(Clear{});
			}

			template<te::member_of<Update> S>
			void addUpdate(S&& s) {
				this->queue->push_back(std::forward<S>(s));
			}

			using process_tag = void;
			void processUpdates(std::vector<Update>& updates) {
				for (auto& update : updates) {
					std::visit(overloaded{
						[&](RetrieveFrees& retrieveFrees) {
							retrieveFrees.frees.for_each(
								[&](auto freeMemory) {
									this->freeStorage.push_back(freeMemory.free);
								}
							);

						},
						[](auto&) {}
						},
						update);
				}
			}

			nonrt() {
				this->freeStorage.resize(size);
				std::iota(this->freeStorage.rbegin(), this->freeStorage.rend(), IndexType{});
			}
		};

		struct rt
		{
			std::array<qualified<T>, size> data;
			std::array<intrusive_list<FreeMemory>*, size> freeMemory;

			intrusive_list_owned<FreeMemory> freeMemoryQueue{};

			auto begin() {
				return this->data.begin();
			}

			auto end() {
				return this->data.end();
			}

			void processUpdates(std::vector<Update>& updates) {
				for (auto& update : updates) {
					std::visit(overloaded{
						[this](Add& add) {
							this->data[add.index] = std::move(add.data);
							assert(this->freeMemory[add.index] == nullptr);
							this->freeMemory[add.index] = add.freeMemory.release();
						},
						[this](RetrieveFrees& retrieveFrees) {
							assert(retrieveFrees.frees.empty());
							retrieveFrees.frees.data = this->freeMemoryQueue.release();
						},
						[this](Clear) {
							for (size_t i = 0; i < size; i++) {
								auto& entry = this->data[i];
								entry.qualifier = {};
								auto free = this->freeMemory[i];
								this->freeMemory[i] = nullptr;
								if (free != nullptr) {
									this->freeMemoryQueue.insert_before(free);
								}
							}
						},
						[](auto& extended) {
							extended.run();
						}
						},
						update);
				}
			}
		};
	};
}
