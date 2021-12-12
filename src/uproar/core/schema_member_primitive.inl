#include "schema_member_primitive.hpp"

namespace tc {
	namespace schema {
		namespace detail {
			template<typename>
			struct ObjectPrimitiveVtableHelper;

			template<typename Outer>
			struct ObjectPrimitiveVtableHelper {
				static_assert(member_type_trait_v<Outer> == MT_object, "Object Primitive VTable helper should only be used with object types");
				using ObjOuter = object<Outer>;
				using obj_vtable = member_primitive_vtable<Outer, ObjOuter>;
				using obj_member = member<Outer, ObjOuter>;
				using obj_prim = member_primitive<Outer, ObjOuter>;
				using obj_object = member_object<Outer, ObjOuter>;

				template<bool use_self, typename Prop>
				static auto create_set_value() {
					using obj_ptr = member_ptr<Outer, Prop>;
					static auto set_member = [](const ObjOuter &obj, context &ctx, Outer &outer, id_type member_idx, const auto &value) {
						const obj_member &prop = obj.props.at(ctx.object);
						if constexpr (use_self) {
							obj_vtable::set(ctx.member_context[ctx.object], prop, outer, value);
						}
						else {
							const obj_ptr ptr = obj.prop_ptrs<Prop>.at(prop.ptr);
							auto &sub = outer.*ptr;
							obj_vtable::set(ctx.member_context[ctx.object], prop, sub, value);
						}
					};

					if constexpr (member_type_trait_v<Prop> == MT_enum) {
						return [](const ObjOuter &obj, context &ctx, Outer &outer, id_type member_idx, const std::string &value) {
							set_member(obj, ctx, outer, member_idx, enum_to_string<Prop>::to_enum(value));
						};
					}
					else {
						return [](const ObjOuter &obj, context &ctx, Outer &outer, id_type member_idx, const Prop &value) {
							set_member(obj, ctx, outer, member_idx, value);
						};
					}
				}

				template<bool use_self, typename Prop>
				static auto create_visitor() {
					using obj_ptr = member_ptr<Outer, Prop>;
					return [](const ObjOuter &obj, schema::visitor &v, Outer &outer, id_type member_idx) {
						const obj_member &prop = obj.props[member_idx];
						if constexpr (!use_self) {
							obj_ptr ptr = obj.prop_ptrs<Prop>[prop.ptr];
							auto &sub = outer.*ptr;
							v.set(prop.name, sub);
						}
					};
				}

				template<bool use_self, typename Prop>
				static obj_vtable create(const member_object_type<Prop> &) {
					obj_vtable vtable{};

					if constexpr (std::is_enum_v<Prop>) {
						vtable.type = member_object_type_v<std::string>;
					}
					else {
						vtable.type = member_object_type_v<Prop>;
					}
					vtable.setter = create_set_value<use_self, Prop>();
					vtable.visit = create_visitor<use_self, Prop>();

					return vtable;
				}
			};

			template<typename>
			struct ArrayPrimitiveVtableHelper;

			template<template<class, class...> typename OuterContainer, typename Type, typename... Rest>
			struct ArrayPrimitiveVtableHelper<OuterContainer<Type, Rest...>> {
				using Outer = OuterContainer<Type, Rest...>;
				static_assert(member_type_trait_v<Outer> == MT_array, "Array Primitive VTable helper should only be used with array types");
				using ObjOuter = object<Outer>;
				using obj_vtable = member_primitive_vtable<Outer, ObjOuter>;
				using obj_member = member<Outer, ObjOuter>;
				using obj_prim = member_primitive<Outer, ObjOuter>;
				using obj_object = member_object<Outer, ObjOuter>;

				template<bool use_self, typename Prop>
				static auto create_set_value() {
					using obj_ptr = member_ptr<Outer, Prop>;

					static auto set_member = [](const ObjOuter &obj, context &ctx, Outer &outer, id_type member_idx, const auto &value) {
						const obj_member &prop = obj.props.at(ctx.object);
						if constexpr (use_self) {
							auto &arr = outer;
							auto &v = arr[ctx.id.idx];

							obj_vtable::set(ctx.member_context[0], prop, v, value);
						}
					};

					if constexpr (member_type_trait_v<Prop> == MT_enum) {
						return [](const ObjOuter &obj, context &ctx, Outer &outer, id_type member_idx, const std::string &value) {
							set_member(obj, ctx, outer, member_idx, enum_to_string<Prop>::to_enum(value));
						};
					}
					else {
						return [](const ObjOuter &obj, context &ctx, Outer &outer, id_type member_idx, const Prop &value) {
							set_member(obj, ctx, outer, member_idx, value);
						};
					}
				}

				template<bool use_self, typename Prop>
				static auto create_visitor() {
					using obj_ptr = member_ptr<Outer, Prop>;
					return [](const ObjOuter &obj, schema::visitor &v, Outer &outer, id_type member_idx) {
						if constexpr (use_self) {
							auto &arr = outer;
							for (const auto &i : arr) {
								static constexpr std::string_view empty_name{ "" };
								v.set(empty_name, i);
							}
						}
					};
				}

				template<bool use_self, typename Prop>
				static obj_vtable create(const member_object_type<Prop> &) {
					obj_vtable vtable{};

					if constexpr (std::is_enum_v<Prop>) {
						vtable.type = member_object_type<std::string>{};
					}
					else {
						vtable.type = member_object_type<Prop>{};
					}

					vtable.setter = create_set_value<use_self, Prop>();
					vtable.visit = create_visitor<use_self, Prop>();

					return vtable;
				}
			};

			template<typename>
			struct MapPrimitiveVtableHelper;

