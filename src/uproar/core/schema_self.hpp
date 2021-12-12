#ifndef UPROAR_CORE_SCHEMA_SELF_HPP
#define UPROAR_CORE_SCHEMA_SELF_HPP

#include "schema.hpp"

#include <tuple>

namespace tc {
	namespace schema {
		namespace detail {
			template<typename Outer, typename ObjType>
			struct self_vtable {
				template<typename T>
				using push_back_t = void (*)(context_stack &, int, T);
				template<typename SetType>
				using Setter = void (*)(const ObjType &, context_stack &, int, Outer &, const SetType &);
				using Visitor = void (*)(const ObjType &, schema::visitor &, Outer &, const std::string_view &);

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

				push_back_t<const std::string_view &> push_back;
				push_back_t<const int32 &> push_back_array;
				Visitor visit;

				template<typename X>
				void set_value(const ObjType &, context_stack &, int, Outer &, const X &) const;
			};

			template<typename Outer, typename ObjType>
			struct self_object {
				self_object();

				void push_back(context_stack &, int32, const std::string_view &) const;
				void push_back(context_stack &, int32, int32) const;
				template<typename Value>
				void set_value(const ObjType &obj, context_stack &stack, int stack_pos, Outer &outer, const Value &value) const;

				void visit(const ObjType &obj, schema::visitor &v, Outer &outer, const std::string_view &outer_name) const;

				const id_type type_id;

			private:
				self_vtable<Outer, ObjType> vtable;
			};
		}    // namespace detail
	}    // namespace schema
}    // namespace tc

#include "schema_self.inl"

#endif    // UPROAR_CORE_SCHEMA_SELF_HPP
