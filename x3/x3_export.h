
#ifndef X3_API_H
#define X3_API_H

#ifdef X3_STATIC_DEFINE
#  define X3_API
#  define X3_NO_EXPORT
#else
#  ifndef X3_API
#    ifdef libx0_EXPORTS
        /* We are building this library */
#      define X3_API 
#    else
        /* We are using this library */
#      define X3_API 
#    endif
#  endif

#  ifndef X3_NO_EXPORT
#    define X3_NO_EXPORT 
#  endif
#endif

#ifndef X3_DEPRECATED_API
#  define X3_DEPRECATED_API __declspec(deprecated)
#endif

#ifndef X3_DEPRECATED_API_EXPORT
#  define X3_DEPRECATED_API_EXPORT X3_API X3_DEPRECATED_API
#endif

#ifndef X3_DEPRECATED_API_NO_EXPORT
#  define X3_DEPRECATED_API_NO_EXPORT X3_NO_EXPORT X3_DEPRECATED_API
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef X3_DEPRECATED_NO_API
#    define X3_DEPRECATED_NO_API
#  endif
#endif

#endif /* X3_API_H */
