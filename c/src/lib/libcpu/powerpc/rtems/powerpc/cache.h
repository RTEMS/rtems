/*
 * $Id$
 */

#ifndef _rtems_powerpc_cache_h
#define _rtems_powerpc_cache_h

#include <rtems/score/ppc.h>

#ifdef _OLD_EXCEPTIONS

#if (PPC_D_CACHE != 0)
#define _CPU_Data_Cache_Block_Flush( _address ) \
  do { register void *__address = (_address); \
       register unsigned32 _zero = 0; \
       asm volatile ( "dcbf %0,%1" : \
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
       register unsigned32 _zero = 0; \
       asm volatile ( "dcbi %0,%1" : \
		      "=r" (_zero), "=r" (__address) : \
                      "0" (_zero), "1" (__address) \
       ); \
  } while (0)
#else
#define _CPU_Data_Cache_Block_Invalidate( _address ) /* nop */
#endif
#endif

#endif
