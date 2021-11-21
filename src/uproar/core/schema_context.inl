#include "schema_context.hpp"

namespace tc {
	namespace schema {
		context &context_stack::emplace_back() {
			return stack.emplace_back();
		}

		context context_stack::pop_back() {
			auto pop = stack.back();
			stack.pop_back();
			return pop;
		}

		context &context_stack::operator[](int i) {
			return stack[i];
		}

		const context &context_stack::operator[](int i) const {
			return stack[i];
		}

		int context_stack::size() const {
			return int(stack.size());
		}

		bool context_stack::empty() const {
			return stack.empty();
		}
	}    // namespace schema
}    // namespace tc
