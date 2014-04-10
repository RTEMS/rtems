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

#include <bsp/bootcard.h>
#include <bsp/alt_reset_manager.h>
#include "socal/alt_rstmgr.h"
#include "socal/hps.h"

void bsp_reset(void)
{
  uint32_t           self_cpu  = rtems_get_current_processor();
  volatile uint32_t *mpumodrst = ALT_RSTMGR_MPUMODRST_ADDR;
  
  if( self_cpu == 0 ) {
    /* Reset CPU1 */
    (*mpumodrst) |= ALT_RSTMGR_MPUMODRST_CPU1_SET_MSK;
    
    /* Simply call the reset method from alteras HWLIB */
    (void) alt_reset_cold_reset();
  } else {
    /* Keep CPU1 waiting until it gets reset by CPU0 */
    while ( true ) {
      __asm__ volatile ("wfi");
    }
  }
}
