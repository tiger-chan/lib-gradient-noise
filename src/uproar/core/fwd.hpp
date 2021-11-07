#ifndef UPROAR_CORE_FWD_HPP
#define UPROAR_CORE_FWD_HPP

#include "../config/config.hpp"

namespace tc {
	template<typename Blender>
	class perlin_noise;
	class random;

	template<typename Data>
	class scope_ptr;

	template<typename Key, typename Base, typename Factory>
	class factory_base;

	template<typename Type>
	class singleton_base;
}    // namespace tc

#endif    // UPROAR_CORE_FWD_HPP
