#include "schema_member_object.hpp"
#include "schema_member_primitive.hpp"

namespace tc {
	namespace schema {
		namespace detail {
			template<typename Type>
			context create_context(object<Type> &obj, uint32 array_idx) {
				context ctx{};
				ctx.object = 0;
				ctx.type = member_type_trait_v<Type>;
				ctx.id.idx = array_idx;
				ctx.member_context.resize(obj.props.size());
				return ctx;
			}

			template<typename Type>
			context create_context(uint32 array_idx) {
				context ctx{};
				ctx.object = 0;
				ctx.type = member_type_trait_v<Type>;
				ctx.id.idx = array_idx;
				ctx.member_context.resize(1);
				return ctx;
			}

			template<typename Type>
			context create_context(const std::string_view &name) {
				context ctx{};
				ctx.object = 0;
				ctx.type = member_type_trait_v<Type>;
				ctx.id.name = name;
				ctx.member_context.resize(1);
				return ctx;
			}

			template<typename Type>
			context create_map_context(const std::string_view &name) {
				context ctx{};
				ctx.object = 0;
				ctx.type = member_type_trait_v<Type>;
				ctx.id.name = name;
				ctx.member_context.resize(1);
				return ctx;
			}

			template<typename Type>
			context create_context(object<Type> &obj, std::string_view name) {
				if constexpr (member_type_trait_v<Type> == MT_object) {
					auto iter = obj.prop_lookup.find(name);
					if (iter == std::end(obj.prop_lookup)) {
						context ctx{};
						ctx.type = MT_unknown;
						return ctx;
					}

					context ctx{};
					ctx.object = iter->second;
					ctx.type = obj.props[iter->second].type;
					ctx.id.name = obj.props[iter->second].name;
					ctx.member_context.resize(obj.props.size());
					return ctx;
				}
				else if constexpr (member_type_trait_v<Type> == MT_map) {
					context ctx{};
					ctx.object = 0;
					ctx.type = obj.props.empty() ? member_type_trait_v<Type> : obj.props[0].type;
					ctx.id.name = name;
					ctx.member_context.resize(1);
					return ctx;
				}
				else {
					context ctx{};
					ctx.type = MT_unknown;
					return ctx;
				}
			}

			// Container / Array vtable creation
			template<typename>
			struct ObjectVtableHelper;

			template<typename Outer>
			struct ObjectVtableHelper {
				static_assert(member_type_trait_v<Outer> == MT_object, "Object VTable helper should only be used with object types");
				using ObjOuter = object<Outer>;
				using obj_vtable = member_object_vtable<Outer, ObjOuter>;
				using obj_member = member<Outer, ObjOuter>;
				using obj_prim = member_primitive<Outer, ObjOuter>;
				using obj_object = member_object<Outer, ObjOuter>;

				template<typename Prop>
				static auto create_set_value() {
					return [](const ObjOuter &obj, context_stack &stack, int stack_pos, Outer &outer, const auto &value) {
						if constexpr (member_type_trait_v<Prop> >= MT_object) {
							using type_obj = object<Prop>;
							using type_member = member<Prop, type_obj>;
							using type_primitive = member_primitive<Prop, type_obj>;
							using type_sub_object = member_object<Prop, type_obj>;
							using type_ptr = member_ptr<Outer, Prop>;
							// If it's an object underneath retrieve and move the stack along
							context &ctx = stack[stack_pos];
							const obj_member &prop = obj.props[ctx.object];
							const type_ptr ptr = obj.prop_ptrs<Prop>[prop.ptr];

							auto &sub = outer.*ptr;

							object<Prop> &objY = object<Prop>::instance;
							objY.self.set_value(objY, stack, stack_pos + 1, sub, value);
						}
					};
				}

