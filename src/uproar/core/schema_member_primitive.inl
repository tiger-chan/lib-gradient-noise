#include "schema_member_primitive.hpp"

namespace tc {
	namespace schema {
		namespace detail {
			template<typename Outer, typename ObjType>
			template<typename Prop>
			member_primitive<Outer, ObjType>::member_primitive(ObjType &obj, member_ptr<Outer, Prop> mem) {
#define SET_TYPE(TYPE) \
	if constexpr (std::is_same_v<Prop, TYPE>) { \
		setter_type = PrimitiveType<TYPE>{}; \
		setter = [](ObjType &obj, member_context &ctx, Outer &outer, id_type member_idx, const TYPE &value) -> void { \
			set_impl<Prop, TYPE>(obj, ctx, outer, obj.props[member_idx], value); \
		}; \
	}

				SET_TYPE(bool)

				else SET_TYPE(char) else SET_TYPE(int16) else SET_TYPE(int32) else SET_TYPE(int64)

					else SET_TYPE(uint8) else SET_TYPE(uint16) else SET_TYPE(uint32) else SET_TYPE(uint64)

						else SET_TYPE(float) else SET_TYPE(double)
#undef SET_TYPE

							else if constexpr (member_type_trait_v<Prop> == MT_string || member_type_trait_v<Prop> == MT_enum) {
					setter_type = PrimitiveType<std::string>{};
					setter = [](ObjType &obj, member_context &ctx, Outer &outer, id_type member_idx, const std::string &value) -> void {
						set_impl<Prop, std::string>(obj, ctx, outer, obj.props[member_idx], value);
					};
				}
			}

			template<typename Outer, typename ObjType>
			template<typename Value>
			void member_primitive<Outer, ObjType>::set_value(ObjType &obj, member_context &ctx, Outer &outer, id_type member_idx, const Value &value) {
				auto handler = [this, &obj, &ctx, &outer, &member_idx, &value](auto &primitive) {
					using Type = typename std::decay_t<decltype(primitive)>::type;
					if constexpr (std::is_convertible_v<Value, Type>) {
						auto &set = std::get<PrimitiveSetter<Type>>(setter);
						set(obj, ctx, outer, member_idx, static_cast<Type>(value));
					}
				};
				std::visit(handler, setter_type);
			}

			template<typename Outer, typename ObjType>
			template<typename Prop, typename Value>
			void member_primitive<Outer, ObjType>::set_impl(ObjType &obj, member_context &ctx, Outer &outer, member<Outer, ObjType> &mem, const Value &value) {
				obj.set_value<Prop, Value>(ctx, mem, outer, value);
			}
		}    // namespace detail
	}    // namespace schema
}    // namespace tc
