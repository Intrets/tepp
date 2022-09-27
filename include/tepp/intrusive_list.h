// tepp - a template library for c++
// Copyright (C) 2022 intrets

#pragma once

#include <utility>

#include "misc.h"

namespace te
{
	template<class T>
	struct intrusive_list;

	template<class T>
	struct intrusive_list_owned;

	template<class T>
	struct intrusive_list
	{
		using data_type = T;

		intrusive_list<T>* next = nullptr;
		intrusive_list<T>* previous = nullptr;

		T data{};

		template<class F>
		void for_each_forward(F&& f) {
			auto ptr = this;

			while (ptr != nullptr) {
				f(ptr->data);
				ptr = ptr->next;
			}
		}

		template<class F>
		void for_each_backward(F&& f) {
			auto ptr = this;

			while (ptr != nullptr) {
				f(ptr->data);
				ptr = ptr->previous;
			}
		}

		intrusive_list& back() {
			auto res = this;
			auto nextPtr = this->next;

			while (nextPtr != nullptr) {
				res = nextPtr;
				nextPtr = nextPtr->next;
			}

			return *res;
		}

		intrusive_list& front() {
			auto res = this;
			auto nextPtr = this->previous;

			while (nextPtr != nullptr) {
				res = nextPtr;
				nextPtr = nextPtr->previous;
			}

			return *res;
		}

		intrusive_list<T>* insert_after(intrusive_list<T>* l) {
			auto other = this->next;

			this->next = l;
			l->previous = this;

			auto& l2 = l->back();
			l2.next = other;

			if (other != nullptr) {
				other->previous = &l2;
			}

			return l;
		}

		intrusive_list<T>* insert_after(T&& t) {
			return this->insert_after(new intrusive_list<T>(std::forward<T>(t)));
		}

		intrusive_list<T>* insert_before(intrusive_list<T>* l) {
			auto other = this->previous;

			this->previous = l;
			l->next = this;

			auto& l2 = l->front();
			l2.previous = other;

			if (other != nullptr) {
				other->next = &l2;
			}

			return l;
		}

		intrusive_list<T>* insert_before(T&& t) {
			return this->insert_before(new intrusive_list<T>(std::forward<T>(t)));
		}

		intrusive_list() = default;
		intrusive_list(T&& t) : data(std::forward<T>(t)) {
		}
		DEFAULT_MOVE(intrusive_list);
		NO_COPY(intrusive_list);
		~intrusive_list() = default;
	};

	template<class T>
	struct intrusive_list_owned
	{
		intrusive_list<T>* data = nullptr;

		intrusive_list<T>* operator->() {
			return this->data;
		}

		template<class F>
		void for_each(F&& f) {
			if (this->data == nullptr) {
				return;
			}

			this->data->front().for_each_backward(std::forward<F>(f));
		}

		intrusive_list_owned<T>& insert_before(intrusive_list<T>* l) {
			if (this->data == nullptr) {
				this->data = l;
			}
			else {
				this->data = this->data->insert_before(l);
			}

			return *this;
		}

		bool empty() const {
			return this->data == nullptr;
		}

		intrusive_list<T>* release() {
			auto ptr = this->data;
			this->data = nullptr;
			return ptr;
		}

		intrusive_list_owned() = default;
		intrusive_list_owned(intrusive_list<T>* ptr) : data(ptr) {
		}
		intrusive_list_owned(intrusive_list_owned&& other) {
			this->data = other.data;
			other.data = nullptr;
		}
		intrusive_list_owned& operator=(intrusive_list_owned&& other) {
			if (this == &other) {
				return *this;
			}

			delete this->data;
			this->data = other.data;
			other.data = nullptr;
		}
		NO_COPY(intrusive_list_owned);
		~intrusive_list_owned() {
			if (this->data != nullptr) {
				auto next = this->data->next;
				auto previous = this->data->previous;

				delete this->data;

				while (next != nullptr) {
					auto p = next;
					next = next->next;

					delete p;
				}

				while (previous != nullptr) {
					auto p = previous;
					previous = previous->previous;

					delete p;
				}
			}
		}
	};
}
