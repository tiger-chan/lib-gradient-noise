#ifndef UPROAR_CORE_SCHEMA_CONTEXT_HPP
#define UPROAR_CORE_SCHEMA_CONTEXT_HPP

#include "schema.hpp"

namespace tc {
	namespace schema {
		struct member_context {
			bool dirty : 1;
			bool is_in_range : 1;
		};

		struct context {
			id_type object;
			member_type type;
			std::string_view name;
			std::vector<member_context> member_context;
		};

		struct context_stack {
			[[nodiscard]] context &emplace_back();
			[[maybe_unused]] context pop_back();

			context &operator[](int i);
			const context &operator[](int i) const;

			int size() const;
			bool empty() const;

			std::vector<context> stack;
		};
	}    // namespace schema
}    // namespace tc

#include "schema_context.inl"

#endif    // UPROAR_CORE_SCHEMA_CONTEXT_HPP
