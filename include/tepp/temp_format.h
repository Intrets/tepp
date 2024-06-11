#pragma once

#include <format>
#include <vector>

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
				return std::back_inserter(this->buffer->buffer);
			}

			operator te::cstring_view() const {
				assert(this->buffer.has_value());
				return te::cstring_view(this->buffer->buffer.data(), isize(this->buffer->buffer) - 1);
			}

			operator char const*() const {
				assert(this->buffer.has_value());
				return this->buffer->buffer.data();
			}

			NO_COPY(BufferAccess);
			DEFAULT_MOVE(BufferAccess);

			BufferAccess(BufferPool& bufferPool_)
			    : bufferPool(bufferPool_),
			      buffer(bufferPool.get()) {
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
		std::format_to(buffer.getInserter(), fmt, std::forward<Args>(args)...);
		buffer->push_back('\0');
		return buffer;
	}
}
