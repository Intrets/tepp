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

#include <vector>
#include <variant>
#include <atomic>
#include <cassert>

namespace te
{
	// No heap allocation in objects T
	template<class T>
	struct rt_pod_vector
	{
		struct Move
		{
			size_t from{};
			size_t to{};

			T storage{};
		};

		struct Add
		{
			T datum{};
		};

		struct Pop
		{
			T storage{};
		};

		struct ReplaceStorage
		{
			std::vector<T> storage{};
			std::vector<T> tmp{};
		};

		struct Clear
		{
			std::vector<T> storage{};
			std::vector<T> tmp{};
		};

		using Update = std::variant<Move, Add, Pop, ReplaceStorage, Clear>;

		struct nonrt
		{
			size_t capacity = 10;
			size_t size = 0;

			std::vector<Update>* queue{};
			void add(T&& value);
			void clear();
		};


		struct rt
		{
			std::vector<T> data{};
			size_t size = 0;

			rt() {
				data.resize(10);
			}

			auto begin() {
				return this->data.begin();
			}

			auto end() {
				return this->data.begin() + this->size;
			}

			void processUpdates(std::vector<Update>& updates) {
				for (auto& update : updates) {
					std::visit([this]<class S_>(S_ && update) {
						using S = std::remove_cvref_t<S_>;
						if constexpr (std::same_as<S, Add>) {
							assert(this->size < this->data.size());
							this->data[this->size] = std::move(update.datum);
							this->size++;
						}
						else if constexpr (std::same_as<S, Move>) {
							assert(update.to < this->size);
							assert(update.from < this->size);
							if constexpr (std::is_trivial_v<T>) {
								this->data[update.to] = std::move(this->data[update.from]);
							}
							else {
								update.storage = std::move(this->data[update.to]);
								this->data[update.to] = std::move(this->data[update.from]);
							}
						}
						else if constexpr (std::same_as<S, Pop>) {
							assert(this->size > 0);
							if constexpr (std::is_trivial_v<T>) {
								this->size--;
							}
							else {
								this->size--;
								update.storage = std::move(this->data[this->size]);
							}
						}
						else if constexpr (std::same_as<S, ReplaceStorage>) {
							assert(this->size <= update.storage.size());

							if constexpr (std::copyable<T>) {
								std::copy_n(this->data.begin(), this->size, update.storage.begin());
							}
							else {
								std::copy_n(std::move_iterator(this->data.begin()), this->size, update.storage.begin());
							}

							update.tmp = std::move(this->data);
							this->data = std::move(update.storage);
							update.storage = std::move(update.tmp);
						}
						else if constexpr (std::same_as<S, Clear>) {
							this->size = 0;

							if constexpr (!std::is_trivial_v<T>) {
								update.tmp = std::move(this->data);
								this->data = std::move(update.storage);
							}
						}
					}, update);
				}
			}
		};
	};

	template<class T>
	inline void rt_pod_vector<T>::nonrt::clear() {
		Clear c{};
		c.storage.resize(10);
		this->queue->push_back(std::move(c));
		this->size = 0;
		this->capacity = 10;
	}

	template<class T>
	inline void rt_pod_vector<T>::nonrt::add(T&& v) {
		std::vector<typename rt_pod_vector<T>::Update> updates;
		if (this->size == this->capacity) {
			this->capacity *= 2;

			ReplaceStorage replacement{};
			replacement.storage.resize(this->capacity);

			this->queue->push_back(std::move(replacement));
		}

		this->size++;
		this->queue->push_back(Add{ std::move(v) });
	}
}
