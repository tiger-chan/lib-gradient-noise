#ifndef UPROAR_CORE_ATTRIBUTE_HPP
#define UPROAR_CORE_ATTRIBUTE_HPP

#ifndef UPROAR_EXPORT
#	if defined _WIN32 || defined __CYGWIN__ || defined _MSC_VER
#		define UPROAR_EXPORT __declspec(dllexport)
#		define UPROAR_IMPORT __declspec(dllimport)
#		define UPROAR_HIDDEN
#	elif defined __GNUC__ && __GNUC__ >= 4
#		define UPROAR_EXPORT __attribute__((visibility("default")))
#		define UPROAR_IMPORT __attribute__((visibility("default")))
#		define UPROAR_HIDDEN __attribute__((visibility("hidden")))
#	else /* Unsupported compiler */
#		define UPROAR_EXPORT
#		define UPROAR_IMPORT
#		define UPROAR_HIDDEN
#	endif
#endif

#ifndef UPROAR_API
#	if defined UPROAR_API_EXPORT
#		define UPROAR_API UPROAR_EXPORT
#	elif defined UPROAR_API_IMPORT
#		define UPROAR_API UPROAR_IMPORT
#	else /* No API */
#		define UPROAR_API
#	endif
#endif

#endif