			template<template<class, class, class...> typename OuterContainer, typename Key, typename Type, typename... Rest>
			struct MapPrimitiveVtableHelper<OuterContainer<Key, Type, Rest...>> {
				using Outer = OuterContainer<Key, Type, Rest...>;
				static_assert(member_type_trait_v<Outer> == MT_map, "Map Primitive VTable helper should only be used with maps types");
				using ObjOuter = object<Outer>;
				using obj_vtable = member_primitive_vtable<Outer, ObjOuter>;
				using obj_member = member<Outer, ObjOuter>;
				using obj_prim = member_primitive<Outer, ObjOuter>;
				using obj_object = member_object<Outer, ObjOuter>;

				template<bool use_self, typename Prop>
				static auto create_set_value() {
					using obj_ptr = member_ptr<Outer, Prop>;

					static auto set_member = [](const ObjOuter &obj, context &ctx, Outer &outer, id_type member_idx, const auto &value) {
						const obj_member &prop = obj.props.at(member_idx);
						if constexpr (use_self) {
							auto &arr = outer;
							auto &v = arr[std::string(ctx.id.name)];

							obj_vtable::set(ctx.member_context[0], prop, v, value);
						}
					};

					if constexpr (member_type_trait_v<Prop> == MT_enum) {
						return [](const ObjOuter &obj, context &ctx, Outer &outer, id_type member_idx, const std::string &value) {
							set_member(obj, ctx, outer, member_idx, enum_to_string<Prop>::to_enum(value));
						};
					}
					else {
						return [](const ObjOuter &obj, context &ctx, Outer &outer, id_type member_idx, const Prop &value) {
							set_member(obj, ctx, outer, member_idx, value);
						};
					}
				}

				template<bool use_self, typename Prop>
				static auto create_visitor() {
					using obj_ptr = member_ptr<Outer, Prop>;
					return [](const ObjOuter &obj, schema::visitor &v, Outer &outer, id_type member_idx) {
						if constexpr (use_self) {
							auto &arr = outer;
							for (const auto &i : arr) {
								v.set(i.first, i.second);
							}
						}
					};
				}

				template<bool use_self, typename Prop>
				static obj_vtable create(const member_object_type<Prop> &) {
					obj_vtable vtable{};

					if constexpr (std::is_enum_v<Type>) {
						vtable.type = member_object_type<std::string>{};
					}
					else {
						vtable.type = member_object_type<Type>{};
					}

					vtable.setter = create_set_value<use_self, Type>();
					vtable.visit = create_visitor<use_self, Type>();

					return vtable;
				}
			};

			template<typename Outer, bool use_self, typename Prop>
			member_primitive_vtable<Outer, object<Outer>> create_primitive_vtable(member_object_type<Prop>) {
				if constexpr (member_type_trait_v<Prop> < MT_object) {
					static_assert(member_type_trait_v<Outer> >= MT_object, "Property type must be a primitive to create vtable");
				}

				if constexpr (member_type_trait_v<Outer> == MT_object) {
					return ObjectPrimitiveVtableHelper<Outer>::create<use_self>(member_object_type_v<Prop>);
				}
				else if constexpr (member_type_trait_v<Outer> == MT_array) {
					return ArrayPrimitiveVtableHelper<Outer>::create<use_self>(member_object_type_v<Prop>);
				}
				else if constexpr (member_type_trait_v<Outer> == MT_map) {
					return MapPrimitiveVtableHelper<Outer>::create<use_self>(member_object_type_v<Prop>);
				}

				return member_primitive_vtable<Outer, object<Outer>>{};
			}
		}    // namespace detail
	}    // namespace schema
}    // namespace tc

namespace tc {
	namespace schema {
		namespace detail {
			template<typename Outer, typename ObjType>
			template<typename Prop>
			member_primitive<Outer, ObjType>::member_primitive(ObjType &obj, member_object_type<Prop>)
				: type_id{ type_identifier<Prop>() }
				, vtable{ create_primitive_vtable<Outer, true>(member_object_type_v<Prop>) } {
			}

			template<typename Outer, typename ObjType>
			template<typename Prop>
			member_primitive<Outer, ObjType>::member_primitive(ObjType &obj, member_ptr<Outer, Prop>)
				: type_id{ type_identifier<Prop>() }
				, vtable{ create_primitive_vtable<Outer, false>(member_object_type_v<Prop>) } {
			}

			template<typename Outer, typename ObjType>
			template<typename Value>
			void member_primitive<Outer, ObjType>::set_value(const ObjType &obj, context &ctx, Outer &outer, id_type member_idx, const Value &value) const {
				vtable.set_value(obj, ctx, outer, member_idx, value);
			}

			template<typename Outer, typename ObjType>
			void member_primitive<Outer, ObjType>::visit(const ObjType &obj, schema::visitor &v, Outer &outer, id_type id) const {
				vtable.visit(obj, v, outer, id);
			}

			template<typename Outer, typename ObjType>
			template<typename X>
			void member_primitive_vtable<Outer, ObjType>::set_value(const ObjType &obj, context &ctx, Outer &outer, id_type member_idx, const X &value) const {
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
			void member_primitive_vtable<Outer, ObjType>::set(member_context &ctx, const member<Outer, ObjType> &member, X &ptr_value, const Y &value) {
				if constexpr (std::is_convertible_v<Y, X>) {
					object<Outer> &obj = object<Outer>::instance;
					ctx.is_in_range = true;
					if (member.range != schema::null) {
						ctx.is_in_range = detail::check_constraint(obj.ranges<X>[member.range], member, value);
					}

					if constexpr (member_type_trait_v<X> < MT_object) {
						ptr_value = static_cast<X>(value);
					}
					ctx.dirty = true;
				}
			}
		}    // namespace detail
	}    // namespace schema
}    // namespace tc
