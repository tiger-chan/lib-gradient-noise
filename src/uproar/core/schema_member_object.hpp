#ifndef UPROAR_CORE_SCHEMA_MEMBER_OBJECT_HPP
#define UPROAR_CORE_SCHEMA_MEMBER_OBJECT_HPP

#include "schema.hpp"

#include <tuple>

namespace tc {
	namespace schema {
		namespace detail {
			template<typename Outer, typename ObjType>
			struct member_object {
				template<typename SetType>
				using Setter = void (*)(ObjType &, context_stack &, int, Outer &, id_type, std::string_view, const SetType &);

				template<typename Y>
				member_object(ObjType &obj, member_ptr<Outer, Y> mem);

				using push_back_t = void (*)(context_stack &, int, std::string_view);

				template<typename Value>
				void set_value(ObjType &obj, context_stack &stack, int stack_pos, Outer &outer, id_type member_idx, std::string_view name, const Value &value);

				std::tuple<Setter<bool>,
					Setter<char>,
					Setter<int16>,
					Setter<int32>,
					Setter<int64>,
					Setter<uint8>,
					Setter<uint16>,
					Setter<uint32>,
					Setter<uint64>,
					Setter<float>,
					Setter<double>,
					Setter<std::string>>
					setters;

				push_back_t push_back;

			private:
				template<typename Prop, typename Value>
				static void set_impl(context_stack &stack, int stack_pos, Outer &outer, member_ptr<Outer, Prop> ptr, std::string_view name, const Value &value);

				template<typename Y>
				static void push_back_impl(context_stack &stack, int stack_pos, std::string_view name);
			};
		}    // namespace detail
	}    // namespace schema
}    // namespace tc

#include "schema_member_object.inl"

#endif    // UPROAR_CORE_SCHEMA_MEMBER_OBJECT_HPP
