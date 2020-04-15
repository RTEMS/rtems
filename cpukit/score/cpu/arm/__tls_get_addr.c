/*
 * Copyright (c) 2014-2015 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>
#include <rtems/score/tls.h>

#include <assert.h>

void *__tls_get_addr(const TLS_Index *ti);

void *__tls_get_addr(const TLS_Index *ti)
{
  const Thread_Control *executing = _Thread_Get_executing();
  void *tls_block = (char *) executing->Start.tls_area
    + _TLS_Get_thread_control_block_area_size( (uintptr_t) _TLS_Alignment );

  assert(ti->module == 1);

  return (char *) tls_block + ti->offset;
}
