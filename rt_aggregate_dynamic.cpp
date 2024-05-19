#include "tepp/rt_aggregate_dynamic.h"

namespace te
{
	void rt_aggregate_dynamic::handleCleanup(void* context) {
		auto current = te::intrusive_list_owned(this->cleanup.exchange(nullptr));

		if (current.data != nullptr) {
			current.data->for_each_forward([&](Updates& updates) {
				for (auto& update : updates) {
					update.operation->runnonrt_impl(context, *this->elements[update.index]);
				}
				this->inTransit--;
			});
		}
	}

	void rt_aggregate_dynamic::sendQueue() {
		auto current = this->updates.exchange(nullptr);

		if (current == nullptr) {
			current = new te::intrusive_list<Updates>(std::move(this->queue));
		}
		else {
			current->back().insert_after(std::move(this->queue));
		}

		[[maybe_unused]] auto old = this->updates.exchange(current);
		assert(old == nullptr);

		this->inTransit++;
	}

	void rt_aggregate_dynamic::clear() {
		for (auto element : this->elements) {
			element->clear();
		}
		this->sendQueue();
	}

	bool rt_aggregate_dynamic::processOperations() {
		auto maybeUpdates = this->updates.exchange(nullptr);

		if (maybeUpdates == nullptr) {
			return false;
		}

		auto maybeCleanup = this->cleanup.exchange(nullptr);

		maybeUpdates->for_each_forward([&](Updates& updates) {
			for (auto& update : updates) {
				update.operation->runrt_impl(*this->elements[update.index]);
			}
		});

		if (maybeCleanup == nullptr) {
			maybeCleanup = maybeUpdates;
		}
		else {
			maybeCleanup->back().insert_after(maybeUpdates);
		}

		[[maybe_unused]] auto oldCleanup = this->cleanup.exchange(maybeCleanup);
		assert(oldCleanup == nullptr);

		return true;
	}

	void rt_aggregate_dynamic::addUpdate(int64_t index, std::unique_ptr<rt_base_operation> operation) {
		this->queue.push_back({ .index = index, .operation = std::move(operation) });
	}

	void rt_aggregate_dynamic::register_rt(rt_base& base) {
		base.aggregate = this;
		base.aggregateIndex = static_cast<int64_t>(this->elements.size());
		this->elements.push_back(&base);
	}
}
