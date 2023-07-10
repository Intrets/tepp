#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string_view>

namespace te
{
	struct uuid
	{
		std::array<uint8_t, 16> data{};

		// 8-4-4-4-12 format: "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
		static std::optional<uuid> fromString(std::string_view str);

		constexpr static auto parseChar = [](char c) -> uint8_t {
			if (c <= '9') {
				return c - '0';
			}
			else if (c <= 'H') {
				return 10 + c - 'A';
			}
			else {
				return 10 + c - 'a';
			}
		};
		constexpr static auto parseByte = [](char c1, char c2) -> uint8_t {
			return parseChar(c1) * 16 + parseChar(c2);
		};

		constexpr uuid() = default;
		constexpr uuid(char const (&str)[37]) {
			if (str[8] == '-' && str[13] == '-' && str[18] == '-' && str[23] == '-') {
				data[0] = parseByte(str[0], str[1]);
				data[1] = parseByte(str[2], str[3]);
				data[2] = parseByte(str[4], str[5]);
				data[3] = parseByte(str[6], str[7]);

				data[4] = parseByte(str[9], str[10]);
				data[5] = parseByte(str[11], str[12]);

				data[6] = parseByte(str[14], str[15]);
				data[7] = parseByte(str[16], str[17]);

				data[8] = parseByte(str[19], str[20]);
				data[9] = parseByte(str[21], str[22]);

				data[10] = parseByte(str[24], str[25]);
				data[11] = parseByte(str[26], str[27]);
				data[12] = parseByte(str[28], str[29]);
				data[13] = parseByte(str[30], str[31]);
				data[14] = parseByte(str[32], str[33]);
				data[15] = parseByte(str[34], str[35]);
			}
		};
		~uuid() = default;

		std::string toString() const;

		bool operator==(uuid const& other) const;

		std::size_t getHash() const;
	};
}

template<>
struct std::hash<te::uuid>
{
	std::size_t operator()(te::uuid const& uuid) {
		return uuid.getHash();
	}
};
