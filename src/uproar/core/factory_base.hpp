#ifndef UPROAR_CORE_FACTORY_BASE_HPP
#define UPROAR_CORE_FACTORY_BASE_HPP

#include "../config/config.hpp"
#include "attributes.hpp"
#include "scoped_ptr.hpp"
#include "unordered_map"

namespace tc {
	template<typename Key, typename Base, typename Factory>
	class UPROAR_API factory_base {
		using spawn_t = scope_ptr<Base> (*)();

		template<typename Type>
		static scope_ptr<Base> make_type() {
			auto t = make_scoped<Type>();
			scope_ptr<Base> base;
			base = std::move(t);
			return base;
		};

	public:
		template<typename Type>
		bool record(const Key &key) {
			if (registered_types.find(key) != std::end(registered_types)) {
				return false;
			}

			auto x = registered_types.emplace(key, []() { return make_type<Type>(); });
			static_cast<Factory *>(this)->record_impl<Type>(key);
			return x.second;
		}

		scope_ptr<Base> spawn(const Key &key) const {
			auto s = registered_types.find(key);
			if (s != std::end(registered_types)) {
				auto spawner = s->second;
				return spawner();
			}

			return { nullptr };
		}

	private:
		std::unordered_map<Key, spawn_t> registered_types;
	};
}    // namespace tc

#endif    // UPROAR_CORE_FACTORY_BASE_HPP
