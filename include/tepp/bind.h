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
		struct enumerate_holes_args<HoleCount, ArgCount, te::list<>>
		{
			using type = te::list<>;
		};

		template<int HoleCount, int ArgCount, class Arg>
		struct enumerate_holes_args<HoleCount, ArgCount, te::list<Arg>>
		{
			using type = std::conditional_t<
				std::same_as<Arg, uhole_>,
				te::list<te::enumeration<Arg, HoleCount>>,
				te::list<te::enumeration<Arg, ArgCount>>
			>;
		};

		template<int HoleCount, int ArgCount, class Arg, class... Args>
		struct enumerate_holes_args<HoleCount, ArgCount, te::list<Arg, Args...>>
		{
			static constexpr int next_HoleCount = HoleCount + (std::same_as<Arg, uhole_> ? 1 : 0);
			static constexpr int next_ArgCount = ArgCount + (std::same_as<Arg, uhole_> ? 0 : 1);

			using next = typename enumerate_holes_args<next_HoleCount, next_ArgCount, te::list<Args...>>::type;
			using type = std::conditional_t<
				std::same_as<Arg, uhole_>,
				typename next::template prepend_t<te::enumeration<Arg, HoleCount>>,
				typename next::template prepend_t<te::enumeration<Arg, ArgCount>>
			>;
		};

		struct Select
		{
			template<class I, class Holes, class Args>
			static auto run(Holes&& holes, Args&& args) {
				if constexpr (std::same_as<typename I::type, uhole_>) {
					return std::get<I::index>(std::forward<Holes>(holes));
				}
				else {
					return std::get<I::index>(std::forward<Args>(args));
				}
			}
		};

		struct Order
		{
			template<class Info2, class F, class Holes2, class Args2, class... Ordered>
			static auto run(F&& f, Holes2&& holes, Args2&& args, Ordered&&... ordered) {
				if constexpr (Info2::is_empty) {
					return f(ordered...);
				}
				else {
					using I = typename Info2::head;
					using Next = typename Info2::tail;

					return Order::run<Next>(
						std::forward<F>(f),
						std::forward<Holes2>(holes),
						std::forward<Args2>(args),
						std::forward<Ordered>(ordered)...,
						Select::run<I>(std::forward<Holes2>(holes), std::forward<Args2>(args))
						);
				}
			}
		};


		template<class F, class Info, class Args>
		struct Bind2;

		template<class F, class Infos, class... Args>
		struct Bind2<F, Infos, std::tuple<Args...>>
		{
			static auto bind(F const& f, std::tuple<Args...>&& args) {
				return[&, args = std::move(args)]<class... LArgs>(LArgs&&... hole) {
					constexpr auto hole_count = filter_t<inspect_type_t<is_<uhole_>>, Infos>::size;
					constexpr auto missing_holes = sizeof...(LArgs) - hole_count;
					using enumerated_list = typename enumerate_holes_args<hole_count, 0, replicate_t<missing_holes, uhole_>>::type;

					return Order::run<typename Infos::template append_t<enumerated_list>>(f, std::forward_as_tuple(hole...), args);
				};
			}
		};

		template<class F, class... Args>
		struct Bind
		{
			static auto bind(F f, Args&&... args) {
				using enumerated_list = typename enumerate_holes_args<0, 0, te::list<Args...>>::type;

				auto args_values = te::split_tuple::run<te::not_<te::is_<uhole_>>>(std::forward_as_tuple(args...));

				return Bind2<F, enumerated_list, decltype(args_values)>::bind(f, std::move(args_values));
			}
		};

	}

	template<class F, class... Args>
	auto bind(F f, Args&&... args) {
		using enumerated_list = typename detail::enumerate_holes_args<0, 0, te::list<Args...>>::type;

		auto args_values = te::split_tuple::run<te::not_<te::is_<detail::uhole_>>>(std::forward_as_tuple(args...));

		return detail::Bind2<F, enumerated_list, decltype(args_values)>::bind(f, std::move(args_values));
	}

	using hole = detail::uhole_;
}