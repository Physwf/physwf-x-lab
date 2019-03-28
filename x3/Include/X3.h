
#ifndef X3_API_H
#define X3_API_H

#ifdef X3_STATIC_DEFINE
#  define X3_API
#  define X3_NO_EXPORT
#else
#  ifndef X3_API
#    ifdef libx3_EXPORTS
        /* We are building this library */
#      define X3_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define X3_API __declspec(dllimport)
#    endif
#  endif

#  ifndef X3_NO_EXPORT
#    define X3_NO_EXPORT 
#  endif
#endif

#ifndef X3_DEPRECATED
#  define X3_DEPRECATED __declspec(deprecated)
#endif

#ifndef X3_DEPRECATED_EXPORT
#  define X3_DEPRECATED_EXPORT X3_API X3_DEPRECATED
#endif

#ifndef X3_DEPRECATED_NO_EXPORT
#  define X3_DEPRECATED_NO_EXPORT X3_NO_EXPORT X3_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef X3_NO_DEPRECATED
#    define X3_NO_DEPRECATED
#  endif
#endif

#endif /* X3_API_H */
