/*
 *  bsp_pretasking_hook().
 *  Initializes the heap, libc and VME.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modified to support the MCP750.
 *  Modifications Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  $Id$
 */

#include <string.h>

#include <bsp.h>
#ifndef BSP_HAS_NO_VME
#include <bsp/VME.h>
#endif

#ifdef SHOW_MORE_INIT_SETTINGS
#include <rtems/bspIo.h>
#endif

#include <rtems/malloc.h>

void bsp_libc_init( void *, uint32_t, int );

/*
 *  Function:   bsp_pretasking_hook
 *  Created:    95/03/10
 *
 *  Description:
 *      BSP pretasking hook.  Called just before drivers are initialized.
 *      Used to setup libc and install any BSP extensions.
 *
 *  NOTES:
 *      Must not use libc (to do io) from here, since drivers are
 *      not yet initialized.
 *
 */

void bsp_pretasking_hook(void)
{
  uint32_t        heap_size;
  uint32_t        heap_sbrk_spared;
  extern uint32_t _bsp_sbrk_init(uint32_t, uint32_t*);
  extern void     BSP_vme_config();

  /* make sure it's properly aligned */
  BSP_heap_start = (BSP_heap_start + CPU_ALIGNMENT - 1) & ~(CPU_ALIGNMENT-1);

  heap_size = (BSP_mem_size - BSP_heap_start) - rtems_configuration_get_work_space_size();
  heap_sbrk_spared=_bsp_sbrk_init(BSP_heap_start, &heap_size);

#ifdef SHOW_MORE_INIT_SETTINGS
  printk( "HEAP start %x  size %x (%x bytes spared for sbrk)\n",
             BSP_heap_start, heap_size, heap_sbrk_spared);
#endif    

  /* Must install sbrk helpers since we rely on sbrk for giving
   * us even the first chunk of memory (bsp_libc_init(heap start==NULL))
   */

  rtems_malloc_sbrk_helpers = &rtems_malloc_sbrk_helpers_table;

  bsp_libc_init((void *) 0, heap_size, heap_sbrk_spared);

  /* Note that VME support may be omitted also by
   * providing a NULL BSP_vme_config routine
   * (e.g., linker script)
   */
#ifndef BSP_HAS_NO_VME
  /*
   * Initialize VME bridge - needs working PCI
   * and IRQ subsystems...
   *
   * NOTE: vmeUniverse driver now uses shared interrupts.
   *       this requires malloc/free which are not available
   *       from bspstart()...
   */
#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Going to initialize VME bridge\n");
#endif
  if ( BSP_vme_config )
    BSP_vme_config();
#endif

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Leaving bsp_pretasking_hook\n");
#endif
}
