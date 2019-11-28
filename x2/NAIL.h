
#ifndef NAIL_API_H
#define NAIL_API_H

#ifdef NAIL_STATIC_DEFINE
#  define NAIL_API
#  define NAIL_NO_EXPORT
#else
#  ifndef NAIL_API
#    ifdef libx2_EXPORTS
        /* We are building this library */
#      define NAIL_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define NAIL_API __declspec(dllimport)
#    endif
#  endif

#  ifndef NAIL_NO_EXPORT
#    define NAIL_NO_EXPORT 
#  endif
#endif

#ifndef NAIL_DEPRECATED
#  define NAIL_DEPRECATED __declspec(deprecated)
#endif

#ifndef NAIL_DEPRECATED_EXPORT
#  define NAIL_DEPRECATED_EXPORT NAIL_API NAIL_DEPRECATED
#endif

#ifndef NAIL_DEPRECATED_NO_EXPORT
#  define NAIL_DEPRECATED_NO_EXPORT NAIL_NO_EXPORT NAIL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef NAIL_NO_DEPRECATED
#    define NAIL_NO_DEPRECATED
#  endif
#endif

#endif /* NAIL_API_H */
