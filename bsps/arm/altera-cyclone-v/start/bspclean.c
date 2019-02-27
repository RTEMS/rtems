/**
 * @file
 *
 * @ingroup RTEMSBSPsARMCycV
 */

/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
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

#include <bsp.h>
#include <bsp/bootcard.h>
#include <rtems/bspIo.h>
#include <rtems/score/smpimpl.h>

void bsp_fatal_extension(
  rtems_fatal_source src,
  bool always_set_to_false,
  rtems_fatal_code code
)
{
#ifdef RTEMS_SMP
  if (src == RTEMS_FATAL_SOURCE_SMP && code == SMP_FATAL_SHUTDOWN_RESPONSE) {
    while (true) {
      _ARM_Wait_for_event();
    }
  }
#endif

#if BSP_PRINT_EXCEPTION_CONTEXT
  if (src == RTEMS_FATAL_SOURCE_EXCEPTION) {
    rtems_exception_frame_print((const rtems_exception_frame *) code);
  }
#endif

#if BSP_RESET_BOARD_AT_EXIT
  bsp_reset();
#endif
}
