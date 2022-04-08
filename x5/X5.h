
#ifndef X5_API_H
#define X5_API_H

#ifdef X5_STATIC_DEFINE
#  define X5_API
#  define X5_NO_EXPORT
#else
#  ifndef X5_API
#    ifdef x5_EXPORTS
        /* We are building this library */
#      define X5_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define X5_API __declspec(dllimport)
#    endif
#  endif

#  ifndef X5_NO_EXPORT
#    define X5_NO_EXPORT 
#  endif
#endif

#ifndef X5_DEPRECATED
#  define X5_DEPRECATED __declspec(deprecated)
#endif

#ifndef X5_DEPRECATED_EXPORT
#  define X5_DEPRECATED_EXPORT X5_API X5_DEPRECATED
#endif

#ifndef X5_DEPRECATED_NO_EXPORT
#  define X5_DEPRECATED_NO_EXPORT X5_NO_EXPORT X5_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef X5_NO_DEPRECATED
#    define X5_NO_DEPRECATED
#  endif
#endif

#endif /* X5_API_H */