				template<typename Prop>
				static auto create_push_back() {
					return [](context_stack &stack, int stack_pos, const auto &val) {
						if constexpr (member_type_trait_v<Prop> >= MT_object) {
							using type_obj = object<Prop>;
							using type_member = member<Prop, type_obj>;
							using type_sub_object = member_object<Prop, type_obj>;
							if (stack_pos == stack.size()) {
								auto ctx = create_context<Prop>(type_obj::instance, val);
								if (ctx.type != MT_unknown) {
									stack.emplace_back() = std::move(ctx);
								}
							}
							else if (stack_pos < stack.size()) {
								context &ctx = stack[stack_pos];
								type_obj &objY = type_obj::instance;
								type_member &props = objY.props[ctx.object];
								type_sub_object &sub = objY.children[props.child];
								sub.push_back(stack, stack_pos + 1, val);
							}
						}
					};
				}

				template<typename Prop>
				static auto create_visitor() {
					using obj_ptr = member_ptr<Outer, Prop>;
					return [](const ObjOuter &obj, schema::visitor &v, Outer &outer, const std::string_view &outer_name) {
						if constexpr (member_type_trait_v<Prop> >= MT_object) {
							using type_obj = object<Prop>;
							using type_member = member<Prop, type_obj>;
							using type_sub_object = member_object<Prop, type_obj>;
							using type_ptr = member_ptr<Outer, Prop>;

							auto prop_id = obj.prop_lookup.find(outer_name);
							if (prop_id == std::end(obj.prop_lookup)) {
								return;
							}
							const obj_member &prop = obj.props.at(prop_id->second);
							type_ptr ptr = obj.prop_ptrs<Prop>[prop.ptr];
							auto &sub = outer.*ptr;

							object<Prop> &objY = object<Prop>::instance;
							objY.self.visit(objY, v, sub, prop.name);
						}
					};
				}

				template<typename Prop>
				static obj_vtable create(const member_object_type<Prop> &) {
					obj_vtable vtable{};

					auto action = create_set_value<Prop>();
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

					auto push_back_handler = create_push_back<Prop>();
					vtable.push_back = push_back_handler;
					vtable.push_back_array = push_back_handler;

					vtable.visit = create_visitor<Prop>();

					return vtable;
				}
			};

			// Container / Array vtable creation
			template<typename>
			struct ArrayVtableHelper;

			template<template<class, class...> typename OuterContainer, typename Type, typename... Rest>
			struct ArrayVtableHelper<OuterContainer<Type, Rest...>> {
				using Outer = OuterContainer<Type, Rest...>;
				static_assert(member_type_trait_v<Outer> == MT_array, "Array VTable helper should only be used with array types");
				using ObjOuter = object<Outer>;
				using obj_vtable = member_object_vtable<Outer, ObjOuter>;
				using obj_member = member<Outer, ObjOuter>;
				using obj_prim = member_primitive<Outer, ObjOuter>;
				using obj_object = member_object<Outer, ObjOuter>;

				template<typename Prop>
				static auto create_set_value() {
					return [](const ObjOuter &obj, context_stack &stack, int stack_pos, Outer &outer, const auto &value) {
						if constexpr (member_type_trait_v<Prop> >= MT_object) {
							using type_obj = object<Prop>;
							using type_member = member<Prop, type_obj>;
							using type_primitive = member_primitive<Prop, type_obj>;
							using type_sub_object = member_object<Prop, type_obj>;
							using type_ptr = member_ptr<Outer, Prop>;
							// If it's an object underneath retrieve and move the stack along
							context &ctx = stack[stack_pos];
							const obj_member &prop = obj.props[ctx.object];

							auto &arr = outer;
							if (ctx.id.idx <= arr.size()) {
								arr.resize(ctx.id.idx + 1);
							}
							auto &sub_obj = arr[ctx.id.idx];

							type_obj &objY = type_obj::instance;
							type_member &sub_prop = objY.props[0];
							type_sub_object &sub = objY.children[sub_prop.child];
							sub.set_value(objY, stack, stack_pos + 1, sub_obj, value);
						}

						else if constexpr (member_type_trait_v<Prop> < MT_object) {
							// If it's an object underneath retrieve and move the stack along
							context &ctx = stack[stack_pos];
							auto &arr = outer;

							if (ctx.id.idx <= arr.size()) {
								arr.resize(ctx.id.idx + 1);
							}

							const obj_member &props = obj.props[ctx.object];
							const obj_prim &sub = obj.primitives[props.primitive];
							sub.set_value(obj, ctx, arr, ctx.object, value);
						}
					};
				}

