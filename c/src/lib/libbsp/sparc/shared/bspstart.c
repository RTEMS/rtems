/*
 *  This set of routines are the BSP specific initialization
 *  support routines.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995.
 *  European Space Agency.
 *
 *  $Id$
 */

#include <string.h>

#include <bsp.h>
#include <bsp/bootcard.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <rtems/bspIo.h>

/*
 *  LEON2 Cache Snooping Support
 */
#ifdef LEON2
  /*
   * Tells us if data cache snooping is available
   */ 
  int CPU_SPARC_HAS_SNOOPING;

  /*
   * set_snooping
   * 
   * Read the data cache configuration register to determine if
   * bus snooping is available. This is needed for some drivers so
   * that they can select the most efficient copy routines.  
   */
  static inline int set_snooping(void) 
  {
    unsigned int tmp = *(unsigned int *)0x80000014; /* Cache control register */
    return ((tmp>>23) & 1); /* Data cache snooping enabled */
  }
#endif

/*
 *  BSP pretasking hook.  Called just before drivers are initialized.
 *  Used to setup libc and install any BSP extensions.
 */
void bsp_pretasking_hook(void)
{
  bsp_spurious_initialize();
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */
void bsp_start( void )
{
  #ifdef LEON2
    CPU_SPARC_HAS_SNOOPING = set_snooping();
  #endif
}
