#include "definition.hpp"

namespace tc {
	namespace schema {
		namespace detail {

			template<typename Outer, typename ObjType>
			template<typename Y, typename... Z, template<class> typename... Constraint>
			member<Outer, ObjType>::member(ObjType &obj, std::string_view name, member_ptr<Outer, Y> mem, std::string_view desc, Constraint<Z> &&...details)
				: name{ name }
				, desc{ desc }
				, type{ member_type_trait_v<Y> } {
				ptr = id_type(obj.prop_ptrs<Y>.size());
				obj.prop_ptrs<Y>.emplace_back(mem);

				if constexpr (member_type_trait_v<Y> == MT_object) {
					sub_obj = id_type(obj.sub_objects.size());
					obj.sub_objects.emplace_back(obj, mem);
				}
				else {
					primitive = id_type(obj.primitives.size());
					obj.primitives.emplace_back(obj, mem);
				}

				(add_constraint(obj, std::forward<Constraint<Z>>(details)), ...);
			}

			template<typename Outer, typename ObjType>
			template<typename Y, template<typename> typename Constraint>
			void member<Outer, ObjType>::add_constraint(ObjType &obj, Constraint<Y> &detail) {
				static_assert(is_member_constraint_v<Constraint<Y>>, "must be a known member constraint type");
				detail::add_constraint(detail, obj, *this);
			}
		}    // namespace detail
	}    // namespace schema
}    // namespace tc

namespace tc {
	namespace schema {
		namespace detail {
			template<typename Outer, typename ObjType>
			template<typename Y>
			member_object<Outer, ObjType>::member_object(ObjType &obj, member_ptr<Outer, Y> mem) {
				if constexpr (std::is_same_v<Y, bool>) {
					std::get<Setter<bool>>(setters) = [](ObjType &obj, context_stack &stack, int stack_pos, Outer &outer, id_type member_idx, std::string_view name, const bool &value) {
						set_impl<Y, bool>(stack, stack_pos, outer, obj.prop_ptrs<Y>[member_idx], name, value);
					};
				}
				else {
					std::get<Setter<bool>>(setters) = [](ObjType &, context_stack &, int, Outer &, id_type, std::string_view, const bool &) {};
				}

				if constexpr (std::is_same_v<Y, char>) {
					std::get<Setter<char>>(setters) = [](ObjType &obj, context_stack &stack, int stack_pos, Outer &outer, id_type member_idx, std::string_view name, const char &value) {
						set_impl<Y, char>(stack, stack_pos, outer, obj.prop_ptrs<Y>[member_idx], name, value);
					};
				}
				else {
					std::get<Setter<char>>(setters) = [](ObjType &, context_stack &, int, Outer &, id_type, std::string_view, const char &) {};
				}

				if constexpr (std::is_integral_v<Y>) {
					std::get<Setter<int>>(setters) = [](ObjType &obj, context_stack &stack, int stack_pos, Outer &outer, id_type member_idx, std::string_view name, const int &value) {
						set_impl<Y, int>(stack, stack_pos, outer, obj.prop_ptrs<Y>[member_idx], name, value);
					};
				}
				else {
					std::get<Setter<int>>(setters) = [](ObjType &, context_stack &, int, Outer &, id_type, std::string_view, const int &) {};
				}

				std::get<Setter<std::string>>(setters) = [](ObjType &obj, context_stack &stack, int stack_pos, Outer &outer, id_type member_idx, std::string_view name, const std::string &value) {
					set_impl<Y, std::string>(stack, stack_pos, outer, obj.prop_ptrs<Y>[member_idx], name, value);
				};

				push_back = [](context_stack &stack, int stack_pos, std::string_view name) {
					push_back_impl<Y>(stack, stack_pos, name);
				};
			}

			template<typename Outer, typename ObjType>
			template<typename Value>
			void member_object<Outer, ObjType>::set_value(ObjType &obj, context_stack &stack, int stack_pos, Outer &outer, id_type member_idx, std::string_view name, const Value &value) {
				Setter<Value> &setter = std::get<Setter<Value>>(setters);
				setter(obj, stack, stack_pos, outer, member_idx, name, value);
			}

			template<typename Outer, typename ObjType>
			template<typename Prop, typename Value>
			void member_object<Outer, ObjType>::set_impl(context_stack &stack, int stack_pos, Outer &outer, member_ptr<Outer, Prop> ptr, std::string_view name, const Value &value) {
				auto &objY = object<Prop>::instance;
				objY.set_value<Prop, Value>(stack, stack_pos, outer.*ptr, name, value);
			}

			template<typename Outer, typename ObjType>
			template<typename Y>
			void member_object<Outer, ObjType>::push_back_impl(context_stack &stack, int stack_pos, std::string_view name) {
				object<Y>::instance.push_back(stack, stack_pos, name);
			}
		}    // namespace detail
	}    // namespace schema
}    // namespace tc


