/*  Shared Memory Lock Routines
 *
 *  This shared memory locked queue support routine need to be
 *  able to lock the specified locked queue.  Interrupts are
 *  disabled while the queue is locked to prevent preemption
 *  and deadlock when two tasks poll for the same lock.
 *  previous level.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <shm_driver.h>

/*
 *  Shm_Initialize_lock
 *
 *  Initialize the lock for the specified locked queue.
 */

void Shm_Initialize_lock(
  Shm_Locked_queue_Control *lq_cb
)
{
  lq_cb->lock = LQ_UNLOCKED;
}

/*  void _Shm_Lock( &lq_cb )
 *
 *  This shared memory locked queue support routine locks the
 *  specified locked queue.  It disables interrupts to prevent
 *  a deadlock condition.
 */

void Shm_Lock(
  Shm_Locked_queue_Control *lq_cb
)
{
  uint32_t         isr_level;
  uint32_t         *lockptr = (uint32_t*) &lq_cb->lock;
  uint32_t         lock_value;

  lock_value = 0x80000000;
  rtems_interrupt_disable( isr_level );

    Shm_isrstat = isr_level;
    while ( lock_value ) {
      __asm__ volatile( ""
                         : "=r" (lockptr), "=r" (lock_value)
                         : "0" (lockptr),  "1" (lock_value)
                  );
      /*
       *  If not available, then may want to delay to reduce load on lock.
       */

      if ( lock_value )
        rtems_bsp_delay( 10 );   /* approximately 10 microseconds */
   }
}

/*
 *  Shm_Unlock
 *
 *  Unlock the lock for the specified locked queue.
 */

void Shm_Unlock(
  Shm_Locked_queue_Control *lq_cb
)
{
  uint32_t         isr_level;

  lq_cb->lock = SHM_UNLOCK_VALUE;
  isr_level = Shm_isrstat;
  rtems_interrupt_enable( isr_level );
}
