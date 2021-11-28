#ifndef UPROAR_CORE_SCHEMA_MEMBER_PRIMITIVE_HPP
#define UPROAR_CORE_SCHEMA_MEMBER_PRIMITIVE_HPP

#include "schema.hpp"

#include <variant>

namespace tc {
	namespace schema {
		namespace detail {
			using PrimitiveTypeVariant = std::variant<
				member_object_type<bool>,
				member_object_type<char>,
				member_object_type<int16>,
				member_object_type<int32>,
				member_object_type<int64>,
				member_object_type<uint8>,
				member_object_type<uint16>,
				member_object_type<uint32>,
				member_object_type<uint64>,
				member_object_type<float>,
				member_object_type<double>,
				member_object_type<std::string>>;

			template<typename Outer, typename ObjType, typename SetType>
			using PrimitiveSetter = void (*)(ObjType &, context &, Outer &, id_type, const SetType &);

			template<typename Outer, typename ObjType>
			struct member_primitive_vtable {
				template<typename SetType>
				using Setter = PrimitiveSetter<Outer, ObjType, SetType>;
				using SetterVariant = std::variant<
					Setter<bool>,
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
					Setter<std::string>>;

				PrimitiveTypeVariant type;
				SetterVariant setter;

				template<typename X>
				void set_value(ObjType &obj, context &ctx, Outer &outer, id_type member_idx, const X &value);

				template<typename X, typename Y>
				static void set(member_context &ctx, member<Outer, ObjType> &member, X &ptr_value, const Y &value);
			};

			template<typename Outer, typename ObjType>
			struct member_primitive {
				template<typename Y>
				member_primitive(ObjType &obj, member_object_type<Y>);

				template<typename Y>
				member_primitive(ObjType &obj, member_ptr<Outer, Y> mem);

				template<typename Value>
				void set_value(ObjType &obj, context &ctx, Outer &outer, id_type member_idx, const Value &value);
			private:
				member_primitive_vtable<Outer, ObjType> vtable;
			};
		}    // namespace detail
	}    // namespace schema
}    // namespace tc

#include "schema_member_primitive.inl"

#endif    // UPROAR_CORE_SCHEMA_MEMBER_PRIMITIVE_HPP
