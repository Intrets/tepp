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

#include <atomic>
#include <vector>
#include <variant>

#include <tepp/tepp.h>

namespace te
{
	template<class T>
	struct rt_value
	{
		struct Retrieve
		{
			T storage{};
		};

		struct Send
		{
			T storage{};
		};

		using Update = std::variant<Retrieve, Send>;

		struct rt
		{
			T value{};

			void processUpdates(std::vector<Update>& updates) {
				for (auto& update : updates) {
					std::visit([this]<class S_>(S_ && update) {
						using S = std::remove_cvref_t<S_>;

						if constexpr (std::same_as<S, Send>) {
							this->value = update.storage;
						}
						else if constexpr (std::same_as<S, Retrieve>) {
							update.storage = this->value;
						}
					}, update);
				}
			}
		};

		struct nonrt
		{
			T value{};

			size_t capacity = 10;
			size_t size = 0;

			std::vector<Update>* queue{};
			void clear();

			template<te::member_of<Update> S>
			void addUpdate(S&& s) {
				this->queue->push_back(std::forward<S>(s));
			}

			void send(T&& t) {
				this->addUpdate(Send{ std::forward<T>(t) });
			}

			void send(T const& t) {
				this->addUpdate(Send{ t });
			}

			void retrieve() {
				this->addUpdate(Retrieve{});
			}

			using process_tag = void;
			void processUpdates(std::vector<Update>& updates) {
				for (auto& update : updates) {
					std::visit([this]<class S_>(S_ && update) {
						using S = std::remove_cvref_t<S_>;

						if constexpr (std::same_as<S, Retrieve>) {
							this->value = update.storage;
						}
					}, update);
				}
			}
		};
	};

	template<class T>
	inline void rt_value<T>::nonrt::clear() {
		this->addUpdate(Send());
	}
}
