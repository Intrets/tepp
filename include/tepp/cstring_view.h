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
		constexpr cstring_view(char const (&str)[N]);

		cstring_view(char const* str, size_t size_);
		cstring_view() = default;
		~cstring_view() = default;

		constexpr char const* getData() const {
			return this->data;
		};
	};

	template<size_t N>
	inline constexpr cstring_view::cstring_view(char const (&str)[N])
	    : data(str),
	      size(N) {
	}
}