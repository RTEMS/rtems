/**
 * @file
 *
 * @ingroup RTEMSScoreWatchdog
 *
 * @brief Watchdog Insert
 */

/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#include <rtems/score/watchdogimpl.h>

void _Watchdog_Insert(
  Watchdog_Header  *header,
  Watchdog_Control *the_watchdog,
  uint64_t          expire
)
{
  RBTree_Node **link;
  RBTree_Node  *parent;
  RBTree_Node  *old_first;
  RBTree_Node  *new_first;

  _Assert( _Watchdog_Get_state( the_watchdog ) == WATCHDOG_INACTIVE );

  link = _RBTree_Root_reference( &header->Watchdogs );
  parent = NULL;
  old_first = header->first;
  new_first = &the_watchdog->Node.RBTree;

  the_watchdog->expire = expire;

  while ( *link != NULL ) {
    Watchdog_Control *parent_watchdog;

    parent = *link;
    parent_watchdog = (Watchdog_Control *) parent;

    if ( expire < parent_watchdog->expire ) {
      link = _RBTree_Left_reference( parent );
    } else {
      link = _RBTree_Right_reference( parent );
      new_first = old_first;
    }
  }

  header->first = new_first;
  _RBTree_Initialize_node( &the_watchdog->Node.RBTree );
  _RBTree_Add_child( &the_watchdog->Node.RBTree, parent, link );
  _RBTree_Insert_color( &header->Watchdogs, &the_watchdog->Node.RBTree );
}