namespace tc {
	namespace schema {
		namespace detail {
			template<typename Outer, typename ObjType>
			template<typename Prop>
			member_primitive<Outer, ObjType>::member_primitive(ObjType &obj, member_ptr<Outer, Prop> mem) {
				if constexpr (member_type_trait_v<Prop> == MT_bool) {
					setter = [](ObjType &obj, member_context& ctx, Outer &outer, id_type member_idx, const bool &value) -> void {
						set_impl<Prop, bool>(obj, ctx, outer, obj.props[member_idx], value);
					};
				}
				else if constexpr (member_type_trait_v<Prop> == MT_char) {
					setter = [](ObjType &obj, member_context& ctx, Outer &outer, id_type member_idx, const char &value) -> void {
						set_impl<Prop, char>(obj, ctx, outer, obj.props[member_idx], value);
					};
				}
				else if constexpr (member_type_trait_v<Prop> == MT_int32) {
					setter = [](ObjType &obj, member_context& ctx, Outer &outer, id_type member_idx, const int &value) -> void {
						set_impl<Prop, int>(obj, ctx, outer, obj.props[member_idx], value);
					};
				}
				else if constexpr (member_type_trait_v<Prop> == MT_string || member_type_trait_v<Prop> == MT_enum) {
					setter = [](ObjType &obj, member_context& ctx, Outer &outer, id_type member_idx, const std::string &value) -> void {
						set_impl<Prop, std::string>(obj, ctx, outer, obj.props[member_idx], value);
					};
				}
			}

			template<typename Outer, typename ObjType>
			template<typename Value>
			void member_primitive<Outer, ObjType>::set_value(ObjType &obj, member_context& ctx, Outer &outer, id_type member_idx, const Value &value) {
				Setter<Value> &set = std::get<Setter<Value>>(setter);
				set(obj, ctx, outer, member_idx, value);
			}

			template<typename Outer, typename ObjType>
			template<typename Prop, typename Value>
			void member_primitive<Outer, ObjType>::set_impl(ObjType &obj, member_context& ctx, Outer &outer, member<Outer, ObjType>& mem, const Value &value) {
				obj.set_value<Prop, Value>(ctx, mem, outer, value);
			}
		}    // namespace detail
	}    // namespace schema
}    // namespace tc

namespace tc {
	namespace schema {
		template<typename Type>
		template<typename Y, typename... Z, template<class> typename... Constraint>
		static void object<Type>::add_member(const char *name, detail::member_ptr<Type, Y> mem_ptr, const char *desc, Constraint<Z> &&...constraints) {
			std::string_view n{ name };
			auto iter = instance.prop_lookup.find(n);
			if (iter != std::end(instance.prop_lookup)) {
				// Already been defined
				return;
			}

			instance.prop_lookup[n] = detail::id_type(instance.props.size());
			obj_member member(instance, n, std::move(mem_ptr), desc, std::forward<Constraint<Z>>(constraints)...);
			instance.props.emplace_back(std::move(member));
		}

		template<typename X>
		object<X> object<X>::instance{};

		template<typename Type>
		void object<Type>::set_bool(context_stack &stack, Type &obj, std::string_view name, bool value) {
			set_value<bool>(stack, 0, obj, name, value);
		}

		template<typename Type>
		void object<Type>::set_char(context_stack &stack, Type &obj, std::string_view name, char value) {
			set_value<char>(stack, 0, obj, name, value);
		}

		template<typename Type>
		void object<Type>::set_int(context_stack &stack, Type &obj, std::string_view name, int value) {
			set_value<int>(stack, 0, obj, name, value);
		}

		template<typename Type>
		template<typename Enum>
		void object<Type>::set_enum(context_stack& stack, Type &obj, std::string_view name, Enum value) {
			set_value<Enum>(stack, 0, obj, name, value);
		}

		template<typename Type>
		void object<Type>::set_string(context_stack &stack, Type &obj, std::string_view name, const std::string &value) {
			set_value<std::string>(stack, 0, obj, name, value);
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
				detail::id_type prop_idx = iter->second;
				if (stack.empty()) {
					 mem_ctx = &tmp;
				}
				else if (stack_pos == stack.size()) {
					auto &ctx = stack[stack_pos - 1];
					mem_ctx = &ctx.member_context[prop_idx];
				}

				obj_member& prop = instance.props[prop_idx];
				obj_primitive& primitive = instance.primitives[prop.primitive];
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
			if (member.range != detail::null) {
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
			else if (detail::member_type_trait_v<Prop> != detail::MT_object) {
				obj.*ptr = static_cast<Prop>(value);
			}
			member_ctx.dirty = true;
		}
	}    // namespace schema
}    // namespace tc

namespace tc {
	namespace schema {
		namespace detail {
			template<typename X, typename Obj, typename Member>
			void add_constraint(schema::default_value<X> &detail, Obj &obj, Member &member) {
				member.default_value = id_type(obj.default_values<X>.size());
				obj.default_values<X>.emplace_back(detail);
			}

			template<typename X, typename Obj, typename Member>
			void add_constraint(schema::range<X> &detail, Obj &obj, Member &member) {
				member.range = id_type(obj.ranges<X>.size());
				obj.ranges<X>.emplace_back(detail);
			}

			template<typename X, typename Member, typename Y>
			bool check_constraint(const schema::range<X> &constraint, const Member &member, const Y &value) {
				return (constraint.min <= value && value <= constraint.max);
			}

			template<typename X, typename Obj, typename Member>
			void add_constraint(schema::required<X> &detail, Obj &obj, Member &member) {
				member.required = true;
			}

			template<typename X, typename Obj, typename Member>
			void add_constraint(schema::not_null<X> &detail, Obj &obj, Member &member) {
				member.not_null = true;
			}
		}    // namespace detail
	}    // namespace schema
}    // namespace tc

namespace tc {
	namespace schema {
		context &context_stack::emplace_back() {
			return stack.emplace_back();
		}

		context context_stack::pop_back() {
			auto pop = stack.back();
			stack.pop_back();
			return pop;
		}

		context &context_stack::operator[](int i) {
			return stack[i];
		}

		const context &context_stack::operator[](int i) const {
			return stack[i];
		}

		int context_stack::size() const {
			return int(stack.size());
		}

		bool context_stack::empty() const {
			return stack.empty();
		}
	}    // namespace schema
}    // namespace tc
