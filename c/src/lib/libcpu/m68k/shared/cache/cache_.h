/*
 *  M68K Cache Manager Support
 */

#ifndef __M68K_CACHE_h
#define __M68K_CACHE_h

#if (defined(__mc68020__) && !defined(__mcpu32__))
# define M68K_INSTRUCTION_CACHE_ALIGNMENT 16
#elif defined(__mc68030__)
# define M68K_INSTRUCTION_CACHE_ALIGNMENT 16
# define M68K_DATA_CACHE_ALIGNMENT 16
#elif ( defined(__mc68040__) || defined (__mc68060__) )
# define M68K_INSTRUCTION_CACHE_ALIGNMENT 16
# define M68K_DATA_CACHE_ALIGNMENT 16
#elif ( defined(__mcf5200__) )
# define M68K_INSTRUCTION_CACHE_ALIGNMENT 16
# if ( defined(__mcf528x__) )
#  define M68K_DATA_CACHE_ALIGNMENT 16
# endif
#elif ( defined(__mcf5300__) )
# define M68K_INSTRUCTION_CACHE_ALIGNMENT 16
# define M68K_DATA_CACHE_ALIGNMENT 16
#elif defined(__mcfv4e__)
# define M68K_INSTRUCTION_CACHE_ALIGNMENT 16
# define M68K_DATA_CACHE_ALIGNMENT 16
#endif

#if defined(M68K_DATA_CACHE_ALIGNMENT)
#define CPU_DATA_CACHE_ALIGNMENT M68K_DATA_CACHE_ALIGNMENT
#endif

#if defined(M68K_INSTRUCTION_CACHE_ALIGNMENT)
#define CPU_INSTRUCTION_CACHE_ALIGNMENT M68K_INSTRUCTION_CACHE_ALIGNMENT
#endif

#include <libcpu/cache.h>

#endif
/* end of include file */
