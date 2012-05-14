/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/smplock.h>
#include <rtems/score/smp.h>
#include <rtems/score/isr.h>

/*
 * Some debug stuff that is being left in, but disabled.  This will keep 
 * a log of lock/unlock sequences that can be printed out when the 
 * lockcount appears to have gotten off track.
 */
/* #define SMPLOCK_DEBUG */
#if defined (SMPLOCK_DEBUG)
  #include <rtems/score/thread.h>
  #include <rtems/bspIo.h>
  #include <rtems/score/percpu.h>
  #if (0)
    #define  ENABLE_ONESHOT_DEBUG_LOGGING
  #else
    #define ENABLE_LOOPED_DEBUG_LOGGING
  #endif
  #define ENABLE_DEBUG_LOGGING 
#endif

/*
 * Prototypes and structures used in the debug lock/unlock error log.
 */
#if defined(ENABLE_DEBUG_LOGGING)
  typedef struct {
    char      action;
    char      lock;
    char      cpu;
    char      count;
    uint32_t  nest_level;
    void      *ret1;
    void      *ret2;
    void      *ret3;
    void      *ret4;
  } debug_spinlog_t;

  extern void start(void);
  extern void _fini(void);

  #define DEBUG_SPINLOG_MAX 1024
  debug_spinlog_t DEBUG_SPINLOG[DEBUG_SPINLOG_MAX];
  int DEBUG_SPINLOG_INDEX = 0;

  static void debug_logit(
    char                             act,
    SMP_lock_spinlock_nested_Control *lock
  );
  static void debug_dump_log(void);
#else
  #define debug_logit( _act, _lock )
  #define debug_dump_log()
#endif

/*
 * SMP spinlock simple methods
 */
void _SMP_lock_spinlock_simple_Initialize(
  SMP_lock_spinlock_simple_Control *lock
)
{
  *lock = 0;
}

ISR_Level _SMP_lock_spinlock_simple_Obtain(
  SMP_lock_spinlock_simple_Control *lock
)
{
   ISR_Level  level = 0;
   uint32_t   value = 1;
   uint32_t   previous;

   /* Note: Disable provides an implicit memory barrier. */
  _ISR_Disable_on_this_core( level );
   do {
     RTEMS_COMPILER_MEMORY_BARRIER();
     SMP_CPU_SWAP( lock, value, previous );
     RTEMS_COMPILER_MEMORY_BARRIER();
   } while (previous == 1);

  return level;
}

void _SMP_lock_spinlock_simple_Release(
  SMP_lock_spinlock_simple_Control *lock,
  ISR_Level                        level
)
{
   *lock = 0;
   _ISR_Enable_on_this_core( level );
}

/*
 * SMP spinlock nested methods.
 */
void _SMP_lock_spinlock_nested_Initialize(
  SMP_lock_spinlock_nested_Control *lock
)
{
  lock->lock = 0;
  lock->count = 0;
  lock->cpu_id = -1;
}

void _SMP_lock_spinlock_nested_Release(
  SMP_lock_spinlock_nested_Control *lock,
  ISR_Level                        level
)
{
  #if defined (RTEMS_DEBUG) || defined(SMPLOCK_DEBUG)
    if ( lock->count == 0 ) {
      printk(
        "\ncpu %d lock %d Releasing spinlock when count is already "
        "zero (%p from %p,%p)?!?!\n",
        bsp_smp_processor_id(),  
        lock->cpu_id,
        lock,
        __builtin_return_address(0),
        __builtin_return_address(1)
      );
      debug_dump_log();
      return;
    }
  #endif

  /* assume we actually have it */
  if (lock->count == 1) {
    lock->cpu_id = -1;
    debug_logit( 'U', lock );
    lock->count  = 0;
    RTEMS_COMPILER_MEMORY_BARRIER();
    lock->lock = 0;
  } else {
    debug_logit( 'u', lock );
    lock->count--;
  }

  _ISR_Enable_on_this_core( level ); 
}

