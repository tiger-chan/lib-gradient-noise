#ifndef UPROAR_CORE_SCHEMA_DEFINITION_HPP
#define UPROAR_CORE_SCHEMA_DEFINITION_HPP

#include "schema_object.hpp"

namespace tc {
	namespace schema {
		template<typename Type>
		struct definition {
			template<typename Y, typename... Z, template<class> typename... Constraint>
			void add_member(const char *name, member_ptr<Type, Y> mem_ptr, const char *desc, Constraint<Z> &&...constraints);
		};
	}    // namespace schema
}    // namespace tc

#include "schema_definition.inl"

#endif    // UPROAR_CORE_SCHEMA_DEFINITION_HPP
