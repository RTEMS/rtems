/**
 *  @file
 *
 *  @brief RTEMS Output a Character
 *  @ingroup libcsupport
 */

/*
 * Copyright (c) 2012, 2017 embedded brains GmbH.  All rights reserved.
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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/bspIo.h>

void rtems_putc(char c)
{
  if (c == '\n') {
    (*BSP_output_char)('\r');
  }

  (*BSP_output_char)(c);
}