ISR_Level _SMP_lock_spinlock_nested_Obtain(
  SMP_lock_spinlock_nested_Control *lock
)
{
  ISR_Level  level = 0;
  uint32_t   value = 1;
  uint32_t   previous;
  int        cpu_id;

  /* Note: Disable provides an implicit memory barrier. */
  _ISR_Disable_on_this_core( level ); 

  cpu_id = bsp_smp_processor_id();

  /*
   *  Attempt to obtain the lock.  If we do not get it immediately, then
   *  do a single "monitor" iteration.  This should allow the loop to back
   *  off the bus a bit and allow the other core to finish sooner.
   */
  while (1) {
    RTEMS_COMPILER_MEMORY_BARRIER();
    SMP_CPU_SWAP( &lock->lock, value, previous );
    RTEMS_COMPILER_MEMORY_BARRIER();
    if ( previous == 0 ) {
      /* was not locked */
      break;
    }

    /* Deal with nested calls from one cpu */
    if (cpu_id == lock->cpu_id) {
      lock->count++;
      debug_logit( 'l', lock );
      return level;
    }
  }

  lock->cpu_id = cpu_id;
  lock->count = 1;
  debug_logit( 'L', lock );

  return level;
}

/*
 * Debug log for debugging nested lock/unlock problems.
 */
#if defined(ENABLE_DEBUG_LOGGING)
  static void debug_logit(
    char                             act,
    SMP_lock_spinlock_nested_Control *lock
  )
  { 
    debug_debug_spinlog_t *sp;
    if ( DEBUG_SPINLOG_INDEX == DEBUG_SPINLOG_MAX )
      #if defined (ENABLE_LOOPED_DEBUG_LOGGING)
        DEBUG_SPINLOG_INDEX = 0;
      #else
        return;
      #endif

    sp = &DEBUG_SPINLOG[ DEBUG_SPINLOG_INDEX++ ];
    sp->action = act;

    #if 0
      if ( lock == &_ISR_SMP_Lock )
        sp->lock = 'I';
      else 
    #endif
    if ( lock == &_Thread_Dispatch_disable_level_lock )
      sp->lock   = 'D';
    sp->cpu    = bsp_smp_processor_id() + '0';
    sp->count  = lock->count;
    #if 0
      if ( sp->lock == 'I' ) {
        if ( _Thread_Dispatch_smp_spin_lock.id == 0 )
          printk( "not nested %p from %p %p %p %p\n", sp,
          __builtin_return_address(0), __builtin_return_address(1),
          __builtin_return_address(2), __builtin_return_address(3) 
        );
      }
    #endif
    sp->nest_level =  _ISR_Nest_level; 
    sp->ret1 = 0;
    sp->ret2 = 0;
    sp->ret3 = 0;
    sp->ret4 = 0;
    sp->ret1 = __builtin_return_address(0);
    if ( sp->ret1 >= start && sp->ret1 <= _fini ) {
      sp->ret2   = __builtin_return_address(1);
      if ( sp->ret2 >= start && sp->ret2 <= _fini ) {
        sp->ret3   = __builtin_return_address(2);
        if ( sp->ret3 >= start && sp->ret3 <= _fini ) {
          sp->ret4   = __builtin_return_address(3);
        }
      }
    }
  }
  
  static void debug_dump_log(void)
  { 
    debug_debug_spinlog_t *sp;
    int       index;
    bool      done =false;
    
    #if defined (ENABLE_ONESHOT_DEBUG_LOGGING)
      index = 0;
    #else
      if (DEBUG_SPINLOG_INDEX >= DEBUG_SPINLOG_MAX)
        index = 0;
      else
        index = DEBUG_SPINLOG_INDEX;
    #endif


    do {
      sp = &DEBUG_SPINLOG[ index ];
      printk("%d) act %c lock %c cpu %c count=%d nest %d (%p, %p, %p, %p)\n",
        index, sp->action, sp->lock, sp->cpu, sp->count, sp->nest_level, 
        sp->ret1, sp->ret2, sp->ret3, sp->ret4
      );

      index++;
      if (index == DEBUG_SPINLOG_INDEX)
        break;
      if (index >= DEBUG_SPINLOG_MAX)
        index = 0;
    } while (1);
  }
#endif
