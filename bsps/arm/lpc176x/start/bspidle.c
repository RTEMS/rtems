/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief Idle task.
 */

/*
 * Copyright (c) 2008-2011 embedded brains GmbH.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/lpc176x.h>

void*bsp_idle_thread( const uintptr_t ignored )
{
  while ( true ) {
  }
}
