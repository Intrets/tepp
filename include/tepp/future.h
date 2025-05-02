#pragma once

#include <future>
#include <tepp/optional.h>

namespace te
{
	template<class T>
	using future = std::future<T>;

	template<class T>
	using promise = std::promise<T>;

	template<class T>
	te::optional<T> get_future(future<T>& f) {
		if (f.valid() && f.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
			return f.get();
		}
		else {
			return std::nullopt;
		}
	}
}
