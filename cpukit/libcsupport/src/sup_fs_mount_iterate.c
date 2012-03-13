/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/libio_.h>

bool rtems_filesystem_mount_iterate(
  rtems_filesystem_mt_entry_visitor visitor,
  void *visitor_arg
)
{
  rtems_chain_control *chain = &rtems_filesystem_mount_table;
  rtems_chain_node *node = NULL;
  bool stop = false;

  rtems_filesystem_mt_lock();
  for (
    node = rtems_chain_first( chain );
    !rtems_chain_is_tail( chain, node ) && !stop;
    node = rtems_chain_next( node )
  ) {
    const rtems_filesystem_mount_table_entry_t *mt_entry =
      (rtems_filesystem_mount_table_entry_t *) node;

    stop = (*visitor)( mt_entry, visitor_arg );
  }
  rtems_filesystem_mt_unlock();

  return stop;
}
