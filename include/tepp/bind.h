#pragma once

#include "tepp/tepp.h"

namespace te
{
	namespace detail
	{
		struct uhole_ {};

		template<int HoleCount, int ArgCount, class List>
		struct enumerate_holes_args;

		template<int HoleCount, int ArgCount>
		struct enumerate_holes_args<HoleCount, ArgCount, list<>>
		{
			using type = list<>;
		};

		template<int HoleCount, int ArgCount, class Arg>
		struct enumerate_holes_args<HoleCount, ArgCount, list<Arg>>
		{
			using type = std::conditional_t<
				std::same_as<Arg, uhole_>,
				list<enumeration<Arg, HoleCount>>,
				list<enumeration<Arg, ArgCount>>
			>;
		};

		template<int HoleCount, int ArgCount, class Arg, class... Args>
		struct enumerate_holes_args<HoleCount, ArgCount, list<Arg, Args...>>
		{
			static constexpr int next_HoleCount = HoleCount + (std::same_as<Arg, uhole_> ? 1 : 0);
			static constexpr int next_ArgCount = ArgCount + (std::same_as<Arg, uhole_> ? 0 : 1);

			using next = typename enumerate_holes_args<next_HoleCount, next_ArgCount, list<Args...>>::type;
			using type = std::conditional_t<
				std::same_as<Arg, uhole_>,
				typename next::template prepend_t<enumeration<Arg, HoleCount>>,
				typename next::template prepend_t<enumeration<Arg, ArgCount>>
			>;
		};


		template<class I>
		struct Select;

		template<class I>
		requires std::same_as<typename I::type, uhole_>
			struct Select<I>
		{
			template<class Holes, class Args>
			constexpr static tuple_type_at_t<I::index, Holes>&& run(Holes&& holes, Args&& args) {
				return std::get<I::index>(std::forward<Holes>(holes));
			}
		};

		template<class I>
		requires (!std::same_as<typename I::type, uhole_>)
			struct Select<I>
		{
			template<class Holes, class Args>
			constexpr static tuple_type_at_t<I::index, Args>&& run(Holes&& holes, Args&& args) {
				return std::get<I::index>(std::forward<Args>(args));
			}
		};

		template<class Info2, class F, class Holes2, class Args2>
		struct Order2;

		template<class... Is, class F, class Holes2, class Args2>
		struct Order2<list<Is...>, F, Holes2, Args2>
		{
			constexpr static auto run(F f, Holes2&& holes, Args2&& args) {
				return std::invoke(f, Select<Is>::run(std::forward<Holes2>(holes), std::forward<Args2>(args))...);
			}
		};

		struct Order
		{
			template<class Info2, class F, class Holes2, class Args2>
			constexpr static auto run(F f, Holes2&& holes, Args2&& args) {
				return Order2<Info2, F, Holes2, Args2>::run(
					std::forward<F>(f),
					std::forward<Holes2>(holes),
					std::forward<Args2>(args)
				);
			}
		};

		template<class Infos>
		struct Bind2
		{
			template<class F, class... Args>
			constexpr static auto bind_mutable(F&& f, Args&&... args) {
				return[=, f = std::move(f)]<class... LArgs>(LArgs&&... hole) mutable {
					constexpr auto hole_count = filter_t<inspect_type_<is_<uhole_>>, Infos>::size;
					constexpr auto missing_holes = std::max(static_cast<int>(sizeof...(LArgs)) - hole_count, 0);

					using enumerated_list = typename enumerate_holes_args<hole_count, 0, replicate_t<missing_holes, uhole_>>::type;

					return Order::run<typename Infos::template append_t<enumerated_list>>(f, std::forward_as_tuple(hole...), std::forward_as_tuple(args...));
				};
			}

			template<class F, class... Args>
			constexpr static auto bind_const(F&& f, Args&&... args) {
				return[=, f = std::move(f)]<class... LArgs>(LArgs&&... hole) {
					constexpr auto hole_count = filter_t<inspect_type_<is_<uhole_>>, Infos>::size;
					constexpr auto missing_holes = std::max(static_cast<int>(sizeof...(LArgs)) - hole_count, 0);

					using enumerated_list = typename enumerate_holes_args<hole_count, 0, replicate_t<missing_holes, uhole_>>::type;

					return Order::run<typename Infos::template append_t<enumerated_list>>(f, std::forward_as_tuple(hole...), std::forward_as_tuple(args...));
				};
			}
		};
	}

	template<class F, class... Args>
	constexpr auto bind_const(F&& f, Args&&... args) {
		if constexpr (sizeof...(Args) == 0) {
			return f;
		}
		else {
			using enumerated_list = typename detail::enumerate_holes_args<0, 0, list<Args...>>::type;

			auto args_values = filter_tuple<not_<is_<detail::uhole_>>>::run(std::forward_as_tuple(args...));

			return std::apply([f]<class... As>(As&&... as) { return detail::Bind2<enumerated_list>::bind_const(f, std::forward<As>(as)...); }, args_values);
		}
	}

	template<class F, class... Args>
	constexpr auto bind_mutable(F&& f, Args&&... args) {
		if constexpr (sizeof...(Args) == 0) {
			return f;
		}
		else {
			using enumerated_list = typename detail::enumerate_holes_args<0, 0, list<Args...>>::type;

			auto args_values = filter_tuple<not_<is_<detail::uhole_>>>::run(std::forward_as_tuple(args...));

			return std::apply([f]<class... As>(As&&... as) { return detail::Bind2<enumerated_list>::bind_mutable(f, std::forward<As>(as)...); }, args_values);
		}
	}

	template<class F, class... Args>
	constexpr auto bind(F&& f, Args&&... args) {
		if (std::is_constant_evaluated()) {
			return bind_const(std::forward<F>(f), std::forward<Args>(args)...);
		}
		else {
			return bind_mutable(std::forward<F>(f), std::forward<Args>(args)...);
		}
	}

	using hole = detail::uhole_;
}