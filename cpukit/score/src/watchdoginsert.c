/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreWatchdog
 *
 * @brief This source file contains the implementation of
 *   _Watchdog_Insert().
 */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
