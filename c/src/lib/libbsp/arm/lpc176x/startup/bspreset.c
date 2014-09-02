/**
 * @file
 *
 * @ingroup lpc176x
 *
 * @brief Reset code.
 */

/*
 * Copyright (c) 2008-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/score/armv7m.h>

#include <bsp/bootcard.h>
#include <bsp/lpc176x.h>
#include <bsp/start.h>

BSP_START_TEXT_SECTION __attribute__( ( flatten ) ) void bsp_reset( void )
{
  rtems_interrupt_level level;

  (void) level;
  rtems_interrupt_disable( level );

  _ARMV7M_SCB->aircr = ARMV7M_SCB_AIRCR_VECTKEY |
                       ARMV7M_SCB_AIRCR_SYSRESETREQ;

  while ( true ) {
    /* Do nothing */
  }
}
