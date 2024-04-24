// tepp - A template library for C++
// Copyright (C) 2022 intrets

#include <atomic>
#include <cassert>
#include <vector>

#include "simple_vector.h"
#include "tepp.h"
#include "variant.h"

namespace te
{
	template<class T, class S>
	concept has_rt_vector_run = requires(te::simple_vector<T> v, S s) { s.run(v); };

	enum class rt_vector_tag
	{
		swap,
		add,
		pop,
		set,
		replaceStorage,
		clear,
		overwriteClear,
		copy,
	};

	template<class T, rt_vector_tag tag>
	concept has_rt_vector_tag = requires(T t) { T::tag; } && (T::tag == tag);

	template<class T, class IndexType = size_t, class... Extended>
	struct rt_vector
	{
		struct Swap
		{
			static constexpr auto tag = rt_vector_tag::swap;

			IndexType from{};
			IndexType to{};

			T storage{};
		};

		struct Add
		{
			static constexpr auto tag = rt_vector_tag::add;

			T datum{};
		};

		struct Pop
		{
			static constexpr auto tag = rt_vector_tag::pop;

			T storage{};
		};

		struct ReplaceStorage
		{
			static constexpr auto tag = rt_vector_tag::replaceStorage;

			simple_vector<T> storage;

			ReplaceStorage(size_t size)
			    : storage(size) {
			}
		};

		struct Set
		{
			static constexpr auto tag = rt_vector_tag::set;

			simple_vector<T> storage;

			Set(simple_vector<T>&& storage_)
			    : storage(std::move(storage_)) {
			}
		};

		struct Clear
		{
			static constexpr auto tag = rt_vector_tag::clear;

			simple_vector<T> storage;

			Clear(size_t size)
			    : storage(size) {
			}
		};

		struct OverwriteClear
		{
			static constexpr auto tag = rt_vector_tag::overwriteClear;
		};

		struct Copy
		{
			static constexpr auto tag = rt_vector_tag::copy;

			simple_vector<T> storage;

			Copy(size_t size)
			    : storage(size) {
			}
		};

		using Update = te::variant<Swap, Add, Pop, Set, ReplaceStorage, Clear, OverwriteClear, Copy, Extended...>;

		struct nonrt
		{
			size_t capacity = 10;
			size_t size = 0;

			std::vector<Update>* queue{};
			void add(T&& value);
			void add(T const& value);
			void swap(IndexType from, IndexType to);
			void pop();
			void clear();
			void overwriteClear();
			void getCopy();
			void set(simple_vector<T>&& v);

			template<te::member_of<Update> S>
			void addUpdate(S&& s) {
				this->queue->push_back(std::forward<S>(s));
			}

			void processUpdates(std::vector<Update>& updates) {
			}
		};

		struct rt
		{
			simple_vector<T> data{ 10 };

			auto begin() {
				return this->data.begin();
			}

			auto end() {
				return this->data.begin() + this->data.size;
			}

			T& operator[](IndexType i) {
				return this->data[i];
			}

			T const& operator[](IndexType i) const {
				return this->data[i];
			}

			auto size() const {
				return this->data.size;
			}

			void processUpdates(std::vector<Update>& updates) {
				for (auto& update : updates) {
					te::visit(update, [this]<class S_>(S_&& update) {
						using S = std::remove_cvref_t<S_>;

						if constexpr (std::same_as<S, Add>) {
							this->data.add_back(std::move(update.datum));
						}
						else if constexpr (std::same_as<S, Swap>) {
							assert(update.to < this->data.size);
							assert(update.from < this->data.size);
							update.storage = std::move(this->data[update.to]);
							this->data[update.to] = std::move(this->data[update.from]);
							this->data[update.from] = std::move(update.storage);
						}
						else if constexpr (std::same_as<S, Pop>) {
							update.storage = std::move(this->data.pop());
						}
						else if constexpr (std::same_as<S, Set>) {
							std::swap(this->data, update.storage);
						}
						else if constexpr (std::same_as<S, ReplaceStorage>) {
							assert(this->data.size <= update.storage.capacity);
							update.storage.size = this->data.size;

							if constexpr (std::copyable<T>) {
								std::copy_n(this->data.begin(), this->data.size, update.storage.begin());
							}
							else {
								std::copy_n(std::move_iterator(this->data.begin()), this->data.size, update.storage.begin());
							}

							std::swap(this->data, update.storage);
						}
						else if constexpr (std::same_as<S, Clear>) {
							std::swap(this->data, update.storage);
						}
						else if constexpr (std::same_as<S, OverwriteClear>) {
							this->data.size = 0;
						}
						else if constexpr (std::same_as<S, Copy> && std::copyable<T>) {
							assert(this->data.size <= update.storage.capacity);
							std::copy_n(this->data.begin(), this->data.size, update.storage.begin());
							update.storage.size = this->data.size;
						}
						else if constexpr (has_rt_vector_run<T, S>) {
							update.run(this->data);
						}
						else {
						}
					});
				}
			}
		};
	};

	template<class T, class IndexType, class... Extended>
	inline void rt_vector<T, IndexType, Extended...>::nonrt::clear() {
		Clear c{ this->capacity };
		this->size = 0;
		this->queue->push_back(std::move(c));
	}

	template<class T, class IndexType, class... Extended>
	inline void rt_vector<T, IndexType, Extended...>::nonrt::overwriteClear() {
		this->size = 0;
		this->queue->push_back(OverwriteClear{});
	}

	template<class T, class IndexType, class... Extended>
	inline void rt_vector<T, IndexType, Extended...>::nonrt::getCopy() {
		static_assert(std::is_trivially_copyable_v<T>);
		Copy c{ this->size };
		this->queue->push_back(std::move(c));
	}

	template<class T, class IndexType, class... Extended>
	inline void rt_vector<T, IndexType, Extended...>::nonrt::set(simple_vector<T>&& v) {
		this->size = v.size;
		this->capacity = v.capacity;
		this->queue->push_back(Set(std::move(v)));
	}

	template<class T, class IndexType, class... Extended>
	inline void rt_vector<T, IndexType, Extended...>::nonrt::add(T&& v) {
		if (this->size == this->capacity) {
			this->capacity *= 2;

			ReplaceStorage replacement(this->capacity);

			this->queue->push_back(std::move(replacement));
		}

		this->size++;
		this->queue->push_back(Add{ std::move(v) });
	}

	template<class T, class IndexType, class... Extended>
	inline void rt_vector<T, IndexType, Extended...>::nonrt::add(T const& v) {
		if (this->size == this->capacity) {
			this->capacity *= 2;

			ReplaceStorage replacement(this->capacity);

			this->queue->push_back(std::move(replacement));
		}

		this->size++;
		this->queue->push_back(Add{ v });
	}

	template<class T, class IndexType, class... Extended>
	inline void rt_vector<T, IndexType, Extended...>::nonrt::swap(IndexType from, IndexType to) {
		this->queue->push_back(Swap{ .from = from, .to = to });
	}

	template<class T, class IndexType, class... Extended>
	inline void rt_vector<T, IndexType, Extended...>::nonrt::pop() {
		this->size--;
		this->queue->push_back(Pop{});
	}
}
