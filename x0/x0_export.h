
#ifndef X0_EXPORT_H
#define X0_EXPORT_H

#ifdef X0_STATIC_DEFINE
#  define X0_EXPORT
#  define X0_NO_EXPORT
#else
#  ifndef X0_EXPORT
#    ifdef libx0_EXPORTS
        /* We are building this library */
#      define X0_EXPORT 
#    else
        /* We are using this library */
#      define X0_EXPORT 
#    endif
#  endif

#  ifndef X0_NO_EXPORT
#    define X0_NO_EXPORT 
#  endif
#endif

#ifndef X0_DEPRECATED
#  define X0_DEPRECATED __declspec(deprecated)
#endif

#ifndef X0_DEPRECATED_EXPORT
#  define X0_DEPRECATED_EXPORT X0_EXPORT X0_DEPRECATED
#endif

#ifndef X0_DEPRECATED_NO_EXPORT
#  define X0_DEPRECATED_NO_EXPORT X0_NO_EXPORT X0_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef X0_NO_DEPRECATED
#    define X0_NO_DEPRECATED
#  endif
#endif

#endif /* X0_EXPORT_H */
