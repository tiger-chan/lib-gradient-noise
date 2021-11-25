#ifndef UPROAR_CORE_SCHEMA_MEMBER_PRIMITIVE_HPP
#define UPROAR_CORE_SCHEMA_MEMBER_PRIMITIVE_HPP

#include "schema.hpp"

#include <variant>

namespace tc {
	namespace schema {
		namespace detail {
			template<typename T>
			struct PrimitiveType {
				using type = T;
			};

			using PrimitiveTypeVariant = std::variant<
				PrimitiveType<bool>,
				PrimitiveType<char>,
				PrimitiveType<int16>,
				PrimitiveType<int32>,
				PrimitiveType<int64>,
				PrimitiveType<uint8>,
				PrimitiveType<uint16>,
				PrimitiveType<uint32>,
				PrimitiveType<uint64>,
				PrimitiveType<float>,
				PrimitiveType<double>,
				PrimitiveType<std::string>>;

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
