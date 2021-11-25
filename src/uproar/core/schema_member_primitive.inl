#include "schema_member_primitive.hpp"

namespace tc {
	namespace schema {
		namespace detail {
			template<typename Outer, typename ObjType, typename Prop, typename = std::enable_if_t<!is_container_v<Prop> && !is_map_v<Prop>>>
			member_primitive_vtable<Outer, ObjType> create_primitive_vtable(member_ptr<Outer, Prop>) {
				member_primitive_vtable<Outer, ObjType> vtable;
				vtable.type = PrimitiveType<Prop>{};
				if constexpr (member_type_trait_v<Prop> == MT_enum) {
					vtable.setter = [](ObjType &obj, context &ctx, Outer &outer, id_type member_idx, const std::string& value) {
						object<Outer> &obj = object<Outer>::instance;
						member<Outer, ObjType> &mem = obj.props[member_idx];
						member_ptr<Outer, Prop> ptr = instance.prop_ptrs<Prop>[mem.ptr];
						member_primitive_vtable<Outer, ObjType>::set(ctx.member_context[member_idx], mem, outer.*ptr, value);
					}
				}
				else {
					vtable.setter = [](ObjType &obj, context &ctx, Outer &outer, id_type member_idx, const Prop& value) {
						member<Outer, ObjType> &mem = obj.props[member_idx];
						member_ptr<Outer, Prop> ptr = obj.prop_ptrs<Prop>[mem.ptr];
						member_primitive_vtable<Outer, ObjType>::set(ctx.member_context[member_idx], mem, outer.*ptr, value);
					};
				}

				return vtable;
			}
			
			template<typename Outer, typename ObjType, typename Type, template<class...> typename Prop, typename ...Rest, typename = std::enable_if_t<is_container_v<Prop<Type, Rest...>> && !is_map_v<Prop<Type, Rest...>>>>
			member_primitive_vtable<Outer, ObjType> create_primitive_vtable(member_ptr<Outer, Prop<Type, Rest...>>) {
				member_primitive_vtable<Outer, ObjType> vtable;
				vtable.type = PrimitiveType<Type>{};
				if constexpr (member_type_trait_v<Type> == MT_enum) {
					vtable.setter = [](ObjType &obj, context &ctx, Outer &outer, id_type member_idx, const std::string& value) {
						// object<Outer> &obj = object<Outer>::instance;
						// member<Outer, ObjType> &mem = obj.props[member_idx];
						// member_ptr<Outer, Prop<Type, Rest...>> ptr = instance.prop_ptrs<Prop<Type, Rest...>>[mem.ptr];
						// member_primitive_vtable<Outer, ObjType>::set(ctx, mem, outer.*ptr, value);
					};
				}
				else {
					vtable.setter = [](ObjType &obj, context &ctx, Outer &outer, id_type member_idx, const Type& value) {
						member<Outer, ObjType> &mem = obj.props[member_idx];
						member_ptr<Outer, Prop<Type, Rest...>> ptr = obj.prop_ptrs<Prop<Type, Rest...>>[mem.ptr];
						auto &v = outer.*ptr;
						if (ctx.id.idx >= v.size()) {
							v.resize(ctx.id.idx + 1);
						}
						member_primitive_vtable<Outer, ObjType>::set(ctx.member_context[0], mem, v[ctx.id.idx], value);
					};
				}

				return vtable;
			}

			template<typename Outer, typename ObjType>
			template<typename X>
			void member_primitive_vtable<Outer, ObjType>::set_value(ObjType &obj, context &ctx, Outer &outer, id_type member_idx, const X &value) {
				auto handler = [this, &obj, &ctx, &outer, &member_idx, &value](auto &primitive) {
					using Type = typename std::decay_t<decltype(primitive)>::type;
					if constexpr (std::is_convertible_v<X, Type>) {
						auto &set_ptr = std::get<Setter<Type>>(setter);
						set_ptr(obj, ctx, outer, member_idx, static_cast<Type>(value));
					}
				};
				std::visit(handler, type);
			}

			template<typename Outer, typename ObjType>
			template<typename X, typename Y>
			void member_primitive_vtable<Outer, ObjType>::set(member_context& ctx, member<Outer, ObjType>&member, X &ptr_value, const Y &value) {
				object<Outer> &obj = object<Outer>::instance;
				ctx.is_in_range = true;
				if (member.range != schema::null) {
					if constexpr (std::is_enum_v<X>) {
						X prop = enum_to_string<Prop>::to_enum(value);
						ctx.is_in_range = detail::check_constraint(obj.ranges<X>[member.range], member, prop);
					}
					else {
						ctx.is_in_range = detail::check_constraint(obj.ranges<X>[member.range], member, value);
					}
				}

				if constexpr (std::is_enum_v<X>) {
					ptr_value = enum_to_string<X>::to_enum(value);
				}
				else if (member_type_trait_v<X> != MT_object) {
					ptr_value = static_cast<X>(value);
				}
				ctx.dirty = true;
			}
		}
	}
}


namespace tc {
	namespace schema {
		namespace detail {
			template<typename Outer, typename ObjType>
			template<typename Prop>
			member_primitive<Outer, ObjType>::member_primitive(ObjType &obj, member_ptr<Outer, Prop> mem)
				: vtable{ create_primitive_vtable<Outer, ObjType>(mem) } {
			}

			template<typename Outer, typename ObjType>
			template<typename Value>
			void member_primitive<Outer, ObjType>::set_value(ObjType &obj, context &ctx, Outer &outer, id_type member_idx, const Value &value) {
				vtable.set_value(obj, ctx, outer, member_idx, value);
			}
		}    // namespace detail
	}    // namespace schema
}    // namespace tc
