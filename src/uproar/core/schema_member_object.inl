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
			context create_context(object<Type> &obj, std::string_view name) {
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

			template<typename Outer, typename Prop>
			member_object_vtable<Outer, object<Outer>> create_member_object_vtable(member_object_type<Prop>) {
				using ObjOuter = object<Outer>;
				member_object_vtable<Outer, ObjOuter> vtable;

				// Set value handler
				{
					auto action = [](ObjOuter &obj, context_stack &stack, int stack_pos, Outer &outer, std::string_view name, const auto &value) {
						if constexpr (member_type_trait_v<Prop> >= MT_object) {
							using ValueType = decltype(value);
							if (stack_pos == stack.size()) {
								if constexpr (member_type_trait_v<Outer> == MT_array) {
									context &ctx = stack[stack_pos - 1];
									if constexpr (member_type_trait_v<Prop> >= MT_object) {
										object<Prop> &objY = object<Prop>::instance;
										auto iter = objY.prop_lookup.find(name);
										if (iter == std::end(objY.prop_lookup)) {
											return;
										}

										id_type prop_idx = iter->second;
										member<Prop, object<Prop>> &props = objY.props[prop_idx];
										member_primitive<Prop, object<Prop>> &prim = objY.primitives[props.primitive];
										prim.set_value(objY, ctx, outer[ctx.id.idx], prop_idx, value);
									}
								}

								else if constexpr (member_type_trait_v<Prop> < MT_object && std::is_convertible_v<ValueType, Prop>) {
									context &ctx = stack[stack_pos - 1];
									object<Prop> &objY = object<Prop>::instance;
									member<Prop, object<Prop>> &props = objY.props[ctx.object];

									auto iter = objY.prop_lookup.find(name);
									if (iter == std::end(objY.prop_lookup)) {
										return;
									}

									id_type prop_idx = iter->second;
									member_ptr<Outer, Prop> ptr = obj.prop_ptrs<Prop>[prop_idx];
									member_primitive<Prop, object<Prop>> &prim = objY.primitives[props.primitive];
									prim.set_value(stack, stack_pos, outer.*ptr, prop_idx, value);
									return;
								}
							}
							else if (stack_pos < stack.size()) {
								if constexpr (member_type_trait_v<Outer> == MT_array) {
									context &array_ctx = stack[stack_pos - 1];
									context &ctx = stack[stack_pos];
									if constexpr (member_type_trait_v<Prop> >= MT_object) {
										object<Prop> &objY = object<Prop>::instance;
										auto iter = objY.prop_lookup.find(ctx.id.name);
										if (iter == std::end(objY.prop_lookup)) {
											return;
										}

										id_type prop_idx = iter->second;
										member<Prop, object<Prop>> &props = objY.props[prop_idx];
										member_object<Prop, object<Prop>> &sub = objY.children[props.child];
										sub.set_value(objY, stack, stack_pos, outer[array_ctx.id.idx], name, value);
									}
								}
								else if constexpr (member_type_trait_v<Prop> >= MT_object) {
									context &ctx = stack[stack_pos];

									member<Outer, object<Outer>> &props = obj.props[ctx.object];
									member_ptr<Outer, Prop> ptr = obj.prop_ptrs<Prop>[props.ptr];

									object<Prop> &objY = object<Prop>::instance;
									auto iter = objY.prop_lookup.find(ctx.id.name);
									if (iter == std::end(obj.prop_lookup)) {
										return;
									}

									id_type prop_idx = iter->second;
									member<Prop, object<Prop>> &propsY = objY.props[prop_idx];
									member_object<Prop, object<Prop>> &sub = objY.children[propsY.child];
									sub.set_value(objY, stack, stack_pos + 1, outer.*ptr, name, value);
								}
							}
						}
					};

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
				}

				// Push back handler
				{
					auto push_back_handler = [](context_stack &stack, int stack_pos, auto val) {
						if constexpr (member_type_trait_v<Prop> >= MT_object) {
							if (stack_pos == stack.size()) {
								// This isn't a valid push_back if this isn't an object type
								// When pushing a name it means we already have an index we are now
								// specifying that we want a specific obeject from within the container
								auto ctx = create_context<Prop>(object<Prop>::instance, std::move(val));
								if (ctx.type != MT_unknown) {
									stack.emplace_back() = std::move(ctx);
								}
							}
							else if (stack_pos < stack.size()) {
								context &ctx = stack[stack_pos];
								object<Prop> &objY = object<Prop>::instance;
								member<Prop, object<Prop>> &props = objY.props[ctx.object];
								member_object<Prop, object<Prop>> &sub = objY.children[props.child];
								sub.push_back(stack, stack_pos + 1, std::move(val));
							}
						}
					};
					vtable.push_back = push_back_handler;
					vtable.push_back_array = push_back_handler;
				}

				return vtable;
			}

			// Container / Array vtable creation
			template<typename Outer, template<class, class...> typename Prop, typename Type, typename... Rest>
			member_object_vtable<Outer, object<Outer>> create_array_object_vtable(member_object_type<Prop<Type, Rest...>>) {
				using ObjOuter = object<Outer>;
				using PropType = Prop<Type, Rest...>;
				member_object_vtable<Outer, ObjOuter> vtable;

				// Set value handler
				{
					static auto action = [](ObjOuter &obj, context_stack &stack, int stack_pos, Outer &outer, std::string_view name, const auto &value) {
						if constexpr(member_type_trait_v<PropType> == MT_array) {
							using ValueType = decltype(value);
							if constexpr(member_type_trait_v<Type> < MT_object) {
								if (stack_pos < stack.size()) {
									if constexpr (member_type_trait_v<Outer> == MT_object) {
										context &ctx = stack[stack_pos];
										auto iter = obj.prop_lookup.find(ctx.id.name);
										if (iter == std::end(obj.prop_lookup)) {
											return;
										}

										id_type prop_idx = iter->second;
										member<Outer, object<Outer>> &props = obj.props[prop_idx];
										member_ptr<Outer, PropType> ptr = obj.prop_ptrs<PropType>[props.ptr];

										object<PropType> &objY = object<PropType>::instance;
										member_object<PropType, object<PropType>> &sub = objY.children[0];
										sub.set_value(objY, stack, stack_pos + 1, outer.*ptr, name, value);
									}
									else {
										context &ctx = stack[stack.size() - 1];
										// Always should be the top of the stack because you can't index in to a primitive
										member_primitive<Outer, object<Outer>> &primitive = obj.primitives[0];
										
										primitive.set_value(obj, ctx, outer, 0, value);
									}
								}
								else if (stack_pos == stack.size()) {
									context &ctx = stack[stack_pos - 1];
									// Always should be the top of the stack because you can't index in to a primitive
									member_primitive<Outer, object<Outer>> &primitive = obj.primitives[0];
									
									primitive.set_value(obj, ctx, outer, 0, value);
								}
							}
							else if constexpr(member_type_trait_v<Type> >= MT_object) {
								if (stack_pos < stack.size()) {
									// We are moving one deeper in the stack
									context &ctx = stack[stack_pos];
									auto &v = outer;
									if (ctx.id.idx >= v.size()) {
										v.resize(ctx.id.idx + 1);
									}

									member<Outer, ObjOuter> &mem = obj.props[1];
									member_object<Outer, ObjOuter> &sub =  obj.children[mem.child];
									sub.set_value(obj, stack, stack_pos + 1, v, name, value);
								}
							}
						}
					};

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
				}

				// Push back handler
				{
					auto push_back_handler = [](context_stack &stack, int stack_pos, auto val) {
						if constexpr (member_type_trait_v<Type> >= MT_object) {
							if (stack_pos == stack.size()) {
								// This isn't a valid push_back if this isn't an object type
								// When pushing a name it means we already have an index we are now
								// specifying that we want a specific obeject from within the container
								auto ctx = create_context<Type>(object<Type>::instance, std::move(val));
								if (ctx.type != MT_unknown) {
									stack.emplace_back() = std::move(ctx);
								}
							}
							else if (stack_pos < stack.size()) {
								context &ctx = stack[stack_pos];
								object<Type> &objY = object<Type>::instance;
								member<Type, object<Type>> &props = objY.props[ctx.object];
								member_object<Type, object<Type>> &sub = objY.children[props.child];
								sub.push_back(stack, stack_pos + 1, std::move(val));
							}
						}
						else if constexpr (std::is_same_v<decltype(val), int32>) {
							if (stack_pos == stack.size()) {
								// This isn't a valid push_back if this isn't an object type
								// When pushing a name it means we already have an index we are now
								// specifying that we want a specific obeject from within the container
								context ctx{};
								ctx.object = 0;
								ctx.type = member_type_trait_v<Type>;
								ctx.id.idx = val;
								ctx.member_context.resize(1);
								stack.emplace_back() = std::move(ctx);
							}
						}
					};

					vtable.push_back = push_back_handler;
					vtable.push_back_array = push_back_handler;
				}

				return vtable;
			}

			template<typename Outer, typename Prop>
			member_object_vtable<Outer, object<Outer>> create_object_vtable(member_object_type<Prop>) {
				if constexpr (member_type_trait_v<Prop> == MT_object) {
					return create_member_object_vtable<Outer>(member_object_type_v<Prop>);
				}
				else if constexpr (member_type_trait_v<Prop> == MT_array) {
					return create_array_object_vtable<Outer>(member_object_type_v<Prop>);
				}
				else {
					static_assert(member_type_trait_v<Prop> >= MT_array, "Member type must be object to create vtable");
					return member_object_vtable<Outer, object<Outer>>{};
				}
			}

			template<typename Outer, typename ObjType>
			template<typename X>
			void member_object_vtable<Outer, ObjType>::set_value(ObjType &obj, context_stack &stack, int stack_pos, Outer &outer, std::string_view name, const X &value) {
				if constexpr (std::is_same_v<X, bool>) {
					set_bool(obj, stack, stack_pos, outer, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, char>) {
					set_char(obj, stack, stack_pos, outer, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, int16>) {
					set_int16(obj, stack, stack_pos, outer, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, int32>) {
					set_int32(obj, stack, stack_pos, outer, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, int64>) {
					set_int64(obj, stack, stack_pos, outer, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, uint8>) {
					set_uint8(obj, stack, stack_pos, outer, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, uint16>) {
					set_uint16(obj, stack, stack_pos, outer, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, uint32>) {
					set_uint32(obj, stack, stack_pos, outer, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, uint64>) {
					set_uint64(obj, stack, stack_pos, outer, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, float>) {
					set_float(obj, stack, stack_pos, outer, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, double>) {
					set_double(obj, stack, stack_pos, outer, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, std::string>) {
					set_string(obj, stack, stack_pos, outer, std::move(name), value);
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
				: vtable{ create_object_vtable<Outer, Y>(member_object_type_v<Y>) } {
			}

			template<typename Outer, typename ObjType>
			void member_object<Outer, ObjType>::push_back(context_stack &stack, int stack_pos, std::string_view name) {
				vtable.push_back(stack, stack_pos, std::move(name));
			}

			template<typename Outer, typename ObjType>
			void member_object<Outer, ObjType>::push_back(context_stack &stack, int32 stack_pos, int32 array_idx) {
				vtable.push_back_array(stack, stack_pos, array_idx);
			}

			template<typename Outer, typename ObjType>
			template<typename Value>
			void member_object<Outer, ObjType>::set_value(ObjType &obj, context_stack &stack, int stack_pos, Outer &outer, std::string_view name, const Value &value) {
				vtable.set_value<Value>(obj, stack, stack_pos, outer, std::move(name), value);
			}
		}    // namespace detail
	}    // namespace schema
}    // namespace tc
