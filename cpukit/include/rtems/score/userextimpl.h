/**
 * @file
 *
 * @ingroup RTEMSScoreUserExt
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
 * @addtogroup RTEMSScoreUserExt
 *
 * @{
 */

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
 *
 * @{
 */

/**
 * @brief Initializes the user extensions handler.
 */
void _User_extensions_Handler_initialization( void );

/**
 * @brief Adds a user extension.
 *
 * @param extension The user extension to add.
 */
void _User_extensions_Add_set(
  User_extensions_Control *extension
);

/**
 * @brief Adds a user extension.
 *
 * @param extension The user extension to add.
 */
RTEMS_INLINE_ROUTINE void _User_extensions_Add_API_set(
  User_extensions_Control *extension
)
{
  _User_extensions_Add_set( extension );
}

/**
 * @brief Adds a user extension with the given extension table as callouts.
 *
 * @param[in, out] extension The user extension to add.
 * @param extension_table Is set as callouts for @a extension.
 */
RTEMS_INLINE_ROUTINE void _User_extensions_Add_set_with_table(
  User_extensions_Control     *extension,
  const User_extensions_Table *extension_table
)
{
  extension->Callouts = *extension_table;

  _User_extensions_Add_set( extension );
}

/**
 * @brief Removes a user extension.
 *
 * @param extension The user extension to remove.
 */
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

/**
 * @brief Creates a visitor.
 *
 * @param executing The currently executing thread.
 * @param[in, out] arg Is used as the thread create context for the operation.
 * @param callouts The user extension table for the operation.
 */
void _User_extensions_Thread_create_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
);

/**
 * @brief Deletes a visitor.
 *
 * @param executing The currently executing thread.
 * @param[in, out] arg Parameter for the callout.
 * @param callouts The user extension table for the operation.
 */
void _User_extensions_Thread_delete_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
);

/**
 * @brief Starts a visitor.
 *
 * @param executing The currently executing thread.
 * @param arg Parameter for the callout.
 * @param callouts The user extension table for the operation.
 */
void _User_extensions_Thread_start_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
);

/**
 * @brief Restarts a visitor.
 *
 * @param executing The currently executing thread.
 * @param arg Parameter for the callout.
 * @param callouts The user extension table for the operation.
 */
void _User_extensions_Thread_restart_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
);

/**
 * @brief Calls the begin function of the thread callout for the visitor.
 *
 * @param executing The currently executing thread.
 * @param arg This parameter is unused.
 * @param callouts The user extension table for the operation.
 */
void _User_extensions_Thread_begin_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
);

/**
 * @brief Calls the exitted function of the thread callout for the visitor.
 *
 * @param executing The currently executing thread.
 * @param arg This parameter is unused.
 * @param callouts The user extension table for the operation.
 */
void _User_extensions_Thread_exitted_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
);

typedef struct {
  Internal_errors_Source source;
  Internal_errors_t      error;
} User_extensions_Fatal_context;

/**
 * @brief Calls the fatal function of the thread callout for the visitor.
 *
 * @param executing The currently executing thread.
 * @param arg Is used as the user extension fatal context.
 * @param callouts The user extension table for the operation.
 */
void _User_extensions_Fatal_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
);

/**
 * @brief Terminates a visitor.
 *
 * @param executing The currently executing thread.
 * @param arg This parameter is unused.
 * @param callouts The user extension table for the operation.
 */
void _User_extensions_Thread_terminate_visitor(
  Thread_Control              *executing,
  void                        *arg,
  const User_extensions_Table *callouts
);

/**
 * @brief Iterates through all user extensions and calls the visitor for each.
 *
 * @param[in, out] arg The argument passed to the visitor.
 * @param visitor The visitor for each extension.
 * @param direction The iteration direction for dynamic extensions.
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
/** @{ **/

/**
 * @brief Creates a thread.
 *
 * @param[out] created The thread to create.
 *
 * @retval true The operation succeeded.
 * @retval false The operation failed.
 */
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

