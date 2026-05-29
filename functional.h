#pragma once

namespace te
{
	struct first_
	{
		auto& operator()(auto& pair) {
			return pair.first;
		}

		auto const& operator()(auto const& pair) {
			return pair.first;
		}
	};

	constexpr auto first = first_{};

	template<class F, class G>
	auto on2(F&& f, G&& g) {
		return [=](auto&& left, auto&& right) {
			return std::invoke(
			    f,
			    std::invoke(g, std::forward<decltype(left)>(left)),
			    std::invoke(g, std::forward<decltype(right)>(right))
			);
		};
	}

	template<class F>
	auto copy(F&& f) {
		return [=](auto&& val) -> std::remove_cvref_t<std::invoke_result_t<F, decltype(val)>> {
			return std::invoke(f, std::forward<decltype(val)>(val));
		};
	}

	template<class T>
	concept can_ref = requires(T t) {
		std::ref(t);
	};

	auto ref_if_possible(auto&& a) {
		if constexpr (can_ref<decltype(a)>) {
			return std::ref(a);
		}
		else {
			return a;
		}
	}

	template<class... Fs>
	auto extract(Fs&&... fs) {
		return [=](auto&& e) {
			return std::tuple<std::invoke_result_t<Fs, decltype(e)>...>(std::invoke(fs, e)...);
		};
	}
}
