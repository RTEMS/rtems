/**
 * @file
 *
 * @brief Thread Global Construction
 *
 * @ingroup ScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>
#include <rtems/score/assert.h>
#include <rtems/config.h>

/*
 *  Conditional magic to determine what style of C++ constructor
 *  initialization this target and compiler version uses.
 */
#if defined(__USE_INIT_FINI__)
  #if defined(__M32R__)
    #define INIT_NAME __init
  #elif defined(__ARM_EABI__)
    #define INIT_NAME __libc_init_array
  #else
    #define INIT_NAME _init
  #endif

  extern void INIT_NAME(void);
  #define EXECUTE_GLOBAL_CONSTRUCTORS
#endif

#if defined(__USE__MAIN__)
  extern void __main(void);
  #define INIT_NAME __main
  #define EXECUTE_GLOBAL_CONSTRUCTORS
#endif

void *_Thread_Global_construction( void )
{
  Thread_Control *executing;
  Thread_Entry    entry_point;

#if defined(EXECUTE_GLOBAL_CONSTRUCTORS)
  /*
   *  _init could be a weak symbol and we SHOULD test it but it isn't
   *  in any configuration I know of and it generates a warning on every
   *  RTEMS target configuration.  --joel (12 May 2007)
   */
  INIT_NAME();
#endif

#if defined(RTEMS_POSIX_API)
  if ( Configuration_RTEMS_API.number_of_initialization_tasks > 0 ) {
#endif
    entry_point = (Thread_Entry)
      Configuration_RTEMS_API.User_initialization_tasks_table[ 0 ].entry_point;
#if defined(RTEMS_POSIX_API)
  } else {
    entry_point = (Thread_Entry)
      Configuration_POSIX_API
        .User_initialization_threads_table[ 0 ].thread_entry;
  }
#endif

  _Thread_Disable_dispatch();

  executing = _Thread_Executing;
  executing->Start.entry_point = entry_point;

  _Thread_Restart(
    executing,
    executing,
    executing->Start.pointer_argument,
    executing->Start.numeric_argument
  );

  _Thread_Enable_dispatch();

  _Assert_Not_reached();

  return NULL;
}
