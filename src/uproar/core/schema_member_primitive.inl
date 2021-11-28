#include "schema_member_primitive.hpp"

namespace tc {
	namespace schema {
		namespace detail {
			template<typename Outer, bool use_self, typename Prop>
			member_primitive_vtable<Outer, object<Outer>> create_object_primitive_vtable(member_object_type<Prop>) {
				using ObjOuter = object<Outer>;
				member_primitive_vtable<Outer, ObjOuter> vtable;

				vtable.type = member_object_type<Prop>{};

				static auto set_member = [](ObjOuter &obj, context &ctx, Outer &outer, id_type member_idx, const auto& value) {
					member<Outer, ObjOuter> &mem = obj.props[member_idx];
					if constexpr(use_self) {
						member_primitive_vtable<Outer, ObjOuter>::set(ctx.member_context[member_idx], mem, outer, value);
					}
					else {
						member_ptr<Outer, Prop> ptr = obj.prop_ptrs<Prop>[mem.ptr];
						member_primitive_vtable<Outer, ObjOuter>::set(ctx.member_context[member_idx], mem, outer.*ptr, value);
					}
				};
				
				if constexpr (member_type_trait_v<Prop> == MT_enum) {
					vtable.setter = [](ObjOuter &obj, context &ctx, Outer &outer, id_type member_idx, const std::string& value) {
						set_member(obj, ctx, outer, member_idx, value);
					}
				}
				else {
					vtable.setter = [](ObjOuter &obj, context &ctx, Outer &outer, id_type member_idx, const Prop& value) {
						set_member(obj, ctx, outer, member_idx, value);
					};
				}

				return vtable;
			}
			
			template<typename Outer, bool use_self, typename Prop>
			member_primitive_vtable<Outer, object<Outer>> create_array_primitive_vtable(member_object_type<Prop>) {
				using ObjOuter = object<Outer>;
				member_primitive_vtable<Outer, ObjOuter> vtable;

				vtable.type = member_object_type<Prop>{};

				static auto set_member = [](ObjOuter &obj, context &ctx, Outer &outer, id_type member_idx, const auto& value) {
					member<Outer, ObjOuter> &mem = obj.props[member_idx];
					if constexpr (use_self) {
						auto &v = outer;
						if (ctx.id.idx >= v.size()) {
							v.resize(ctx.id.idx + 1);
						}
						member_primitive_vtable<Outer, ObjOuter>::set(ctx.member_context[0], mem, v[ctx.id.idx], value);
					}
					else {
						member_ptr<Outer, Prop> ptr = obj.prop_ptrs<Prop>[mem.ptr];
						auto &v = outer.*ptr;
						if (ctx.id.idx >= v.size()) {
							v.resize(ctx.id.idx + 1);
						}
						member_primitive_vtable<Outer, ObjOuter>::set(ctx.member_context[0], mem, v[ctx.id.idx], value);
					}
				};

				if constexpr (member_type_trait_v<Prop> == MT_enum) {
					vtable.setter = [](ObjOuter &obj, context &ctx, Outer &outer, id_type member_idx, const std::string& value) {
						set_member(obj, ctx, outer, member_idx, value);
					};
				}
				else {
					vtable.setter = [](ObjOuter &obj, context &ctx, Outer &outer, id_type member_idx, const Prop& value) {
						set_member(obj, ctx, outer, member_idx, value);
					};
				}

				return vtable;
			}

			template<typename Outer,  bool use_self, typename Prop>
			member_primitive_vtable<Outer, object<Outer>> create_primitive_vtable(member_object_type<Prop>) {
				if constexpr (member_type_trait_v<Outer> == MT_object) {
					return create_object_primitive_vtable<Outer, use_self>(member_object_type_v<Prop>);
				}
				else if constexpr (member_type_trait_v<Outer> == MT_array) {
					return create_array_primitive_vtable<Outer, use_self>(member_object_type_v<Prop>);
				}
				else {
					static_assert(member_type_trait_v<Prop> >= MT_array, "Member type must be object to create vtable");
					return member_primitive_vtable<Outer, object<Outer>>{};
				}
			}
		}
	}
}


namespace tc {
	namespace schema {
		namespace detail {
			template<typename Outer, typename ObjType>
			template<typename Prop>
			member_primitive<Outer, ObjType>::member_primitive(ObjType &obj, member_object_type<Prop>)
				: vtable{ create_primitive_vtable<Outer, true>(member_object_type_v<Prop>) } {
			}
			
			template<typename Outer, typename ObjType>
			template<typename Prop>
			member_primitive<Outer, ObjType>::member_primitive(ObjType &obj, member_ptr<Outer, Prop>)
				: vtable{ create_primitive_vtable<Outer, false>(member_object_type_v<Prop>) } {
			}

			template<typename Outer, typename ObjType>
			template<typename Value>
			void member_primitive<Outer, ObjType>::set_value(ObjType &obj, context &ctx, Outer &outer, id_type member_idx, const Value &value) {
				vtable.set_value(obj, ctx, outer, member_idx, value);
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
				if constexpr(std::is_convertible_v<Y, X>) {
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
		}    // namespace detail
	}    // namespace schema
}    // namespace tc
