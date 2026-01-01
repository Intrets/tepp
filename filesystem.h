#pragma once

#include <format>
#include <filesystem>
#include <string_view>

namespace te
{
}

template<class C>
struct std::formatter<std::filesystem::path, C>
{
	template<class ParseContext>
	constexpr ParseContext::iterator parse(ParseContext& ctx) {
		return std::formatter<std::string_view, C>{}.parse(ctx);
	}

	template<class FmtContext>
	FmtContext::iterator format(std::filesystem::path const& p, FmtContext& ctx) const {
		try {
			auto s = p.generic_string();
			return std::formatter<std::string_view, C>{}.format(std::string_view(s), ctx);
		} catch (std::exception&) {
			auto text = std::string_view("<failed to convert path to string>");
			return std::formatter<std::string_view, C>{}.format(text, ctx);
		}
	}
};
