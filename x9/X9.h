
#ifndef X9_API_H
#define X9_API_H

#ifdef X9_STATIC_DEFINE
#  define X9_API
#  define X9_NO_EXPORT
#else
#  ifndef X9_API
#    ifdef libx9_EXPORTS
        /* We are building this library */
#      define X9_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define X9_API __declspec(dllimport)
#    endif
#  endif

#  ifndef X9_NO_EXPORT
#    define X9_NO_EXPORT 
#  endif
#endif

#ifndef X9_DEPRECATED
#  define X9_DEPRECATED __declspec(deprecated)
#endif

#ifndef X9_DEPRECATED_EXPORT
#  define X9_DEPRECATED_EXPORT X9_API X9_DEPRECATED
#endif

#ifndef X9_DEPRECATED_NO_EXPORT
#  define X9_DEPRECATED_NO_EXPORT X9_NO_EXPORT X9_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef X9_NO_DEPRECATED
#    define X9_NO_DEPRECATED
#  endif
#endif

#endif /* X9_API_H */
