#include "schema_options.hpp"

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
