/*
 * Copyright (c) 2011 Sebastian Huber.  All rights reserved.
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

#include <rtems/score/cpu.h>

#ifdef ARM_MULTILIB_ARCH_V7M

#include <rtems/score/armv7m.h>

void _CPU_Initialize( void )
{
  /*
   * The exception handler used to carry out the thead dispatching must have
   * the lowest priority possible.  No other exception handlers must have this
   * priority if they use services that may lead to a thread dispatch.  See
   * also "ARMv7-M Architecture Reference Manual, Issue D" section B1.5.4
   * "Exception priorities and preemption".
   */
  _ARMV7M_Set_exception_priority_and_handler(
    ARMV7M_VECTOR_SVC,
    ARMV7M_EXCEPTION_PRIORITY_LOWEST,
    _ARMV7M_Supervisor_call
  );
  _ARMV7M_Set_exception_priority_and_handler(
    ARMV7M_VECTOR_PENDSV,
    ARMV7M_EXCEPTION_PRIORITY_LOWEST,
    _ARMV7M_Pendable_service_call
  );
}

#endif /* ARM_MULTILIB_ARCH_V7M */
