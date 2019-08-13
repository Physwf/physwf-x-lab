
#ifndef X7_API_H
#define X7_API_H

#ifdef X7_STATIC_DEFINE
#  define X7_API
#  define X7_NO_EXPORT
#else
#  ifndef X7_API
#    ifdef libx7_EXPORTS
        /* We are building this library */
#      define X7_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define X7_API __declspec(dllimport)
#    endif
#  endif

#  ifndef X7_NO_EXPORT
#    define X7_NO_EXPORT 
#  endif
#endif

#ifndef X7_DEPRECATED
#  define X7_DEPRECATED __declspec(deprecated)
#endif

#ifndef X7_DEPRECATED_EXPORT
#  define X7_DEPRECATED_EXPORT X7_API X7_DEPRECATED
#endif

#ifndef X7_DEPRECATED_NO_EXPORT
#  define X7_DEPRECATED_NO_EXPORT X7_NO_EXPORT X7_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef X7_NO_DEPRECATED
#    define X7_NO_DEPRECATED
#  endif
#endif

#endif /* X7_API_H */
