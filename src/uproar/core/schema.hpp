#ifndef UPROAR_CORE_SCHEMA_HPP
#define UPROAR_CORE_SCHEMA_HPP

#include <type_traits>
#include <vector>
#include <string_view>
#include "schema_traits.hpp"

namespace tc {
	namespace schema {
		namespace detail {
			template<typename T>
			struct member_object_type {
				using type = T;
			};

			template<typename T>
			static const constexpr member_object_type<T> member_object_type_v{};
			
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

		template<typename Type>
		struct object_traits {
			using object_t = object<Type>;
			using member = detail::member<Type, object_t>;
			using sub_object = detail::member_object<Type, object_t>;
			using primitive = detail::member_primitive<Type, object_t>;
			static constexpr const member_type type = member_type_trait_v<Type>;
		};

		struct member_context;
		struct context;
		struct context_stack;
	}    // namespace schema
}    // namespace tc

#endif    // UPROAR_CORE_SCHEMA_HPP
