/**
 * @file
 *
 * @ingroup ScoreUserExt
 *
 * @brief User Extension Handler API
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_USEREXTIMPL_H
#define _RTEMS_SCORE_USEREXTIMPL_H

#include <rtems/score/userext.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/percpu.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ScoreUserExt User Extension Handler
 *
 * @ingroup Score
 *
 * @addtogroup ScoreUserExt
 */
/**@{**/

/**
 * @brief List of active extensions.
 */
extern Chain_Control _User_extensions_List;

/**
 * @brief List of active task switch extensions.
 */
extern Chain_Control _User_extensions_Switches_list;

/**
 * @name Extension Maintainance
 */
/**@{**/

void _User_extensions_Handler_initialization( void );

void _User_extensions_Add_set(
  User_extensions_Control *extension
);

RTEMS_INLINE_ROUTINE void _User_extensions_Add_API_set(
  User_extensions_Control *extension
)
{
  _User_extensions_Add_set( extension );
}

RTEMS_INLINE_ROUTINE void _User_extensions_Add_set_with_table(
  User_extensions_Control     *extension,
  const User_extensions_Table *extension_table
)
{
  extension->Callouts = *extension_table;

  _User_extensions_Add_set( extension );
}

void _User_extensions_Remove_set(
  User_extensions_Control *extension
);

/**
 * @brief User extension visitor.
 *
 * @param[in, out] executing The currently executing thread.
 * @param[in, out] arg The argument passed to _User_extensions_Iterate().
 * @param[in] callouts The current callouts.
 */
typedef void (*User_extensions_Visitor)(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
);

typedef struct {
  Thread_Control *created;
  bool            ok;
} User_extensions_Thread_create_context;

void _User_extensions_Thread_create_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
);

void _User_extensions_Thread_delete_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
);

void _User_extensions_Thread_start_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
);

void _User_extensions_Thread_restart_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
);

void _User_extensions_Thread_begin_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
);

void _User_extensions_Thread_exitted_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
);

typedef struct {
  Internal_errors_Source source;
  bool                   is_internal;
  Internal_errors_t      error;
} User_extensions_Fatal_context;

void _User_extensions_Fatal_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
);

void _User_extensions_Thread_terminate_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
);

/**
 * @brief Iterates through all user extensions and calls the visitor for each.
 *
 * @param[in, out] arg The argument passed to the visitor.
 * @param[in] visitor is the visitor for each extension.
 */
void _User_extensions_Iterate(
  void                    *arg,
  User_extensions_Visitor  visitor
);

/** @} */

/**
 * @name Extension Callout Dispatcher
 */
/**@{**/

static inline bool _User_extensions_Thread_create( Thread_Control *created )
{
  User_extensions_Thread_create_context ctx = { created, true };

  _User_extensions_Iterate( &ctx, _User_extensions_Thread_create_visitor );

  return ctx.ok;
}

static inline void _User_extensions_Thread_delete( Thread_Control *deleted )
{
  _User_extensions_Iterate(
    deleted,
    _User_extensions_Thread_delete_visitor
  );
}

static inline void _User_extensions_Thread_start( Thread_Control *started )
{
  _User_extensions_Iterate(
    started,
    _User_extensions_Thread_start_visitor
  );
}

static inline void _User_extensions_Thread_restart( Thread_Control *restarted )
{
  _User_extensions_Iterate(
    restarted,
    _User_extensions_Thread_restart_visitor
  );
}

static inline void _User_extensions_Thread_begin( Thread_Control *executing )
{
  _User_extensions_Iterate(
    executing,
    _User_extensions_Thread_begin_visitor
  );
}

static inline void _User_extensions_Thread_switch(
  Thread_Control *executing,
  Thread_Control *heir
)
{
  const Chain_Control *chain = &_User_extensions_Switches_list;
  const Chain_Node    *tail = _Chain_Immutable_tail( chain );
  const Chain_Node    *node = _Chain_Immutable_first( chain );

  if ( node != tail ) {
    Per_CPU_Control *cpu_self = _Per_CPU_Get();

    _Per_CPU_Acquire( cpu_self );

    while ( node != tail ) {
      const User_extensions_Switch_control *extension =
        (const User_extensions_Switch_control *) node;

      (*extension->thread_switch)( executing, heir );

      node = _Chain_Immutable_next( node );
    }

    _Per_CPU_Release( cpu_self );
  }
}

static inline void _User_extensions_Thread_exitted( Thread_Control *executing )
{
  _User_extensions_Iterate(
    executing,
    _User_extensions_Thread_exitted_visitor
  );
}

static inline void _User_extensions_Fatal(
  Internal_errors_Source source,
  bool                   is_internal,
  Internal_errors_t      error
)
{
  User_extensions_Fatal_context ctx = { source, is_internal, error };

  _User_extensions_Iterate( &ctx, _User_extensions_Fatal_visitor );
}

static inline void _User_extensions_Thread_terminate(
  Thread_Control *executing
)
{
  _User_extensions_Iterate(
    executing,
    _User_extensions_Thread_terminate_visitor
  );
}

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
