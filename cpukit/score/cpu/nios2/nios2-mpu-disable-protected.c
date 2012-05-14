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

#include <rtems/score/isr.h>
#include <rtems/score/nios2-utility.h>

uint32_t _Nios2_MPU_Disable_protected( void )
{
  ISR_Level level;
  uint32_t config;

  _ISR_Disable( level );
  config = _Nios2_MPU_Disable();
  _ISR_Enable( level );

  return config;
}
