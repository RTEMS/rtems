#ifndef _RTEMS_POWERPC_CACHE_H
#define _RTEMS_POWERPC_CACHE_H

#include <rtems/powerpc/powerpc.h> /* for PPC_D_CACHE */

#ifdef _OLD_EXCEPTIONS

#if (PPC_D_CACHE != 0)
#define _CPU_Data_Cache_Block_Flush( _address ) \
  do { register void *__address = (_address); \
       register uint32_t   _zero = 0; \
       __asm__ volatile ( "dcbf %0,%1" : \
		      "=r" (_zero), "=r" (__address) : \
                      "0" (_zero), "1" (__address) \
       ); \
  } while (0)
#else
#define _CPU_Data_Cache_Block_Flush( _address ) /* nop */
#endif

/*
 * FIXME: This is not used anywhere.
 */
#if (PPC_D_CACHE != 0)
#define _CPU_Data_Cache_Block_Invalidate( _address ) \
  do { register void *__address = (_address); \
       register uint32_t   _zero = 0; \
       __asm__ volatile ( "dcbi %0,%1" : \
		      "=r" (_zero), "=r" (__address) : \
                      "0" (_zero), "1" (__address) \
       ); \
  } while (0)
#else
#define _CPU_Data_Cache_Block_Invalidate( _address ) /* nop */
#endif
#endif

#endif
