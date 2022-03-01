/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/cpu.h>

uint32_t _CPU_Counter_frequency( void )
{
  return 1000000000;
}
