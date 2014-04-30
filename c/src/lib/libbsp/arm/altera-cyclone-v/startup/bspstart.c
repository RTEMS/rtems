/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <assert.h>
#include <stdint.h>
#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/irq-generic.h>
#include <bsp/linker-symbols.h>
#include <bsp/start.h>
#include <bsp/nocache-heap.h>
#include <rtems/config.h>
#include "socal/alt_rstmgr.h"
#include "socal/alt_sysmgr.h"
#include "socal/hps.h"

#ifndef MIN
#define MIN( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )
#endif

#define BSPSTART_MAX_CORES_PER_CONTROLLER 2

#ifdef RTEMS_NETWORKING
/* FIXME: This is a workaround for the broken cache manager support */

#include <rtems/rtems_bsdnet.h>

void* rtems_bsdnet_malloc_mbuf(size_t size, int type)
{
  (void)type;
  return altera_cyclone_v_nocache_malloc(size);
}
#endif

static void bsp_start_secondary_cores( void )
{
#ifdef RTEMS_SMP
  volatile uint32_t *mpumodrst       = ALT_RSTMGR_MPUMODRST_ADDR;
  uint32_t          *cpu1_start_addr = (
    ALT_SYSMGR_ROMCODE_ADDR + ALT_SYSMGR_ROMCODE_CPU1STARTADDR_OFST );
  const uint32_t     CORES           = MIN(
    (uintptr_t) bsp_processor_count,
    rtems_configuration_get_maximum_processors() );
  unsigned int       index;


  /* Memory would get overwritten if a too small processor count
   * would be specified */
  assert( 
    (uintptr_t) bsp_processor_count >= BSPSTART_MAX_CORES_PER_CONTROLLER );

  if ( (uintptr_t) bsp_processor_count >= BSPSTART_MAX_CORES_PER_CONTROLLER ) {
    for ( index = 1; index < CORES; ++index ) {
      /* set the start address from where the core will execute */
      (*cpu1_start_addr) = ALT_SYSMGR_ROMCODE_CPU1STARTADDR_VALUE_SET(
        (uintptr_t) _start );

      /* Make the core finish it's reset */
      (*mpumodrst) &= ~ALT_RSTMGR_MPUMODRST_CPU1_SET_MSK;
    }
  }
#endif /* #ifdef RTEMS_SMP */
}

void bsp_start( void )
{
  bsp_interrupt_initialize();
  altera_cyclone_v_nocache_init_heap();
  bsp_start_secondary_cores();
}
