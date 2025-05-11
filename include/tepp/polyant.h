#pragma once

#include <utility>

#include <tepp/optional_ref.h>
#include <tepp/tepp.h>
#include <tepp/variant.h>

namespace te
{
	template<class B, class First, class... Args>
	struct polyant
	{
	private:
		te::variant<First, Args...> storage{};
		B* base = nullptr;

		B* getBaseImpl() {
			return std::visit([](auto& v) { return static_cast<B*>(&v); }, this->storage);
		}

	public:
		B& getBase() {
			return *this->base;
		}

		B const& getBase() const {
			return *this->base;
		}

		B* operator->() {
			return &this->getBase();
		}

		B const* operator->() const {
			return &this->getBase();
		}

		template<te::member_of2<First, Args...> T>
		te::optional_ref<T> getIf() {
			if (auto ptr = std::get_if<T>(&this->storage)) {
				return *ptr;
			}
			else {
				return te::nullopt;
			}
		}

		template<class... F>
		auto visit(F&&... f) {
			return te::visit(this->storage, std::forward<F>(f)...);
		}

		template<class... F>
		auto visit(F&&... f) const {
			return te::visit(this->storage, std::forward<F>(f)...);
		}

	private:
		void move_from(polyant&& other) {
			this->storage = std::move(other.storage);
			this->base = this->getBaseImpl();
		}

	public:
		polyant(polyant&& other) {
			this->move_from(std::move(other));
		}

		polyant& operator=(polyant&& other) {
			this->move_from(std::move(other));

			return *this;
		}

	private:
		void copy_from(polyant const& other) {
			this->storage = other.storage;
			this->base = this->getBaseImpl();
		}

	public:
		polyant(polyant const& other) {
			this->copy_from(other);
		}

		polyant& operator=(polyant const& other) {
			this->copy_from(other);

			return *this;
		}

		template<te::member_of2<First, Args...> T, class... TArgs>
		T& emplace(TArgs&&... targs) noexcept(noexcept(T(std::forward<TArgs>(targs)...))) {
			this->storage.emplace<T>(std::forward<TArgs>(targs)...);
			T& object = std::get<T>(this->storage);
			this->base = &object;

			return object;
		}

		template<te::member_of2<First, Args...> T>
		polyant(T&& other) noexcept(noexcept(this->emplace<T>(std::forward<T>(other)))) {
			this->emplace<T>(std::forward<T>(other));
		}

		template<te::member_of2<First, Args...> T>
		polyant(T const& other) noexcept(noexcept(this->emplace<T>(other))) {
			this->emplace<T>(other);
		}

		template<te::member_of2<First, Args...> T>
		polyant& operator=(T&& other) {
			this->emplace<T>(std::forward<T>(other));

			return *this;
		}

		template<te::member_of2<First, Args...> T>
		polyant& operator=(T const& other) {
			this->emplace<T>(other);

			return *this;
		}

		polyant() {
			this->emplace<First>();
		}

		~polyant() = default;
	};
}