				template<typename Prop>
				static auto create_push_back() {
					return [](context_stack &stack, int stack_pos, const auto &val) {
						if constexpr (member_type_trait_v<Prop> >= MT_object) {
							using type_obj = object<Prop>;
							using type_member = member<Prop, type_obj>;
							using type_sub_object = member_object<Prop, type_obj>;
							type_obj &objY = type_obj::instance;
							if (stack_pos == stack.size()) {
								auto ctx = create_context<Prop>(objY, val);
								if (ctx.type != MT_unknown) {
									stack.emplace_back() = std::move(ctx);
								}
							}
							else if (stack_pos < stack.size()) {
								// Grab the child member helper
								type_member &props = objY.props[0];
								type_sub_object &sub = objY.children[props.child];
								sub.push_back(stack, stack_pos + 1, val);
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
						if constexpr (member_type_trait_v<Prop> >= MT_object) {
							const obj_member &prop = obj.props[0];
							object<Prop> &objY = object<Prop>::instance;
							for (auto &&item : outer) {
								const static constexpr std::string_view array_item_name{ "" };
								objY.self.visit(objY, v, item, array_item_name);
							}
						}
					};
				}

				template<typename Prop>
				static obj_vtable create(const member_object_type<Prop> &) {
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
			struct MapVtableHelper;

			template<template<class, class, class...> typename OuterContainer, typename Key, typename Type, typename... Rest>
			struct MapVtableHelper<OuterContainer<Key, Type, Rest...>> {
				using Outer = OuterContainer<Key, Type, Rest...>;
				static_assert(member_type_trait_v<Outer> == MT_map, "Map VTable helper should only be used with map types");
				using ObjOuter = object<Outer>;
				using obj_vtable = member_object_vtable<Outer, ObjOuter>;
				using obj_member = member<Outer, ObjOuter>;
				using obj_prim = member_primitive<Outer, ObjOuter>;
				using obj_object = member_object<Outer, ObjOuter>;

				template<typename Prop>
				static auto create_set_value() {
					return [](const ObjOuter &obj, context_stack &stack, int stack_pos, Outer &outer, const auto &value) {
						if constexpr (member_type_trait_v<Prop> >= MT_object) {
							using type_obj = object<Prop>;
							using type_member = member<Prop, type_obj>;
							using type_primitive = member_primitive<Prop, type_obj>;
							using type_sub_object = member_object<Prop, type_obj>;
							using type_ptr = member_ptr<Outer, Prop>;
							// If it's an object underneath retrieve and move the stack along
							context &ctx = stack[stack_pos];
							const obj_member &prop = obj.props[ctx.object];

							auto &arr = outer;
							auto &sub_obj = arr[std::string(ctx.id.name)];

							type_obj &objY = type_obj::instance;
							type_member &sub_prop = objY.props[0];
							type_sub_object &sub = objY.children[sub_prop.child];
							sub.set_value(objY, stack, stack_pos + 1, sub_obj, value);
						}

						else if constexpr (member_type_trait_v<Prop> < MT_object) {
							// If it's an object underneath retrieve and move the stack along
							context &ctx = stack[stack_pos];
							auto &arr = outer;

							obj_member &props = obj.props[ctx.object];
							obj_prim &sub = obj.primitives[props.primitive];
							sub.set_value(obj, ctx, arr, ctx.object, value);
						}
					};
				}

				template<typename Prop>
				static auto create_push_back() {
					return [](context_stack &stack, int stack_pos, const auto &val) {
						using ValueType = std::remove_cv_t<std::remove_reference_t<decltype(val)>>;
						if constexpr (member_type_trait_v<Prop> >= MT_object) {
							using type_obj = object<Prop>;
							using type_member = member<Prop, type_obj>;
							using type_sub_object = member_object<Prop, type_obj>;
							type_obj &objY = type_obj::instance;
							if (stack_pos == stack.size()) {
								if constexpr (std::is_same_v<ValueType, std::string_view>) {
									auto ctx = create_map_context<Prop>(val);
									stack.emplace_back() = std::move(ctx);
								}
							}
							else if (stack_pos < stack.size()) {
								// Grab the child member helper
								type_member &props = objY.props[0];
								type_sub_object &sub = objY.children[props.child];
								sub.push_back(stack, stack_pos + 1, val);
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
					using obj_ptr = member_ptr<Outer, Prop>;
					return [](const ObjOuter &obj, schema::visitor &v, Outer &outer, const std::string_view &outer_name) {
						if constexpr (member_type_trait_v<Prop> >= MT_object) {
							object<Prop> &objY = object<Prop>::instance;

							for (auto &&item : outer) {
								objY.self.visit(objY, v, item.second, item.first);
							}
						}
					};
				}

				template<typename Prop>
				static obj_vtable create(const member_object_type<Prop> &) {
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

			template<typename Outer, typename Prop>
			member_object_vtable<Outer, object<Outer>> create_object_vtable(member_object_type<Prop>) {
				if constexpr (member_type_trait_v<Prop> < MT_object) {
					static_assert(member_type_trait_v<Outer> >= MT_object, "Member type must be object to create vtable");
				}

				else if constexpr (member_type_trait_v<Outer> == MT_object) {
					return ObjectVtableHelper<Outer>::create(member_object_type_v<Prop>);
				}
				else if constexpr (member_type_trait_v<Outer> == MT_array) {
					return ArrayVtableHelper<Outer>::create(member_object_type_v<Prop>);
				}
				else if constexpr (member_type_trait_v<Outer> == MT_map) {
					return MapVtableHelper<Outer>::create(member_object_type_v<Prop>);
				}

				return member_object_vtable<Outer, object<Outer>>{};
			}

			template<typename Outer, typename ObjType>
			template<typename X>
			void member_object_vtable<Outer, ObjType>::set_value(const ObjType &obj, context_stack &stack, int stack_pos, Outer &outer, const X &value) const {
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
			template<typename Y>
			member_object<Outer, ObjType>::member_object(ObjType &obj, member_object_type<Y>)
				: type_id{ type_identifier<Y>() }
				, vtable{ create_object_vtable<Outer>(member_object_type_v<Y>) } {
			}

			template<typename Outer, typename ObjType>
			void member_object<Outer, ObjType>::push_back(context_stack &stack, int stack_pos, const std::string_view &name) const {
				vtable.push_back(stack, stack_pos, name);
			}

			template<typename Outer, typename ObjType>
			void member_object<Outer, ObjType>::push_back(context_stack &stack, int32 stack_pos, int32 array_idx) const {
				vtable.push_back_array(stack, stack_pos, array_idx);
			}

			template<typename Outer, typename ObjType>
			template<typename Value>
			void member_object<Outer, ObjType>::set_value(const ObjType &obj, context_stack &stack, int stack_pos, Outer &outer, const Value &value) const {
				vtable.set_value<Value>(obj, stack, stack_pos, outer, value);
			}

			template<typename Outer, typename ObjType>
			void member_object<Outer, ObjType>::visit(const ObjType &obj, schema::visitor &v, Outer &outer, const std::string_view &outer_name) const {
				vtable.visit(obj, v, outer, outer_name);
			}
		}    // namespace detail
	}    // namespace schema
}    // namespace tc
