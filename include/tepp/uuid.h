#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string_view>

namespace te
{
	struct uuid
	{
	private:
		std::array<uint8_t, 16> data;

	public:
		// 8-4-4-4-12 format: "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
		static std::optional<uuid> fromString(std::string_view str);

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
