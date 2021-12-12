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

			template<typename Type>
			static object<Type> create_object_instance(instance_type_helper<Type>) {
				object<Type> instance{};

				member<Type, object<Type>> self(instance, "this", member_object_type_v<Type>, n<Type>());
				instance.props.emplace_back(std::move(self));

				return instance;
			}

			template<template<class...> typename Container, typename Type, typename... Rest>
			static object<Container<Type, Rest...>> create_array_instance(instance_type_helper<Container<Type, Rest...>>) {
				using Outer = Container<Type, Rest...>;
				object<Outer> instance{};

				member<Outer, object<Outer>> helper(instance, "child_type", member_object_type_v<Type>, n<Type>());
				instance.props.emplace_back(std::move(helper));

				return instance;
			}

			template<template<class...> typename Container, typename Key, typename Type, typename... Rest>
			static object<Container<Key, Type, Rest...>> create_map_instance(instance_type_helper<Container<Key, Type, Rest...>>) {
				using Outer = Container<Key, Type, Rest...>;
				object<Outer> instance{};

				member<Outer, object<Outer>> helper(instance, "child_type", member_object_type_v<Type>, n<Type>());
				instance.props.emplace_back(std::move(helper));

				return instance;
			}

			template<typename Type>
			static object<Type> create_instance(const instance_type_helper<Type> &) {
				if constexpr (member_type_trait_v<Type> < MT_object) {
					static_assert(member_type_trait_v<Type> > MT_object, "Cannot create an instance from a primitive type.");
				}
				else if constexpr (member_type_trait_v<Type> == MT_object) {
					return create_object_instance(instance_type_helper<Type>{});
				}
				else if constexpr (member_type_trait_v<Type> == MT_array) {
					return create_array_instance(instance_type_helper<Type>{});
				}
				else if constexpr (member_type_trait_v<Type> == MT_map) {
					return create_map_instance(instance_type_helper<Type>{});
				}
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
			void object<Type>::set_value(context_stack &stack, Type &outer, const Value &value) const {
				if constexpr (std::is_enum_v<Value>) {
					std::string converted{ enum_to_string<Value>::to_string(value) };
					set_value<std::string>(stack, stack_pos, outer, converted);
				}
				else {
					if constexpr (member_type_trait_v<Type> >= MT_object) {
						const object<Type> &obj = *this;
						obj.self.set_value(obj, stack, 0, outer, value);
					}
				}
			}

			template<typename Type>
			template<typename Value>
			void object<Type>::visit(schema::visitor &v, Value &val) const {
				const object<Type> &obj = *this;
				obj.self.visit(obj, v, val, "");

				// if constexpr (member_type_trait_v<Type> == MT_object) {
				// 	object<Type> &obj = instance;
				// 	for (size_t i = 1; i < obj.props; ++i) {
				// 		obj_member &prop = obj.props[0];
				// 		if (prop.child != null) {
				// 			obj_forward &sub = obj.children[prop.child];
				// 			sub.visit(obj, v, val, "");
				// 		}
				// 		else if (prop.primitive != null) {
				// 			obj_primitive &sub = obj.primitives[prop.primitive];
				// 			sub.visit(obj, v, val, i);
				// 		}
				// 	}
				// }
				// else if constexpr (member_type_trait_v<Type> == MT_array || member_type_trait_v<Type> == MT_map) {
				// 	object<Type> &obj = instance;
				// 	bool contains_obj = obj.props.size() == 2;
				// 	if (contains_obj) {
				// 		obj_member &prop = obj.props[0];
				// 		if (prop.child != null) {
				// 			obj_forward &sub = obj.children[prop.child];
				// 			sub.visit(obj, v, val, "");
				// 		}
				// 	}
				// 	else {
				// 		obj_member &prop = obj.props[0];
				// 		if (prop.primitive != null) {
				// 			obj_primitive &sub = obj.primitives[prop.primitive];
				// 			sub.visit(obj, v, val, 0);
				// 		}
				// 	}
				// }
			}

			template<typename Type>
			void object<Type>::push_back(context_stack &stack, int stack_pos, std::string_view name) const {
				const object<Type> &obj = *this;
				obj.self.push_back(stack, stack_pos, name);
			}

			template<typename Type>
			void object<Type>::push_back(context_stack &stack, std::string_view name) const {
				push_back(stack, 0, name);
			}

			template<typename Type>
			void object<Type>::push_back(context_stack &stack, int stack_pos, int32 array_idx) const {
				const object<Type> &obj = *this;
				obj.self.push_back(stack, stack_pos, array_idx);
			}

			template<typename Type>
			void object<Type>::push_back(context_stack &stack, int32 array_idx) const {
				push_back(stack, 0, array_idx);
			}

			template<typename Type>
			context object<Type>::pop_back(context_stack &stack) const {
				// TODO: Validation should occur here?
				return stack.pop_back();
			}
		}    // namespace detail
	}    // namespace schema
}    // namespace tc

namespace tc {
	namespace schema {
		template<typename Type>
		void visit(visitor &visitor, Type &obj) {
			if constexpr (member_type_trait_v<Type> < MT_object) {
				visitor.set("", obj);
			}
			else {
				detail::object<Type>::instance.visit(visitor, obj);
			}
		}
	}    // namespace schema
}    // namespace tc