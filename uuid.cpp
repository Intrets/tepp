#include "tepp/uuid.h"

#include <cassert>
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

		int i = 0;
		bool first = true;
		for (auto c : str) {
			if (c == '-') {
				continue;
			}

			result.data[i] <<= 4;
			result.data[i] += parseChar(c);

			if (!first) {
				i++;
				first = true;
			}
			else {
				first = false;
			}
		}

		return result;
	}

	bool uuid::operator==(uuid const& other) const {
		static_assert(sizeof(other.data) == 16);
		static_assert(sizeof(this->data) == 16);
		return std::memcmp(this->data.data(), other.data.data(), 16) == 0;
	}

	uint64_t uuid::getLower() const {
		uint64_t result{};
		std::memcpy(&result, &this->data[0], sizeof(uint64_t));
		return result;
	}

	uint64_t uuid::getUpper() const {
		uint64_t result{};
		std::memcpy(&result, &this->data[8], sizeof(uint64_t));
		return result;
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
		constexpr uint64_t prime = 1099511628211ULL;
		constexpr uint64_t offset = 14695981039346656037ULL;

		uint64_t hash = offset;
		for (auto byte : this->data) {
			hash *= prime;
			hash ^= byte;
		}

		return static_cast<size_t>(hash);
	}
}
