#include "schema_definition.hpp"
#include "schema_object.hpp"

namespace tc {
	namespace schema {
		template<typename Type>
		template<typename Y, typename... Z, template<class> typename... Constraint>
		void definition<Type>::add_member(const char *name, member_ptr<Type, Y> mem_ptr, const char *desc, Constraint<Z> &&...constraints) {
			auto& obj = detail::object<Type>::instance;
			obj.add_member(name, mem_ptr, desc, std::forward<Constraint<Z>>(constraints)...);
		}
	}    // namespace schema
}    // namespace tc
