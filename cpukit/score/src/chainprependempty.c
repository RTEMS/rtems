/**
 * @file
 *
 * @ingroup ScoreChain
 *
 * @brief _Chain_Prepend_with_empty_check() implementation.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
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

#include <rtems/system.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>

bool _Chain_Prepend_with_empty_check(
  Chain_Control *chain,
  Chain_Node *node
)
{
  ISR_Level level;
  bool was_empty;

  _ISR_Disable( level );
  was_empty = _Chain_Prepend_with_empty_check_unprotected( chain, node );
  _ISR_Enable( level );

  return was_empty;
}
