#ifndef UPROAR_CORE_SCHEMA_MEMBER_OBJECT_HPP
#define UPROAR_CORE_SCHEMA_MEMBER_OBJECT_HPP

#include "schema.hpp"

#include <tuple>

namespace tc {
	namespace schema {
		namespace detail {
			template<typename Outer, typename ObjType>
			struct member_object_vtable {
				template<typename T>
				using push_back_t = void (*)(context_stack &, int, T);
				template<typename SetType>
				using Setter = void (*)(ObjType &, context_stack &, int, Outer &, std::string_view, const SetType &);

				Setter<bool> set_bool;
				Setter<char> set_char;
				Setter<int16> set_int16;
				Setter<int32> set_int32;
				Setter<int64> set_int64;
				Setter<uint8> set_uint8;
				Setter<uint16> set_uint16;
				Setter<uint32> set_uint32;
				Setter<uint64> set_uint64;
				Setter<float> set_float;
				Setter<double> set_double;
				Setter<std::string> set_string;

				push_back_t<std::string_view> push_back;
				push_back_t<int32> push_back_array;

				template<typename X>
				void set_value(ObjType &, context_stack &, int, Outer &, std::string_view, const X &);
			};
			
			template<typename Outer, typename ObjType>
			struct member_object {
				member_object(ObjType &obj);
				template<typename Y>
				member_object(ObjType &obj, member_object_type<Y> mem);


				void push_back(context_stack &, int32, std::string_view);
				void push_back(context_stack &, int32, int32);
				template<typename Value>
				void set_value(ObjType &obj, context_stack &stack, int stack_pos, Outer &outer, std::string_view name, const Value &value);

			private:
				member_object_vtable<Outer, ObjType> vtable;
			};
		}    // namespace detail
	}    // namespace schema
}    // namespace tc

#include "schema_member_object.inl"

#endif    // UPROAR_CORE_SCHEMA_MEMBER_OBJECT_HPP
