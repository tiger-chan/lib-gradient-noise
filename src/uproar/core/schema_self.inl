#include "schema_member_object.hpp"
#include "schema_member_primitive.hpp"
#include "schema_self.hpp"

namespace tc {
	namespace schema {
		namespace detail {
			// Container / Array vtable creation
			template<typename>
			struct SelfObjectVtableHelper;

			template<typename Outer>
			struct SelfObjectVtableHelper {
				static_assert(member_type_trait_v<Outer> == MT_object, "Self Object VTable helper should only be used with object types");
				using ObjOuter = object<Outer>;
				using obj_vtable = self_vtable<Outer, ObjOuter>;
				using obj_member = member<Outer, ObjOuter>;
				using obj_prim = member_primitive<Outer, ObjOuter>;
				using obj_object = member_object<Outer, ObjOuter>;

				static auto create_set_value() {
					return [](const ObjOuter &obj, context_stack &stack, int stack_pos, Outer &outer, const auto &value) {
						context &ctx = stack[stack_pos];
						const obj_member &prop = obj.props.at(ctx.object);
						if (stack_pos == stack.size() - 1) {
							// this is a as deep as we need to go.
							const obj_prim &prim = obj.primitives.at(prop.primitive);
							prim.set_value(obj, ctx, outer, ctx.object, value);
						}
						else {
							const obj_object &sub = obj.children.at(prop.child);
							sub.set_value(obj, stack, stack_pos, outer, value);
						}
					};
				}

				static auto create_push_back() {
					return [](context_stack &stack, int stack_pos, const auto &val) {
						ObjOuter &obj = ObjOuter::instance;
						if (stack_pos == stack.size()) {
							auto ctx = create_context<Outer>(obj, val);
							if (ctx.type != MT_unknown) {
								stack.emplace_back() = std::move(ctx);
							}
						}
						else if (stack_pos < stack.size()) {
							context &ctx = stack[stack_pos];
							obj_member &props = obj.props[ctx.object];
							if (props.child != null) {
								obj_object &sub = obj.children[props.child];
								sub.push_back(stack, stack_pos + 1, val);
							}
						}
					};
				}

				template<typename Prop>
				static auto create_visitor() {
					using obj_ptr = member_ptr<Outer, Prop>;
					return [](const ObjOuter &obj, schema::visitor &v, Outer &outer, const std::string_view &outer_name) {
						if constexpr (std::is_same_v<Outer, Prop>) {
							v.begin_object(outer_name);
							for (const auto& prop_id: obj.prop_lookup) {
								const obj_member &prop = obj.props[prop_id.second];
								if (prop.child != null) {
									const obj_object &sub = obj.children[prop.child];
									sub.visit(obj, v, outer, prop.name);
								}
								else if (prop.primitive != null) {
									const obj_prim &sub = obj.primitives[prop.primitive];
									sub.visit(obj, v, outer, prop_id.second);
								}
							}
							v.end_object(outer_name);
						}
					};
				}

				static obj_vtable create() {
					obj_vtable vtable{};

					auto action = create_set_value();
					vtable.set_bool = action;
					vtable.set_char = action;
					vtable.set_int16 = action;
					vtable.set_int32 = action;
					vtable.set_int64 = action;
					vtable.set_uint8 = action;
					vtable.set_uint16 = action;
					vtable.set_uint32 = action;
					vtable.set_uint64 = action;
					vtable.set_float = action;
					vtable.set_double = action;
					vtable.set_string = action;

					auto push_back_handler = create_push_back();
					vtable.push_back = push_back_handler;
					vtable.push_back_array = push_back_handler;

					vtable.visit = create_visitor<Outer>();

					return vtable;
				}
			};

			// Container / Array vtable creation
			template<typename>
			struct SelfArrayVtableHelper;

