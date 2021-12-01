#include "schema_object.hpp"

namespace tc {
	namespace schema {
		namespace detail {
			constexpr std::string_view pretty_name(std::string_view name) {
				for (std::size_t i = name.size(); i > 0; --i) {
					auto v = name[i - 1];
					if (!((v >= '0' && v <= '9') || (v >= 'a' && v <= 'z') || (v >= 'A' && v <= 'Z') || (v == '_') || (v == ':'))) {
						name.remove_prefix(i);
						break;
					}
				}

				return name;
			}

			template<typename Class>
			constexpr auto n() {
#if defined(__clang__) || defined(__GNUC__)
				auto view = std::string_view{ __PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) };
				view.remove_suffix(2);
#elif defined(_MSC_VER)
				auto view = std::string_view{ __FUNCSIG__, sizeof(__FUNCSIG__) };
				view.remove_suffix(8);
#else
				auto view = std::string_view{};
#endif
				return pretty_name(view);
			}

			template<typename T>
			struct instance_type_helper {
				using type = T;
			};

			template<typename Type, typename = std::enable_if_t<member_type_trait_v<Type> == MT_object>>
			static object<Type> create_instance(instance_type_helper<Type>) {
				object<Type> instance{};
				member<Type, object<Type>> self(instance, "this", member_object_type_v<Type>, n<Type>());
				instance.props.emplace_back(std::move(self));
				return instance;
			}

			template<template<class...> typename Container, typename Type, typename... Rest, typename = std::enable_if_t<member_type_trait_v<Container<Type, Rest...>> == MT_array>>
			static object<Container<Type, Rest...>> create_instance(instance_type_helper<Container<Type, Rest...>>) {
				using Outer = Container<Type, Rest...>;
				object<Outer> instance{};
				member<Outer, object<Outer>> self(instance, "this", member_object_type_v<Container<Type>>, n<Outer>());
				instance.props.emplace_back(std::move(self));

				if constexpr (member_type_trait_v<Type> >= MT_object) {
					member<Outer, object<Outer>> helper(instance, "child_type", member_object_type_v<Type>, "Member helper for the child type");
					instance.props.emplace_back(std::move(helper));
				}

				return instance;
			}

			template<template<class...> typename Container, typename Key, typename Type, typename... Rest, typename = std::enable_if_t<member_type_trait_v<Container<Key, Type, Rest...>> == MT_map>>
			static object<Container<Key, Type, Rest...>> create_instance(instance_type_helper<Container<Key, Type, Rest...>>) {
				using Outer = Container<Key, Type, Rest...>;
				object<Outer> instance{};

				member<Outer, object<Outer>> self(instance, "this", member_object_type_v<Container<Key, Type>>, n<Outer>());
				instance.props.emplace_back(std::move(self));

				if constexpr (member_type_trait_v<Type> >= MT_object) {
					member<Outer, object<Outer>> helper(instance, "child_type", member_object_type_v<Type>, "Member helper for the child type");
					instance.props.emplace_back(std::move(helper));
				}

				return instance;
			}

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
			object<X> object<X>::instance{ create_instance(instance_type_helper<X>{}) };

			template<typename Type>
			template<typename Value>
			void object<Type>::set_value(context_stack &stack, Type &obj, const Value &value) {
				if constexpr (std::is_enum_v<Value>) {
					std::string converted{ enum_to_string<Value>::to_string(value) };
					set_value<std::string>(stack, stack_pos, obj, converted);
				}
				else {
					context &ctx{ stack[0] };
					if constexpr (member_type_trait_v<Type> >= MT_object) {
						if (stack.size() == 1) {
							// this is a as deep as we need to go.
							obj_member &prop = instance.props[ctx.object];
							obj_primitive &prim = instance.primitives[prop.primitive];
							prim.set_value(*this, ctx, obj, ctx.object, value);
						}
						else {
							obj_member &prop = instance.props[ctx.object];
							obj_forward &sub = instance.children[prop.child];
							sub.set_value(*this, stack, 0, obj, value);
						}
					}
				}
			}

			template<typename Type>
			void object<Type>::push_back(context_stack &stack, int stack_pos, std::string_view name) {
				if (stack_pos == stack.size()) {
					if constexpr (member_type_trait_v<Type> == MT_object) {
						auto iter = instance.prop_lookup.find(name);
						if (iter == std::end(instance.prop_lookup)) {
							return;
						}

						context &ctx = stack.emplace_back();
						ctx.object = iter->second;
						ctx.type = instance.props[iter->second].type;
						ctx.id.name = instance.props[iter->second].name;
						ctx.member_context.resize(instance.props.size());
					}
					else if constexpr (member_type_trait_v<Type> == MT_map) {
						auto &prop = instance.props[0];
						obj_forward &sub = instance.children[prop.child];
						sub.push_back(stack, stack_pos, name);
					}
				}
				else if (stack_pos < stack.size()) {
					auto &ctx = stack[stack_pos];
					auto &prop = instance.props[ctx.object];
					obj_forward &sub = instance.children[prop.child];
					sub.push_back(stack, stack_pos, name);
				}
			}

			template<typename Type>
			void object<Type>::push_back(context_stack &stack, std::string_view name) {
				push_back(stack, 0, name);
			}

			template<typename Type>
			void object<Type>::push_back(context_stack &stack, int stack_pos, int32 array_idx) {
				if (stack_pos == stack.size()) {
					if constexpr (member_type_trait_v<Type> == MT_array) {
						auto &prop = instance.props[0];
						obj_forward &sub = instance.children[prop.child];
						sub.push_back(stack, stack_pos, array_idx);
					}
				}
				else if (stack_pos < stack.size()) {
					auto &ctx = stack[stack_pos];
					auto &prop = instance.props[ctx.object];
					obj_forward &sub = instance.children[prop.child];
					sub.push_back(stack, stack_pos, array_idx);
				}
			}

			template<typename Type>
			void object<Type>::push_back(context_stack &stack, int32 array_idx) {
				push_back(stack, 0, array_idx);
			}

			template<typename Type>
			context object<Type>::pop_back(context_stack &stack) {
				// TODO: Validation should occur here?
				return stack.pop_back();
			}
		}    // namespace detail
	}    // namespace schema
}    // namespace tc
