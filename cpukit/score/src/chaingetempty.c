/**
 * @file
 *
 * @ingroup ScoreChain
 *
 * @brief _Chain_Get_with_empty_check() implementation.
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

bool _Chain_Get_with_empty_check(
  Chain_Control *chain,
  Chain_Node **node
)
{
  ISR_Level level;
  bool is_empty_now;

  _ISR_Disable( level );
  is_empty_now = _Chain_Get_with_empty_check_unprotected( chain, node );
  _ISR_Enable( level );

  return is_empty_now;
}
