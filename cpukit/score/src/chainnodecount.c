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

#include <rtems/score/chainimpl.h>

size_t _Chain_Node_count_unprotected( const Chain_Control *chain )
{
  size_t            count = 0;
  const Chain_Node *tail  = _Chain_Immutable_tail( chain );
  const Chain_Node *node  = _Chain_Immutable_first( chain );

  while ( node != tail ) {
    ++count;

    node = _Chain_Immutable_next( node );
  }

  return count;
}
