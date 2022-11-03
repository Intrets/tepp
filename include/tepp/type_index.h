#pragma once

namespace te
{
	template<class T, class index_type = int>
	struct type_index
	{
	private:
		static index_type& type_counter() {
			static index_type counter = 0;
			return counter;
		};

	public:
		template<class S>
		static int get() {
			static int index = type_counter()++;
			return index;
		}
	};
}