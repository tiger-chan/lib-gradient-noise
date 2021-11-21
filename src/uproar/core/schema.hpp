#ifndef UPROAR_CORE_SCHEMA_HPP
#define UPROAR_CORE_SCHEMA_HPP

#include <type_traits>
#include <vector>
#include <string_view>

#ifndef TC_STDINT
#	define TC_STDINT
using int8 = signed char;
using int16 = signed short;
using int32 = signed int;
using int64 = signed long long;

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;
#endif

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

		template<typename>
		struct is_member_constraint : std::false_type {};

		template<typename T>
		constexpr bool is_member_constraint_v = is_member_constraint<T>::value;

		template<typename X, typename = std::enable_if_t<std::is_enum_v<X>>>
		struct enum_to_string {
			static std::string_view to_string(X x);
			static X to_enum(std::string_view x);
		};

		using id_type = uint32;
		static const constexpr id_type null = UINT32_MAX;

		enum member_type : int32 {
			MT_bool,
			MT_char,
			MT_int16,
			MT_int32,
			MT_int64,
			MT_uint8,
			MT_uint16,
			MT_uint32,
			MT_uint64,
			MT_float,
			MT_double,
			MT_string,
			MT_enum,
			MT_object,
		};

		template<member_type Value>
		struct member_type_value_trait {
			static constexpr member_type value = Value;
		};

		template<typename Value>
		struct member_type_trait : public member_type_value_trait<std::is_enum_v<Value> ? MT_enum : MT_object> {};

		template<>
		struct member_type_trait<bool> : public member_type_value_trait<MT_bool> {};

		template<>
		struct member_type_trait<uint8> : public member_type_value_trait<MT_uint8> {};

		template<>
		struct member_type_trait<uint16> : public member_type_value_trait<MT_uint16> {};

		template<>
		struct member_type_trait<uint32> : public member_type_value_trait<MT_uint32> {};
		
		template<>
		struct member_type_trait<uint64> : public member_type_value_trait<MT_uint64> {};

		template<>
		struct member_type_trait<char> : public member_type_value_trait<MT_char> {};

		template<>
		struct member_type_trait<int16> : public member_type_value_trait<MT_int16> {};

		template<>
		struct member_type_trait<int32> : public member_type_value_trait<MT_int32> {};
		
		template<>
		struct member_type_trait<int64> : public member_type_value_trait<MT_int64> {};
		
		template<>
		struct member_type_trait<float> : public member_type_value_trait<MT_float> {};
		
		template<>
		struct member_type_trait<double> : public member_type_value_trait<MT_double> {};
		
		template<>
		struct member_type_trait<std::string> : public member_type_value_trait<MT_string> {};
		
		template<>
		struct member_type_trait<char *> : public member_type_value_trait<MT_string> {};

		template<typename T>
		static constexpr member_type member_type_trait_v = member_type_trait<T>::value;

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
