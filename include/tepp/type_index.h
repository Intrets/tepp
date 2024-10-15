#pragma once

#include <atomic>

namespace te
{
	template<class T, class index_type = int>
	struct type_index
	{
	private:
		static index_type& type_counter() {
			static std::atomic<index_type> counter = 0;
			return counter;
		};

	public:
		template<class S>
		static int get() {
			static int index = type_counter().fetch_add(1);
			return index;
		}
	};
}