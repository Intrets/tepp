#pragma once

#include <cassert>
#include <string>
#include <string_view>

namespace te
{
	template<class T>
	struct const_string_view
	{
	private:
		T const* data{};
		int64_t size{};

	public:
		bool empty() const {
			return this->size == 0;
		}

		std::string_view string_view() const {
			return *this;
		}

		operator std::string_view() const {
			if (size > 0) {
				return std::string_view(data, size - 1);
			}
			else {
				return std::string_view{};
			}
		}

		template<class S, int64_t N>
		constexpr const_string_view(S const (&str)[N])
		    : data(str),
		      size(N) {
		}

		std::string string() const {
			return std::string(this->string_view());
		}

		const_string_view(std::string const& str)
		    : data(str.data()),
		      size(str.size()) {
		}

		const_string_view(char const* str, int64_t size_)
		    : data(str),
		      size(size_) {
			assert(str[size_] == '\0');
		}

		const_string_view& set(std::string const& str) {
			this->data = str.data();
			this->size = str.size();

			return *this;
		}

		const_string_view() = default;
		~const_string_view() = default;

		constexpr char const* getData() const {
			return reinterpret_cast<char const*>(this->data);
		};
	};

	using cstring_view = const_string_view<char>;
	using cu8string_view = const_string_view<char8_t>;
}

bool operator==(te::cstring_view left, std::string const& right);

template<>
struct std::formatter<te::cstring_view, char>
{
	template<class ParseContext>
	constexpr ParseContext::iterator parse(ParseContext& ctx) {
		return std::formatter<std::string_view, char>{}.parse(ctx);
	}

	template<class FmtContext>
	FmtContext::iterator format(te::cstring_view s, FmtContext& ctx) const {
		return std::formatter<std::string_view, char>{}.format(s.string_view(), ctx);
	}
};
