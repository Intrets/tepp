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
		};

		struct Add
		{
			T datum{};
		};

		struct Pop
		{
		};

		struct ReplaceStorage
		{
			std::vector<T> storage{};
			std::vector<T> tmp{};
		};

		using Update = std::variant<Move, Add, Pop, ReplaceStorage>;

		struct nonrt
		{
			size_t capacity = 10;
			size_t size = 0;

			std::vector<Update>* queue;
			void add(T value);
		};


		struct rt
		{
			std::vector<T> data{};
			size_t size = 0;

			rt() {
				data.resize(10);
			}

			void processUpdates(std::vector<Update>& updates) {
				for (auto& update : updates) {
					std::visit([this]<class S_>(S_ && update) {
						rand();
						using S = std::remove_cvref_t<S_>;
						if constexpr (std::same_as<S, Add>) {
							assert(this->size < this->data.size());
							this->data[this->size] = update.datum;
							this->size++;
						}
						else if constexpr (std::same_as<S, Move>) {
							assert(update.to < this->size);
							assert(update.from < this->size);
							this->data[update.to] = this->data[update.from];
						}
						else if constexpr (std::same_as<S, Pop>) {
							assert(this->size > 0);
							this->size--;
						}
						else if constexpr (std::same_as<S, ReplaceStorage>) {
							assert(this->size <= update.storage.size());

							std::copy_n(this->data.begin(), this->size, update.storage.begin());
							rand();

							update.tmp = std::move(this->data);
							this->data = std::move(update.storage);
							update.storage = std::move(update.tmp);
						}
					}, update);
				}
			}
		};
	};

	template<class T>
	inline void rt_pod_vector<T>::nonrt::add(T v) {
		std::vector<typename rt_pod_vector<T>::Update> updates;
		if (this->size == this->capacity) {
			this->capacity *= 2;

			ReplaceStorage replacement{};
			replacement.storage.resize(this->capacity);

			this->queue->push_back(std::move(replacement));
		}

		this->size++;
		this->queue->push_back(Add{ v });
	}
}
