#include "schema_member_object.hpp"
#include "schema_member_primitive.hpp"

namespace tc {
	namespace schema {
		namespace detail {
			template<typename Outer, typename ObjType, typename Prop, std::enable_if_t<!is_container_v<Prop> && !is_map_v<Prop>>>
			member_object_vtable<Outer, ObjType> create_object_vtable(member_ptr<Outer, Prop>) {
				member_object_vtable<Outer, ObjType> vtable;
				auto action = [](ObjType &obj, context_stack &stack, int stack_pos, Outer &outer, id_type member_idx, std::string_view name, const auto &value) {
					if constexpr (member_type_trait_v<Prop> >= MT_object) {
						using ValueType = decltype(value);
						if constexpr (std::is_same_v<ValueType, Prop>) {
							auto ptr = obj.prop_ptrs<Prop>[member_idx];
							auto &objY = object<Prop>::instance;
							objY.set_value<Prop, ValueType>(stack, stack_pos, outer.*ptr, name, value);
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

				vtable.push_back = [](context_stack &stack, int stack_pos, std::string_view name) {
					if constexpr (member_type_trait_v<Prop> >= MT_object) {
						object<Prop>::instance.push_back(stack, stack_pos, std::move(name));
					}
				};

				return vtable;
			}

			// Container / Array vtable creation
			template<typename Outer, typename ObjType, typename Type, template<class, class...> typename Prop, typename ...Rest, typename = std::enable_if_t<is_container_v<Prop<Type, Rest...>> && !is_map_v<Prop<Type, Rest...>>>>
			member_object_vtable<Outer, ObjType> create_object_vtable(member_ptr<Outer, Prop<Type, Rest...>>) {
				using PropType = Prop<Type, Rest...>;
				member_object_vtable<Outer, ObjType> vtable;
				static auto action = [](ObjType &obj, context_stack &stack, int stack_pos, Outer &outer, id_type member_idx, std::string_view name, const auto &value) {
					// If we are here we should always be at the point of a stack that needs to be indexed.
					using ValueType = decltype(value);
					if constexpr (member_type_trait_v<Type> < MT_object) {
						if (std::is_convertible_v<ValueType, Type>) {
							context &ctx = stack[stack_pos];
							uint32 idx = ctx.id.idx;
							member<Outer, ObjType> &props = obj.props[member_idx];
							auto ptr = obj.prop_ptrs<PropType>[props.ptr];
							member_primitive<Outer, ObjType> &prim = obj.primitives[props.primitive];
							prim.set_value(obj, stack[stack_pos], outer, member_idx, value);
						}
					}
					else {
						uint32 idx = stack[stack_pos].id.idx;
						member<Outer, ObjType> &props = obj.props[member_idx];
						auto ptr = obj.prop_ptrs<PropType>[member_idx];
						member_object<Outer, ObjType> &sub = obj.children[props.child];

						object<Type> &objY = object<Type>::instance;

						sub.set_value(objY, stack, stack_pos + 1, (outer.*ptr)[idx], 0, std::move(name), value);
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

				vtable.push_back = [](context_stack &stack, int stack_pos, std::string_view name) {
					if (stack_pos == stack.size()) {
						context &ctx = stack.emplace_back();
						uint32 idx = std::stoul(std::string(name));
						ctx.object = 0;
						ctx.type = member_type_trait_v<Type>;
						ctx.id.idx = idx;
						ctx.member_context.emplace_back();
					}
					else if (stack_pos < stack.size()) {
						// This isn't a valid push_back if this isn't an object type
						if constexpr (member_type_trait_v<Type> >= MT_object) {
							object<Type> &objY = object<Type>::instance;
							auto &ctx = stack[stack_pos];
							objY.push_back(stack, stack_pos + 1, name);
						}
					}
				};

				return vtable;
			}

			template<typename Outer, typename ObjType>
			template<typename X>
			void member_object_vtable<Outer, ObjType>::set_value(ObjType &obj, context_stack &stack, int stack_pos, Outer &outer, id_type member_idx, std::string_view name, const X &value) {
				if constexpr (std::is_same_v<X, bool>) {
					set_bool(obj, stack, stack_pos, outer, member_idx, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, char>) {
					set_char(obj, stack, stack_pos, outer, member_idx, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, int16>) {
					set_int16(obj, stack, stack_pos, outer, member_idx, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, int32>) {
					set_int32(obj, stack, stack_pos, outer, member_idx, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, int64>) {
					set_int64(obj, stack, stack_pos, outer, member_idx, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, uint8>) {
					set_uint8(obj, stack, stack_pos, outer, member_idx, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, uint16>) {
					set_uint16(obj, stack, stack_pos, outer, member_idx, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, uint32>) {
					set_uint32(obj, stack, stack_pos, outer, member_idx, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, uint64>) {
					set_uint64(obj, stack, stack_pos, outer, member_idx, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, float>) {
					set_float(obj, stack, stack_pos, outer, member_idx, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, double>) {
					set_double(obj, stack, stack_pos, outer, member_idx, std::move(name), value);
				}
				else if constexpr (std::is_same_v<X, std::string>) {
					set_string(obj, stack, stack_pos, outer, member_idx, std::move(name), value);
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
			member_object<Outer, ObjType>::member_object(ObjType &obj, member_ptr<Outer, Y> mem)
				: vtable{ create_object_vtable<Outer, ObjType>(mem) } {
			}

			template<typename Outer, typename ObjType>
			void member_object<Outer, ObjType>::push_back(context_stack &stack, int stack_pos, std::string_view name) {
				vtable.push_back(stack, stack_pos, std::move(name));
			}

			template<typename Outer, typename ObjType>
			template<typename Value>
			void member_object<Outer, ObjType>::set_value(ObjType &obj, context_stack &stack, int stack_pos, Outer &outer, id_type member_idx, std::string_view name, const Value &value) {
				vtable.set_value<Value>(obj, stack, stack_pos, outer, member_idx, std::move(name), value);
			}
		}    // namespace detail
	}    // namespace schema
}    // namespace tc
