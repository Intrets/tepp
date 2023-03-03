#pragma once

namespace te
{
	struct cstring_view
	{
	private:
		char const* data{};
		size_t size{};

	public:
		template<size_t N>
		cstring_view(char const (&str)[N]);

		cstring_view() = default;
		~cstring_view() = default;

		char const* getData() const;
	};

	template<size_t N>
	inline cstring_view::cstring_view(char const (&str)[N])
	    : data(str),
	      size(N) {
	}
}