			template<template<class, class...> typename OuterContainer, typename Type, typename... Rest>
			struct SelfArrayVtableHelper<OuterContainer<Type, Rest...>> {
				using Outer = OuterContainer<Type, Rest...>;
				static_assert(member_type_trait_v<Outer> == MT_array, "Self Array VTable helper should only be used with array types");
				using ObjOuter = object<Outer>;
				using obj_vtable = self_vtable<Outer, ObjOuter>;
				using obj_member = member<Outer, ObjOuter>;
				using obj_prim = member_primitive<Outer, ObjOuter>;
				using obj_object = member_object<Outer, ObjOuter>;

				template<typename Prop>
				static auto create_set_value() {
					return [](const ObjOuter &obj, context_stack &stack, int stack_pos, Outer &outer, const auto &value) {
						context &ctx{ stack[stack_pos] };

						if (outer.size() <= ctx.id.idx) {
							outer.resize(ctx.id.idx + 1);
						}
						auto &&sub = outer[ctx.id.idx];

						if (stack_pos == stack.size() - 1) {
							// this is a as deep as we need to go.
							const obj_member &prop = obj.props[ctx.object];
							const obj_prim &prim = obj.primitives[prop.primitive];
							prim.set_value(obj, ctx, outer, ctx.object, value);
						}
						else {
							if constexpr (member_type_trait_v<Prop> >= MT_object) {
								object<Prop> &objY = object<Prop>::instance;
								objY.self.set_value(objY, stack, stack_pos + 1, sub, value);
							}
						}
					};
				}

				template<typename Prop>
				static auto create_push_back() {
					return [](context_stack &stack, int stack_pos, const auto &val) {
						if constexpr (member_type_trait_v<Prop> >= MT_object) {
							using type_obj = object<Prop>;
							type_obj &objY = type_obj::instance;
							if (stack_pos == stack.size()) {
								auto ctx = create_context<Prop>(objY, val);
								if (ctx.type != MT_unknown) {
									stack.emplace_back() = std::move(ctx);
								}
							}
							else if (stack_pos < stack.size()) {
								// Grab the child member helper
								objY.self.push_back(stack, stack_pos + 1, val);
							}
						}
						else {
							// Add this to the end of the stack since we don't want to keep passing the values
							// around; since they could be different depending on if they are Object, Array, or Maps.
							if (stack_pos != stack.size()) {
								return;
							}

							stack.emplace_back() = create_context<Prop>(val);
						}
					};
				}

				template<typename Prop>
				static auto create_visitor() {
					using obj_ptr = member_ptr<Outer, Prop>;
					return [](const ObjOuter &obj, schema::visitor &v, Outer &outer, const std::string_view &outer_name) {
						v.begin_array(outer_name);
						const obj_member &prop = obj.props[0];
						if (prop.primitive != null) {
							const obj_prim &prim = obj.primitives[prop.primitive];
							prim.visit(obj, v, outer, prop.primitive);
						}
						else {
							const obj_object &sub = obj.children[prop.child];
							sub.visit(obj, v, outer, outer_name);
						}
						v.end_array(outer_name);
					};
				}

				static obj_vtable create() {
					obj_vtable vtable{};

					auto action = create_set_value<Type>();
					vtable.set_bool = action;
					vtable.set_char = action;
					vtable.set_int16 = action;
					vtable.set_int32 = action;
					vtable.set_int64 = action;
					vtable.set_uint8 = action;
					vtable.set_uint16 = action;
					vtable.set_uint32 = action;
					vtable.set_uint64 = action;
					vtable.set_float = action;
					vtable.set_double = action;
					vtable.set_string = action;

					auto push_back_handler = create_push_back<Type>();
					vtable.push_back = push_back_handler;
					vtable.push_back_array = push_back_handler;

					vtable.visit = create_visitor<Type>();

					return vtable;
				}
			};

			// Map vtable creation
			template<typename>
			struct SelfMapVtableHelper;

