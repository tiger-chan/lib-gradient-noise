#include "schema_object.hpp"

namespace tc {
	namespace schema {
		namespace detail {
			template<typename Type>
			template<typename Y, typename... Z, template<class> typename... Constraint>
			void object<Type>::add_member(const char *name, member_ptr<Type, Y> mem_ptr, const char *desc, Constraint<Z> &&...constraints) {
				std::string_view n{ name };
				auto iter = instance.prop_lookup.find(n);
				if (iter != std::end(instance.prop_lookup)) {
					// Already been defined
					return;
				}

				instance.prop_lookup[n] = id_type(instance.props.size());
				obj_member member(instance, n, std::move(mem_ptr), desc, std::forward<Constraint<Z>>(constraints)...);
				instance.props.emplace_back(std::move(member));
			}

			template<typename X>
			object<X> object<X>::instance{};

			template<typename Type>
			template<typename Value>
			void object<Type>::set_value(context_stack &stack, Type &obj, std::string_view name, const Value &value) {
				set_value<Value>(stack, 0, obj, name, value);
			}

			template<typename Type>
			template<typename Prop, typename Value>
			void object<Type>::set_value(context_stack &stack, int stack_pos, Type &obj, std::string_view name, const Value &value) {
				if constexpr (std::is_enum_v<Prop> && std::is_enum_v<Value>) {
					std::string converted{ enum_to_string<Prop>::to_string(value) };
					set_value<Prop, std::string>(stack, stack_pos, obj, name, converted);
				}
				else {
					if (stack_pos < stack.size()) {
						auto &ctx = stack[stack_pos];
						auto &prop = instance.props[ctx.object];
						obj_forward &sub = instance.sub_objects[prop.sub_obj];
						sub.set_value<Value>(instance, stack, stack_pos + 1, obj, ctx.object, name, value);
						return;
					}

					auto iter = instance.prop_lookup.find(name);
					if (iter == std::end(instance.prop_lookup)) {
						return;
					}

					member_context tmp{};
					member_context *mem_ctx{ nullptr };
					id_type prop_idx = iter->second;
					if (stack.empty()) {
						mem_ctx = &tmp;
					}
					else if (stack_pos == stack.size()) {
						auto &ctx = stack[stack_pos - 1];
						mem_ctx = &ctx.member_context[prop_idx];
					}

					obj_member &prop = instance.props[prop_idx];
					obj_primitive &primitive = instance.primitives[prop.primitive];
					primitive.set_value(instance, *mem_ctx, obj, prop_idx, value);
				}
			}

			template<typename Type>
			void object<Type>::push_back(context_stack &stack, int stack_pos, std::string_view name) {
				if (stack_pos == stack.size()) {
					auto iter = instance.prop_lookup.find(name);
					if (iter == std::end(instance.prop_lookup)) {
						return;
					}

					context &ctx = stack.emplace_back();
					ctx.object = iter->second;
					ctx.type = instance.props[iter->second].type;
					ctx.name = instance.props[iter->second].name;
					ctx.member_context.resize(instance.props.size());
				}
				else if (stack_pos < stack.size()) {
					auto &ctx = stack[stack_pos];
					auto &prop = instance.props[ctx.object];
					obj_forward &sub = instance.sub_objects[prop.sub_obj];
					sub.push_back(stack, stack_pos + 1, name);
				}
			}

			template<typename Type>
			void object<Type>::push_back(context_stack &stack, std::string_view name) {
				push_back(stack, 0, name);
			}

			template<typename Type>
			context object<Type>::pop_back(context_stack &stack) {
				// TODO: Validation should occur here?
				return stack.pop_back();
			}

			template<typename Type>
			template<typename Prop, typename Value>
			void object<Type>::set_value(member_context &member_ctx, obj_member &member, Type &obj, const Value &value) {
				member_ctx.is_in_range = true;
				if (member.range != schema::null) {
					if constexpr (std::is_enum_v<Prop>) {
						Prop prop = enum_to_string<Prop>::to_enum(value);
						member_ctx.is_in_range = detail::check_constraint(instance.ranges<Prop>[member.range], member, prop);
					}
					else {
						member_ctx.is_in_range = detail::check_constraint(instance.ranges<Prop>[member.range], member, value);
					}
				}

				// TODO: Reporting errors could be done here or a context object could be passed to each parse related function to validate
				// And report as a whole?
				// if (!in_range) {
				// 	// report errors
				// }

				auto ptr = instance.prop_ptrs<Prop>[member.ptr];
				if constexpr (std::is_enum_v<Prop>) {
					obj.*ptr = enum_to_string<Prop>::to_enum(value);
				}
				else if (member_type_trait_v<Prop> != MT_object) {
					obj.*ptr = static_cast<Prop>(value);
				}
				member_ctx.dirty = true;
			}
		}    // namespace detail
	}    // namespace schema
}    // namespace tc
