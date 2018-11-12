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

#include <rtems/score/userextdata.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/thread.h>
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
 * @brief Chain iterator for dynamic user extensions.
 *
 * Since user extensions may delete or restart the executing thread, we must
 * clean up registered iterators.
 *
 * @see _User_extensions_Iterate(), _User_extensions_Destroy_iterators() and
 *   Thread_Control::last_user_extensions_iterator.
 */
typedef struct User_extensions_Iterator {
  Chain_Iterator                   Iterator;
  struct User_extensions_Iterator *previous;
} User_extensions_Iterator;

typedef struct {
  /**
   * @brief Active dynamically added user extensions.
   */
  Chain_Control Active;

  /**
   * @brief Chain iterator registration.
   */
  Chain_Iterator_registry Iterators;

  /**
   * @brief Lock to protect User_extensions_List::Active and
   * User_extensions_List::Iterators.
   */
  ISR_LOCK_MEMBER( Lock )
} User_extensions_List;

/**
 * @brief List of active extensions.
 */
extern User_extensions_List _User_extensions_List;

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
 * @param[in] visitor The visitor for each extension.
 * @param[in] direction The iteration direction for dynamic extensions.
 */
void _User_extensions_Iterate(
  void                     *arg,
  User_extensions_Visitor   visitor,
  Chain_Iterator_direction  direction
);

/** @} */

/**
 * @name Extension Callout Dispatcher
 */
/**@{**/

static inline bool _User_extensions_Thread_create( Thread_Control *created )
{
  User_extensions_Thread_create_context ctx = { created, true };

  _User_extensions_Iterate(
    &ctx,
    _User_extensions_Thread_create_visitor,
    CHAIN_ITERATOR_FORWARD
  );

  return ctx.ok;
}

static inline void _User_extensions_Thread_delete( Thread_Control *deleted )
{
  _User_extensions_Iterate(
    deleted,
    _User_extensions_Thread_delete_visitor,
    CHAIN_ITERATOR_BACKWARD
  );
}

static inline void _User_extensions_Thread_start( Thread_Control *started )
{
  _User_extensions_Iterate(
    started,
    _User_extensions_Thread_start_visitor,
    CHAIN_ITERATOR_FORWARD
  );
}

static inline void _User_extensions_Thread_restart( Thread_Control *restarted )
{
  _User_extensions_Iterate(
    restarted,
    _User_extensions_Thread_restart_visitor,
    CHAIN_ITERATOR_FORWARD
  );
}

static inline void _User_extensions_Thread_begin( Thread_Control *executing )
{
  _User_extensions_Iterate(
    executing,
    _User_extensions_Thread_begin_visitor,
    CHAIN_ITERATOR_FORWARD
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
    Per_CPU_Control *cpu_self;
#if defined(RTEMS_SMP)
    ISR_Level        level;
#endif

    cpu_self = _Per_CPU_Get();

#if defined(RTEMS_SMP)
    _ISR_Local_disable( level );
#endif
    _Per_CPU_Acquire( cpu_self );

    while ( node != tail ) {
      const User_extensions_Switch_control *extension =
        (const User_extensions_Switch_control *) node;

      (*extension->thread_switch)( executing, heir );

      node = _Chain_Immutable_next( node );
    }

    _Per_CPU_Release( cpu_self );
#if defined(RTEMS_SMP)
    _ISR_Local_enable( level );
#endif
  }
}

static inline void _User_extensions_Thread_exitted( Thread_Control *executing )
{
  _User_extensions_Iterate(
    executing,
    _User_extensions_Thread_exitted_visitor,
    CHAIN_ITERATOR_FORWARD
  );
}

static inline void _User_extensions_Fatal(
  Internal_errors_Source source,
  Internal_errors_t      error
)
{
  User_extensions_Fatal_context ctx = { source, error };

  _User_extensions_Iterate(
    &ctx,
    _User_extensions_Fatal_visitor,
    CHAIN_ITERATOR_FORWARD
  );
}

static inline void _User_extensions_Thread_terminate(
  Thread_Control *executing
)
{
  _User_extensions_Iterate(
    executing,
    _User_extensions_Thread_terminate_visitor,
    CHAIN_ITERATOR_BACKWARD
  );
}

static inline void _User_extensions_Acquire( ISR_lock_Context *lock_context )
{
  _ISR_lock_ISR_disable_and_acquire(
    &_User_extensions_List.Lock,
    lock_context
  );
}

static inline void _User_extensions_Release( ISR_lock_Context *lock_context )
{
  _ISR_lock_Release_and_ISR_enable(
    &_User_extensions_List.Lock,
    lock_context
  );
}

static inline void _User_extensions_Destroy_iterators(
  Thread_Control *the_thread
)
{
  ISR_lock_Context          lock_context;
  User_extensions_Iterator *iter;

  _User_extensions_Acquire( &lock_context );

  iter = the_thread->last_user_extensions_iterator;

  while ( iter != NULL ) {
    _Chain_Iterator_destroy( &iter->Iterator );
    iter = iter->previous;
  }

  _User_extensions_Release( &lock_context );
}

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
