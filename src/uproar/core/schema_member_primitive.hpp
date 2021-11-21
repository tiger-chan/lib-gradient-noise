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

			template<typename Outer, typename ObjType>
			struct member_primitive {
				template<typename SetType>
				using PrimitiveSetter = void (*)(ObjType &obj, member_context &ctx, Outer &outer, id_type member_idx, const SetType &);
				using PrimitiveSetterVariant = std::variant<
					PrimitiveSetter<bool>,
					PrimitiveSetter<char>,
					PrimitiveSetter<int16>,
					PrimitiveSetter<int32>,
					PrimitiveSetter<int64>,
					PrimitiveSetter<uint8>,
					PrimitiveSetter<uint16>,
					PrimitiveSetter<uint32>,
					PrimitiveSetter<uint64>,
					PrimitiveSetter<float>,
					PrimitiveSetter<double>,
					PrimitiveSetter<std::string>>;

				template<typename Y>
				member_primitive(ObjType &obj, member_ptr<Outer, Y> mem);

				template<typename Value>
				void set_value(ObjType &obj, member_context &ctx, Outer &outer, id_type member_idx, const Value &value);

				PrimitiveSetterVariant setter;
				PrimitiveTypeVariant setter_type;

				template<typename Prop, typename Value>
				static void set_impl(ObjType &obj, member_context &ctx, Outer &outer, member<Outer, ObjType> &mem, const Value &value);
			};
		}    // namespace detail
	}    // namespace schema
}    // namespace tc

#include "schema_member_primitive.inl"

#endif    // UPROAR_CORE_SCHEMA_MEMBER_PRIMITIVE_HPP
