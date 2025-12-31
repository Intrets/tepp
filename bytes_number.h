#pragma once

#include "tepp/integers.h"
#include <span>

#include <format>

namespace te
{
	struct bytes_number;

	struct KB
	{
		double value;
		operator bytes_number() const;
	};

	struct MB
	{
		double value;
		operator bytes_number() const;
	};

	struct GB
	{
		double value;
		operator bytes_number() const;
	};

	struct TB
	{
		double value;
		operator bytes_number() const;
	};

	struct bytes_number
	{
		integer_t value = 0;

		static constexpr integer_t kB_factor = 1024;
		static constexpr integer_t MB_factor = kB_factor * 1024;
		static constexpr integer_t GB_factor = MB_factor * 1024;
#if INTEGER_BITS >= 64
		static constexpr integer_t TB_factor = GB_factor * 1024;
#endif

		KB KB() const;
		MB MB() const;
		GB GB() const;
		TB TB() const;

		auto operator<=>(bytes_number const& other) const = default;

		bytes_number operator+(bytes_number other) const;
		bytes_number& operator+=(bytes_number other);
		bytes_number operator-(bytes_number other) const;
		bytes_number& operator-=(bytes_number other);

		static bytes_number get_size(auto const& e) {
			return { static_cast<integer_t>(std::span(e).size_bytes()) };
		}
	};
}

namespace std
{
	template<class C>
	struct formatter<te::bytes_number, C>
	{
		std::formatter<double, C> formatter;

		template<class ParseContext>
		constexpr ParseContext::iterator parse(ParseContext& ctx) {
			return this->formatter.parse(ctx);
		}

		template<class FmtContext>
		FmtContext::iterator format(te::bytes_number const& s, FmtContext& ctx) const {
			if (s.value < te::bytes_number::kB_factor) {
				return std::formatter<integer_t, C>{}.format(s.value, ctx);
			}
			else if (s.value < te::bytes_number::MB_factor) {
				auto it = this->formatter.format(s.KB().value, ctx);
				return std::format_to(it, " KB");
			}
			else if (s.value < te::bytes_number::GB_factor) {
				auto it = this->formatter.format(s.MB().value, ctx);
				return std::format_to(it, " MB");
			}
			else {
				auto it = this->formatter.format(s.GB().value, ctx);
				return std::format_to(it, " GB");
			}
		}
	};
}
