#pragma once

#include <future>
#include <tepp/optional.h>

namespace te
{
	template<class T>
	using future = std::future<T>;

	template<class T>
	using promise = std::promise<T>;

	inline bool get_future(future<void>& f) {
		if (f.valid() && f.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			try {
				f.get();
				return true;
			} catch (std::exception&) {
				return false;
			}
		}
		else {
			return false;
		}
	}

	template<class T>
	inline te::optional<T> get_future(future<T>& f) {
		if (f.valid() && f.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			try {
				return f.get();
			} catch (std::exception&) {
				return std::nullopt;
			}
		}
		else {
			return std::nullopt;
		}
	}
}
