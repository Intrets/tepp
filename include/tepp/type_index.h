#pragma once

#include <atomic>

namespace te
{
	template<class T, class index_type = integer_t>
	struct type_index
	{
	private:
		static std::atomic<index_type>& type_counter() {
			static std::atomic<index_type> counter = 0;
			return counter;
		};

	public:
		template<class S>
		static index_type get() {
			static index_type index = type_counter().fetch_add(1);
			return index;
		}
	};
}