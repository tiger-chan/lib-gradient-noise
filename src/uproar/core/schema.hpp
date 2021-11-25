#ifndef UPROAR_CORE_SCHEMA_HPP
#define UPROAR_CORE_SCHEMA_HPP

#include <type_traits>
#include <vector>
#include <string_view>
#include "schema_traits.hpp"

namespace tc {
	namespace schema {
		namespace detail {
			template<typename Outer, typename ObjType>
			struct member;
			
			template<typename Outer, typename ObjType>
			struct member_primitive;
			
			template<typename Outer, typename ObjType>
			struct member_object;
		}

		struct context;
		struct member_context;
		struct context_stack;

		template<typename>
		struct required;
		template<typename>
		struct not_null;
		template<typename>
		struct default_value;
		template<typename>
		struct range;

		template<typename X, typename = std::enable_if_t<std::is_enum_v<X>>>
		struct enum_to_string {
			static std::string_view to_string(X x);
			static X to_enum(std::string_view x);
		};
		
		template<typename Type, typename Return>
		using member_ptr = Return Type::*;

		template<typename Type>
		struct object;

		struct member_context;
		struct context;
		struct context_stack;
	}    // namespace schema
}    // namespace tc

#endif    // UPROAR_CORE_SCHEMA_HPP
