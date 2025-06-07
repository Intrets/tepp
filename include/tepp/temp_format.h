#pragma once

#include <format>
#include <optional>
#include <vector>

#include "tepp/assert.h"
#include "tepp/cstring_view.h"
#include "tepp/misc.h"

namespace te
{
	namespace temp_format_buffer
	{
		struct Buffer
		{
			std::vector<char> buffer{};

			NO_COPY(Buffer);
			DEFAULT_MOVE(Buffer);

			Buffer() = default;
			~Buffer() = default;
		};

		struct BufferPool
		{
			std::vector<Buffer> bufferPool{};

			Buffer get() {
				if (this->bufferPool.empty()) {
					this->bufferPool.emplace_back();
				}

				auto result = std::move(this->bufferPool.back());
				this->bufferPool.pop_back();
				result.buffer.clear();

				return result;
			}

			void store(Buffer buffer) {
				this->bufferPool.push_back(std::move(buffer));
			}

			NO_COPY_MOVE(BufferPool);

			BufferPool() = default;
			~BufferPool() = default;
		};

		struct BufferAccess
		{
			BufferPool& bufferPool;
			std::optional<Buffer> buffer{};

			std::vector<char>* operator*() {
				return &this->buffer->buffer;
			}

			std::vector<char>* operator->() {
				return &this->buffer->buffer;
			}

			auto getInserter() {
				this->buffer->buffer.pop_back();
				return std::back_inserter(this->buffer->buffer);
			}

			operator te::cstring_view() const {
				tassert(this->buffer.has_value());
				return te::cstring_view(this->buffer->buffer.data(), isize(this->buffer->buffer) - 1);
			}

			operator std::string_view() const {
				tassert(this->buffer.has_value());
				return std::string_view(this->buffer->buffer.data(), isize(this->buffer->buffer) - 1);
			}

			te::cstring_view sv() const {
				return *this;
			}

			operator char const*() const {
				tassert(this->buffer.has_value());
				return this->buffer->buffer.data();
			}

			void append(std::string_view str) {
				std::ranges::copy(str, this->getInserter());
				this->buffer->buffer.push_back('\0');
			}

			template<class... Args>
			void append(std::format_string<Args...> const fmt, Args&&... args) {
				if constexpr (sizeof...(Args) == 0) {
					std::ranges::copy(fmt.get(), this->getInserter());
				}
				else {
					std::format_to(this->getInserter(), fmt, std::forward<Args>(args)...);
				}
				this->buffer->buffer.push_back('\0');
			}

			NO_COPY(BufferAccess);
			DEFAULT_MOVE(BufferAccess);

			BufferAccess(BufferPool& bufferPool_)
			    : bufferPool(bufferPool_),
			      buffer(bufferPool.get()) {
				this->buffer->buffer.push_back('\0');
			}

			~BufferAccess() {
				if (this->buffer.has_value()) {
					this->bufferPool.store(std::move(this->buffer.value()));
				}
			}
		};

		[[maybe_unused]] static BufferAccess getBuffer() {
			thread_local BufferPool bufferPool{};
			return BufferAccess(bufferPool);
		}
	}

	template<class... Args>
	static temp_format_buffer::BufferAccess temp_format(std::format_string<Args...> const fmt, Args&&... args) {
		auto buffer = temp_format_buffer::getBuffer();
		buffer.append(fmt, std::forward<Args>(args)...);
		return buffer;
	}
}