/**
 * @brief Deletes a thread.
 *
 * @param[out] created The thread to delete.
 */
static inline void _User_extensions_Thread_delete( Thread_Control *deleted )
{
  _User_extensions_Iterate(
    deleted,
    _User_extensions_Thread_delete_visitor,
    CHAIN_ITERATOR_BACKWARD
  );
}

/**
 * @brief Starts a thread.
 *
 * @param created The thread to start.
 */
static inline void _User_extensions_Thread_start( Thread_Control *started )
{
  _User_extensions_Iterate(
    started,
    _User_extensions_Thread_start_visitor,
    CHAIN_ITERATOR_FORWARD
  );
}

/**
 * @brief Restarts a thread.
 *
 * @param created The thread to restart.
 */
static inline void _User_extensions_Thread_restart( Thread_Control *restarted )
{
  _User_extensions_Iterate(
    restarted,
    _User_extensions_Thread_restart_visitor,
    CHAIN_ITERATOR_FORWARD
  );
}

/**
 * @brief Begins a thread.
 *
 * @param created The thread to begin.
 */
static inline void _User_extensions_Thread_begin( Thread_Control *executing )
{
  _User_extensions_Iterate(
    executing,
    _User_extensions_Thread_begin_visitor,
    CHAIN_ITERATOR_FORWARD
  );
}

/**
 * @brief Switches the thread from the executing to the heir.
 *
 * @param executing The currently executing thread.
 * @param heir The thread that will switch with @a executing.
 */
static inline void _User_extensions_Thread_switch(
  Thread_Control *executing,
  Thread_Control *heir
)
{
  const Chain_Control *chain;
  const Chain_Node    *tail;
  const Chain_Node    *node;

  chain = &_User_extensions_Switches_list;
  tail = _Chain_Immutable_tail( chain );
  node = _Chain_Immutable_first( chain );

  if ( node != tail ) {
#if defined(RTEMS_SMP)
    ISR_lock_Context  lock_context;
    Per_CPU_Control  *cpu_self;

    cpu_self = _Per_CPU_Get();

    _ISR_lock_ISR_disable( &lock_context );
    _Per_CPU_Acquire( cpu_self, &lock_context );

    node = _Chain_Immutable_first( chain );
#endif

    while ( node != tail ) {
      const User_extensions_Switch_control *extension;

      extension = (const User_extensions_Switch_control *) node;
      node = _Chain_Immutable_next( node );
      (*extension->thread_switch)( executing, heir );
    }

#if defined(RTEMS_SMP)
    _Per_CPU_Release( cpu_self, &lock_context );
    _ISR_lock_ISR_enable( &lock_context );
#endif
  }
}

/**
 * @brief A user extension thread exitted.
 *
 * @param created The thread.
 */
static inline void _User_extensions_Thread_exitted( Thread_Control *executing )
{
  _User_extensions_Iterate(
    executing,
    _User_extensions_Thread_exitted_visitor,
    CHAIN_ITERATOR_FORWARD
  );
}

/**
 * @brief Forwards all visitors that there was a fatal failure.
 *
 * @param source The error source.
 * @param error The error.
 */
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

/**
 * @brief Terminates the executing thread.
 *
 * @param executing The currently executing thread.
 */
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

/**
 * @brief Disables interrupts and acquires the lock context.
 *
 * @param lock_context The lock context to acquire.
 */
static inline void _User_extensions_Acquire( ISR_lock_Context *lock_context )
{
  _ISR_lock_ISR_disable_and_acquire(
    &_User_extensions_List.Lock,
    lock_context
  );
}

/**
 * @brief Releases the lock context and enables interrupts.
 *
 * @param lock_context The lock context to release.
 */
static inline void _User_extensions_Release( ISR_lock_Context *lock_context )
{
  _ISR_lock_Release_and_ISR_enable(
    &_User_extensions_List.Lock,
    lock_context
  );
}

/**
 * @brief Destroys all user extension iterators of a thread.
 *
 * @param[in, out] the_thread The thread to destroy all user extension
 *      iterators of.
 */
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
