/*
 *  Cache Management Support Routines for the MC68040
 *
 *  $Id$
 */

#include <rtems.h>
#include "cache_.h"


#if defined(HAS_INSTRUCTION_CACHE)

void _CPU_cache_invalidate_entire_instruction ( void )
{
  asm volatile ("flush");
}
#endif

/* end of file */
