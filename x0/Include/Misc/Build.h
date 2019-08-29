#pragma once

#ifndef X_BUILD_DEBUG
	#define X_BUILD_DEBUG				0
#endif
#ifndef X_BUILD_DEVELOPMENT
	#define X_BUILD_DEVELOPMENT		0
#endif
#ifndef X_BUILD_TEST
	#define X_BUILD_TEST				0
#endif
#ifndef X_BUILD_SHIPPING
	#define X_BUILD_SHIPPING			0
#endif


/** If not specified, disable logging in shipping */
#ifndef USE_LOGGING_IN_SHIPPING
	#define USE_LOGGING_IN_SHIPPING 0
#endif

#ifndef USE_CHECKS_IN_SHIPPING
	#define USE_CHECKS_IN_SHIPPING 0
#endif

#ifndef ALLOW_CONSOLE_IN_SHIPPING
	#define ALLOW_CONSOLE_IN_SHIPPING 0
#endif

#if X_BUILD_DEBUG
	#define DO_GUARD_SLOW									1
	#define DO_CHECK										1
	#define ALLOW_DEBUG_FILES								1
	#define ALLOW_CONSOLE									1
	#define NO_LOGGING										0
#elif X_BUILD_DEVELOPMENT
	#define DO_GUARD_SLOW									0
	#define DO_CHECK										1
	#define ALLOW_DEBUG_FILES								1
	#define ALLOW_CONSOLE									1
	#define NO_LOGGING										0
#elif X_BUILD_TEST
	#define DO_GUARD_SLOW									0
	#define DO_CHECK										USE_CHECKS_IN_SHIPPING
	#define ALLOW_DEBUG_FILES								1
	#define ALLOW_CONSOLE									1
	#define NO_LOGGING										!USE_LOGGING_IN_SHIPPING
#elif X_BUILD_SHIPPING
	#define DO_GUARD_SLOW								0
	#define DO_CHECK									USE_CHECKS_IN_SHIPPING
	#define ALLOW_DEBUG_FILES							0
	#define ALLOW_CONSOLE								ALLOW_CONSOLE_IN_SHIPPING
	#define NO_LOGGING									!USE_LOGGING_IN_SHIPPING
#else
#error Exactly one of [X_BUILD_DEBUG X_BUILD_DEVELOPMENT X_BUILD_TEST X_BUILD_SHIPPING] should be defined to be 1
#endif