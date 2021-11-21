#include "schema_member_object.hpp"

// Object
namespace tc {
	namespace schema {
		namespace detail {
			template<typename Outer, typename ObjType>
			template<typename Y>
			member_object<Outer, ObjType>::member_object(ObjType &obj, member_ptr<Outer, Y> mem) {
#define SET_TYPE(TYPE) \
	if constexpr (std::is_same_v<Y, TYPE>) { \
		std::get<Setter<TYPE>>(setters) = [](ObjType &obj, context_stack &stack, int stack_pos, Outer &outer, id_type member_idx, std::string_view name, const TYPE &value) { \
			set_impl<Y, TYPE>(stack, stack_pos, outer, obj.prop_ptrs<Y>[member_idx], name, value); \
		}; \
	} \
	else { \
		std::get<Setter<TYPE>>(setters) = [](ObjType &, context_stack &, int, Outer &, id_type, std::string_view, const TYPE &) {}; \
	}

				SET_TYPE(bool);

				SET_TYPE(char);
				SET_TYPE(int16);
				SET_TYPE(int32);
				SET_TYPE(int64);

				SET_TYPE(uint8);
				SET_TYPE(uint16);
				SET_TYPE(uint32);
				SET_TYPE(uint64);

				SET_TYPE(float);
				SET_TYPE(double);

#undef SET_TYPE

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
