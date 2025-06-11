#pragma once

#include "tepp/integers.h"

namespace te
{
	namespace
	{
		struct KB
		{
			double value;
		};
		struct MB
		{
			double value;
		};
		struct GB
		{
			double value;
		};
		struct TB
		{
			double value;
		};
	}

	struct bytes_number
	{
		integer_t value = 0;

		static constexpr integer_t kB_factor = 1024;
		static constexpr integer_t MB_factor = kB_factor * 1024;
		static constexpr integer_t GB_factor = MB_factor * 1024;
		static constexpr integer_t TB_factor = GB_factor * 1024;

		KB KB() const {
			return { this->value / static_cast<double>(kB_factor) };
		}

		MB MB() const {
			return { this->value / static_cast<double>(MB_factor) };
		}

		GB GB() const {
			return { this->value / static_cast<double>(GB_factor) };
		}

		TB TB() const {
			return { this->value / static_cast<double>(TB_factor) };
		}

		bytes_number operator+(bytes_number other) const {
			return { this->value + other.value };
		}

		bytes_number& operator+=(bytes_number other) {
			this->value += other.value;
			return *this;
		}
	};
}

namespace std
{
	template<class C>
	struct std::formatter<te::bytes_number, C>
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
				this->formatter.format(s.KB().value, ctx);
				return std::ranges::copy(" KB", ctx.out()).out;
			}
			else if (s.value < te::bytes_number::GB_factor) {
				this->formatter.format(s.MB().value, ctx);
				return std::ranges::copy(" MB", ctx.out()).out;
			}
			else {
				this->formatter.format(s.GB().value, ctx);
				return std::ranges::copy(" GB", ctx.out()).out;
			}
		}
	};
}
