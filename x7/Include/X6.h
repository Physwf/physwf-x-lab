
#ifndef X6_API_H
#define X6_API_H

#ifdef X6_STATIC_DEFINE
#  define X6_API
#  define X6_NO_EXPORT
#else
#  ifndef X6_API
#    ifdef libx7_EXPORTS
        /* We are building this library */
#      define X6_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define X6_API __declspec(dllimport)
#    endif
#  endif

#  ifndef X6_NO_EXPORT
#    define X6_NO_EXPORT 
#  endif
#endif

#ifndef X6_DEPRECATED
#  define X6_DEPRECATED __declspec(deprecated)
#endif

#ifndef X6_DEPRECATED_EXPORT
#  define X6_DEPRECATED_EXPORT X6_API X6_DEPRECATED
#endif

#ifndef X6_DEPRECATED_NO_EXPORT
#  define X6_DEPRECATED_NO_EXPORT X6_NO_EXPORT X6_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef X6_NO_DEPRECATED
#    define X6_NO_DEPRECATED
#  endif
#endif

#endif /* X6_API_H */
