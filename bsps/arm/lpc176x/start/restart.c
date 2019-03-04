/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief Restart implementation.
 */

/*
 * Copyright (c) 2011-2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>

void bsp_restart( const void *const addr )
{
  rtems_interrupt_level level;

  void (*start) ( void ) = addr;

  rtems_interrupt_disable( level );
    (void) level; /* avoid set but not used warning */
    ( *start )();
}
