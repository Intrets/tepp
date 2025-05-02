#pragma once

#include <tepp/misc.h>

namespace te
{
	struct duration
	{
		int64_t data = 0;

		static duration s(int64_t count);
		static duration ms(int64_t count);
		static duration us(int64_t count);
		static duration ns(int64_t count);
	};

	struct time_point
	{
		int64_t data = 0;

		bool after(time_point other) const;
		bool before(time_point other) const;
		bool after_or_equal(time_point other) const;
		bool before_or_equal(time_point other) const;

		bool elapsed(duration d) const;

		duration duration_to(duration other) const;
		time_point add(duration d) const;

		time_point& set_now();

		static time_point now();
	};
}
