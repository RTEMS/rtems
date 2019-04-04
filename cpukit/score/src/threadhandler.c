/**
 *  @file
 *
 *  @brief Thread Handler
 *  @ingroup RTEMSScoreThread
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
#include <rtems/score/interr.h>
#include <rtems/score/isrlevel.h>
#include <rtems/score/userextimpl.h>

/*
 *  Conditional magic to determine what style of C++ constructor
 *  initialization this target and compiler version uses.
 */
#if defined(__USE_INIT_FINI__)
  #if defined(__ARM_EABI__) || defined(__riscv) \
    || defined(__or1k__) || defined(__or1knd__)
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

Objects_Id _Thread_Global_constructor;

static void _Thread_Global_construction( Thread_Control *executing )
{
#if defined(EXECUTE_GLOBAL_CONSTRUCTORS)
  if ( executing->Object.id == _Thread_Global_constructor ) {
    /*
     * Prevent double construction in case the initialization thread is deleted
     * and then recycled.  There is not need for extra synchronization since
     * this variable is set during the sequential system boot procedure.
     */
    _Thread_Global_constructor = 0;

    /*
     *  _init could be a weak symbol and we SHOULD test it but it isn't
     *  in any configuration I know of and it generates a warning on every
     *  RTEMS target configuration.  --joel (12 May 2007)
     */
    INIT_NAME();
  }
#endif
}

void _Thread_Handler( void )
{
  Thread_Control  *executing;
  ISR_Level        level;
  Per_CPU_Control *cpu_self;

  /*
   * Some CPUs need to tinker with the call frame or registers when the
   * thread actually begins to execute for the first time.  This is a
   * hook point where the port gets a shot at doing whatever it requires.
   */
  _Context_Initialization_at_thread_begin();
  executing = _Thread_Executing;

  /*
   * have to put level into a register for those cpu's that use
   * inline asm here
   */
  level = executing->Start.isr_level;
  _ISR_Set_level( level );

  /*
   * Initialize the floating point context because we do not come
   * through _Thread_Dispatch on our first invocation. So the normal
   * code path for performing the FP context switch is not hit.
   */
  _Thread_Restore_fp( executing );

  /*
   * Do not use the level of the thread control block, since it has a
   * different format.
   */
  _ISR_Local_disable( level );

  /*
   *  At this point, the dispatch disable level BETTER be 1.
   */
  cpu_self = _Per_CPU_Get();
  _Assert( cpu_self->thread_dispatch_disable_level == 1 );

  /*
   * Make sure we lose no thread dispatch necessary update and execute the
   * post-switch actions.  As a side-effect change the thread dispatch level
   * from one to zero.  Do not use _Thread_Enable_dispatch() since there is no
   * valid thread dispatch necessary indicator in this context.
   */
  _Thread_Do_dispatch( cpu_self, level );

  /*
   * Invoke the thread begin extensions in the context of the thread entry
   * function with thread dispatching enabled.  This enables use of dynamic
   * memory allocation, creation of POSIX keys and use of C++ thread local
   * storage.  Blocking synchronization primitives are allowed also.
   */
  _User_extensions_Thread_begin( executing );

  _Thread_Global_construction( executing );

  /*
   *  RTEMS supports multiple APIs and each API can define a different
   *  thread/task prototype. The following code supports invoking the
   *  user thread entry point using the prototype expected.
   */
  ( *executing->Start.Entry.adaptor )( executing );

  /*
   *  In the call above, the return code from the user thread body which return
   *  something was placed in return_argument.  This assumed that if it
   *  returned anything (which is not supporting in all APIs), then it would be
   *  able to fit in a (void *).
   */

  _User_extensions_Thread_exitted( executing );

  _Internal_error( INTERNAL_ERROR_THREAD_EXITTED );
}
