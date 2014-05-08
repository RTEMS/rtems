/**
 *  @file
 *
 *  LEON3 Shared Memory Lock Routines
 *
 *  This shared memory locked queue support routine need to be
 *  able to lock the specified locked queue.  Interrupts are
 *  disabled while the queue is locked to prevent preemption
 *  and deadlock when two tasks poll for the same lock.
 *  previous level.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>
#include <shm_driver.h>


/*
 *  Initialize the lock for the specified locked queue.
 */
void Shm_Initialize_lock(
  Shm_Locked_queue_Control *lq_cb
)
{
  lq_cb->lock = LQ_UNLOCKED;
}

/*
 *  This shared memory locked queue support routine locks the
 *  specified locked queue.  It disables interrupts to prevent
 *  a deadlock condition.
 */
extern unsigned int LEON3_Atomic_Swap(uint32_t value, uint32_t *address);

__asm__ (
    ".text\n"
    ".align 4\n"
    "LEON3_Atomic_Swap:\n"
    "  retl\n"
    "  swapa [%o1] 1, %o0\n"
);



void Shm_Lock(
  Shm_Locked_queue_Control *lq_cb
)
{
  uint32_t isr_level;
  uint32_t *lockptr = (uint32_t *) &lq_cb->lock;
  uint32_t lock_value;

  lock_value = SHM_LOCK_VALUE;
  rtems_interrupt_disable( isr_level );

    Shm_isrstat = isr_level;
    while ( lock_value ) {
      lock_value = LEON3_Atomic_Swap(lock_value, lockptr);
      /*
       *  If not available, then may want to delay to reduce load on lock.
       */
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
  uint32_t isr_level;

  lq_cb->lock = SHM_UNLOCK_VALUE;
  isr_level = Shm_isrstat;
  rtems_interrupt_enable( isr_level );
}

