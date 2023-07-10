#include "tepp/uuid.h"

#include <ranges>
#include <sstream>

namespace te
{
	std::optional<uuid> uuid::fromString(std::string_view str) {
		using namespace std::literals;

		if (str.size() != "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"sv.size()) {
			return std::nullopt;
		}

		uuid result{};

		for (int i = 0; auto s : str | std::views::filter([](auto c) { return c != '-'; }) | std::views::chunk(2)) {
			std::stringstream byte{};
			for (auto c : s) {
				result.data[i] <<= 4;
				result.data[i] += parseChar(c);
			}

			i++;
		}

		return result;
	}

	bool uuid::operator==(uuid const& other) const {
		return data == other.data;
	}

	std::string uuid::toString() const {
		std::stringstream result{};

		auto constexpr convertToChar = [](uint8_t c) -> char {
			if (c <= 9) {
				return c + '0';
			}
			else {
				return c - 10 + 'a';
			}
		};

		auto writeByte = [&](auto i) {
			auto c = this->data[i];
			result.put(convertToChar(c >> 4));
			result.put(convertToChar(c & 0xF));
		};

		writeByte(0);
		writeByte(1);
		writeByte(2);
		writeByte(3);

		result << '-';

		writeByte(4);
		writeByte(5);

		result << '-';

		writeByte(6);
		writeByte(7);

		result << '-';

		writeByte(8);
		writeByte(9);

		result << '-';

		writeByte(10);
		writeByte(11);
		writeByte(12);
		writeByte(13);
		writeByte(14);
		writeByte(15);

		return result.str();
	}

	std::size_t uuid::getHash() const {
		// FNV-1
		constexpr size_t prime = 1099511628211ULL;
		constexpr size_t offset = 14695981039346656037ULL;

		size_t hash = offset;
		for (auto byte : this->data) {
			hash *= prime;
			hash ^= byte;
		}

		return hash;
	}
}
