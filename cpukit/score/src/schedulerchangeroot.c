/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/schedulerimpl.h>

typedef struct {
  Thread_Control *root;
  Thread_Control *needs_help;
} Scheduler_Set_root_context;

RTEMS_INLINE_ROUTINE bool _Scheduler_Set_root_visitor(
  Resource_Node *resource_node,
  void          *arg
)
{
  Scheduler_Set_root_context *ctx = arg;
  Thread_Control *root = ctx->root;
  Thread_Control *needs_help = root;
  Thread_Control *offers_help =
    THREAD_RESOURCE_NODE_TO_THREAD( resource_node );
  const Scheduler_Control *scheduler = _Scheduler_Get_own( offers_help );
  Thread_Control *needs_help_too;

  _Resource_Node_set_root( resource_node, &root->Resource_node );

  needs_help_too = ( *scheduler->Operations.ask_for_help )(
    scheduler,
    offers_help,
    needs_help
  );

  if ( needs_help_too != needs_help && needs_help_too != NULL ) {
    _Assert( ctx->needs_help == NULL );
    ctx->needs_help = needs_help_too;
  }

  return false;
}

void _Scheduler_Thread_change_resource_root(
  Thread_Control *top,
  Thread_Control *root
)
{
  Scheduler_Set_root_context ctx = { root, NULL };
  Thread_Control *offers_help = top;
  Scheduler_Node *offers_help_node;
  Thread_Control *offers_help_too;
  ISR_Level level;

  _ISR_Disable( level );

  offers_help_node = _Scheduler_Thread_get_node( offers_help );
  offers_help_too = _Scheduler_Node_get_owner( offers_help_node );

  if ( offers_help != offers_help_too ) {
    _Scheduler_Set_root_visitor( &offers_help_too->Resource_node, &ctx );
    _Assert( ctx.needs_help == offers_help );
    ctx.needs_help = NULL;
  }

  _Scheduler_Set_root_visitor( &top->Resource_node, &ctx );
  _Resource_Iterate( &top->Resource_node, _Scheduler_Set_root_visitor, &ctx );

  if ( ctx.needs_help != NULL ) {
    _Scheduler_Ask_for_help( ctx.needs_help );
  }

  _ISR_Enable( level );
}