			template<template<class, class, class...> typename OuterContainer, typename Key, typename Type, typename... Rest>
			struct SelfMapVtableHelper<OuterContainer<Key, Type, Rest...>> {
				using Outer = OuterContainer<Key, Type, Rest...>;
				static_assert(member_type_trait_v<Outer> == MT_map, "Self Map VTable helper should only be used with map types");
				using ObjOuter = object<Outer>;
				using obj_vtable = self_vtable<Outer, ObjOuter>;
				using obj_member = member<Outer, ObjOuter>;
				using obj_prim = member_primitive<Outer, ObjOuter>;
				using obj_object = member_object<Outer, ObjOuter>;

				template<typename Prop>
				static auto create_set_value() {
					return [](const ObjOuter &obj, context_stack &stack, int stack_pos, Outer &outer, const auto &value) {
						context &ctx{ stack[stack_pos] };
						auto &&sub = outer[std::string(ctx.id.name)];
						if (stack_pos == stack.size() - 1) {
							// this is a as deep as we need to go.
							const obj_member &prop = obj.props[ctx.object];
							const obj_prim &prim = obj.primitives[prop.primitive];
							prim.set_value(obj, ctx, outer, ctx.object, value);
						}
						else {
							if constexpr (member_type_trait_v<Prop> >= MT_object) {
								object<Prop> &objY = object<Prop>::instance;
								objY.self.set_value(objY, stack, stack_pos + 1, sub, value);
							}
						}
					};
				}

				template<typename Prop>
				static auto create_push_back() {
					return [](context_stack &stack, int stack_pos, const auto &val) {
						using ValueType = std::remove_cv_t<std::remove_reference_t<decltype(val)>>;
						if constexpr (member_type_trait_v<Prop> >= MT_object) {
							if (stack_pos == stack.size()) {
								if constexpr (std::is_same_v<ValueType, std::string_view>) {
									auto ctx = create_map_context<Prop>(val);
									stack.emplace_back() = std::move(ctx);
								}
							}
							else if (stack_pos < stack.size()) {
								object<Prop> &objY = object<Prop>::instance;
								objY.self.push_back(stack, stack_pos + 1, val);
							}
						}
						else {
							// Add this to the end of the stack since we don't want to keep passing the values
							// around; since they could be different depending on if they are Object, Array, or Maps.
							if (stack_pos != stack.size()) {
								return;
							}

							if constexpr (std::is_same_v<ValueType, std::string_view>) {
								stack.emplace_back() = create_map_context<Prop>(val);
							}
						}
					};
				}

				template<typename Prop>
				static auto create_visitor() {
					return [](const ObjOuter &obj, schema::visitor &v, Outer &outer, const std::string_view &outer_name) {
						v.begin_object(outer_name);
						const obj_member &prop = obj.props[0];
						if (prop.primitive != null) {
							const obj_prim &prim = obj.primitives[prop.primitive];
							prim.visit(obj, v, outer, prop.primitive);
						}
						else {
							if constexpr (member_type_trait_v<Prop> >= MT_object) {
								const obj_object &sub = obj.children[prop.child];
								sub.visit(obj, v, outer, outer_name);
							}
						}
						v.end_object(outer_name);
					};
				}

				static obj_vtable create() {
					obj_vtable vtable{};

					auto action = create_set_value<Type>();
					vtable.set_bool = action;
					vtable.set_char = action;
					vtable.set_int16 = action;
					vtable.set_int32 = action;
					vtable.set_int64 = action;
					vtable.set_uint8 = action;
					vtable.set_uint16 = action;
					vtable.set_uint32 = action;
					vtable.set_uint64 = action;
					vtable.set_float = action;
					vtable.set_double = action;
					vtable.set_string = action;

					auto push_back_handler = create_push_back<Type>();
					vtable.push_back = push_back_handler;
					vtable.push_back_array = push_back_handler;

					vtable.visit = create_visitor<Type>();

					return vtable;
				}
			};

