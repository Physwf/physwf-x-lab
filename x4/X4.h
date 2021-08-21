
#ifndef X4_API_H
#define X4_API_H

#ifdef X4_STATIC_DEFINE
#  define X4_API
#  define X4_NO_EXPORT
#else
#  ifndef X4_API
#    ifdef x4_EXPORTS
        /* We are building this library */
#      define X4_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define X4_API __declspec(dllimport)
#    endif
#  endif

#  ifndef X4_NO_EXPORT
#    define X4_NO_EXPORT 
#  endif
#endif

#ifndef X4_DEPRECATED
#  define X4_DEPRECATED __declspec(deprecated)
#endif

#ifndef X4_DEPRECATED_EXPORT
#  define X4_DEPRECATED_EXPORT X4_API X4_DEPRECATED
#endif

#ifndef X4_DEPRECATED_NO_EXPORT
#  define X4_DEPRECATED_NO_EXPORT X4_NO_EXPORT X4_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef X4_NO_DEPRECATED
#    define X4_NO_DEPRECATED
#  endif
#endif

#endif /* X4_API_H */
