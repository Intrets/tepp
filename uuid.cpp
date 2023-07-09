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
				byte << c;
			}

			byte >> result.data[i];

			i++;
		}

		return result;
	}

	bool uuid::operator==(uuid const& other) const {
		return data == other.data;
	}

	std::string uuid::toString() const {
		return "123";
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