			template<typename Outer>
			self_vtable<Outer, object<Outer>> create_self_vtable(member_object_type<Outer>) {
				if constexpr (member_type_trait_v<Outer> < MT_object) {
					static_assert(member_type_trait_v<Outer> >= MT_object, "Self type must be object to create vtable");
				}

				else if constexpr (member_type_trait_v<Outer> == MT_object) {
					return SelfObjectVtableHelper<Outer>::create();
				}
				else if constexpr (member_type_trait_v<Outer> == MT_array) {
					return SelfArrayVtableHelper<Outer>::create();
				}
				else if constexpr (member_type_trait_v<Outer> == MT_map) {
					return SelfMapVtableHelper<Outer>::create();
				}

				return self_vtable<Outer, object<Outer>>{};
			}

			template<typename Outer, typename ObjType>
			template<typename X>
			void self_vtable<Outer, ObjType>::set_value(const ObjType &obj, context_stack &stack, int stack_pos, Outer &outer, const X &value) const {
				if constexpr (std::is_same_v<X, bool>) {
					set_bool(obj, stack, stack_pos, outer, value);
				}
				else if constexpr (std::is_same_v<X, char>) {
					set_char(obj, stack, stack_pos, outer, value);
				}
				else if constexpr (std::is_same_v<X, int16>) {
					set_int16(obj, stack, stack_pos, outer, value);
				}
				else if constexpr (std::is_same_v<X, int32>) {
					set_int32(obj, stack, stack_pos, outer, value);
				}
				else if constexpr (std::is_same_v<X, int64>) {
					set_int64(obj, stack, stack_pos, outer, value);
				}
				else if constexpr (std::is_same_v<X, uint8>) {
					set_uint8(obj, stack, stack_pos, outer, value);
				}
				else if constexpr (std::is_same_v<X, uint16>) {
					set_uint16(obj, stack, stack_pos, outer, value);
				}
				else if constexpr (std::is_same_v<X, uint32>) {
					set_uint32(obj, stack, stack_pos, outer, value);
				}
				else if constexpr (std::is_same_v<X, uint64>) {
					set_uint64(obj, stack, stack_pos, outer, value);
				}
				else if constexpr (std::is_same_v<X, float>) {
					set_float(obj, stack, stack_pos, outer, value);
				}
				else if constexpr (std::is_same_v<X, double>) {
					set_double(obj, stack, stack_pos, outer, value);
				}
				else if constexpr (std::is_same_v<X, std::string>) {
					set_string(obj, stack, stack_pos, outer, value);
				}
			}
		}    // namespace detail
	}    // namespace schema
}    // namespace tc

// Object
namespace tc {
	namespace schema {
		namespace detail {
			template<typename Outer, typename ObjType>
			self_object<Outer, ObjType>::self_object()
				: type_id{ type_identifier<Outer>() }
				, vtable{ create_self_vtable(member_object_type_v<Outer>) } {
			}

			template<typename Outer, typename ObjType>
			void self_object<Outer, ObjType>::push_back(context_stack &stack, int stack_pos, const std::string_view &name) const {
				vtable.push_back(stack, stack_pos, name);
			}

			template<typename Outer, typename ObjType>
			void self_object<Outer, ObjType>::push_back(context_stack &stack, int32 stack_pos, int32 array_idx) const {
				vtable.push_back_array(stack, stack_pos, array_idx);
			}

			template<typename Outer, typename ObjType>
			template<typename Value>
			void self_object<Outer, ObjType>::set_value(const ObjType &obj, context_stack &stack, int stack_pos, Outer &outer, const Value &value) const {
				vtable.set_value<Value>(obj, stack, stack_pos, outer, value);
			}

			template<typename Outer, typename ObjType>
			void self_object<Outer, ObjType>::visit(const ObjType &obj, schema::visitor &v, Outer &outer, const std::string_view &outer_name) const {
				vtable.visit(obj, v, outer, outer_name);
			}
		}    // namespace detail
	}    // namespace schema
}    // namespace tc
