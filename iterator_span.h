#pragma once

namespace te
{
	template<class T>
	struct iterator_span
	{
		T begin_;
		T end_;

		T begin() {
			return this->begin_;
		}

		T end() {
			return this->end_;
		}

		iterator_span() = delete;
		iterator_span(T begin__, T end__)
		    : begin_(begin__),
		      end_(end__) {
		}

		~iterator_span() = default;
	};
}
