/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
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

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/nios2-utility.h>

bool _ISR_Is_in_progress( void )
{
  if ( _Nios2_Has_internal_interrupt_controller() ) {
    return _ISR_Nest_level != 0;
  } else {
    uint32_t status = _Nios2_Get_ctlreg_status();

    return (status & NIOS2_STATUS_IH) != 0;
  }
}
