/*  Shared Memory Lock Routines
 *
 *  This shared memory locked queue support routine need to be
 *  able to lock the specified locked queue.  Interrupts are
 *  disabled while the queue is locked to prevent preemption
 *  and deadlock when two tasks poll for the same lock.
 *  previous level.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems.h>

#include <bsp.h>
#include <shm.h>

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

extern int      semid;

/*
 *  Shm_Initialize_lock
 *
 *  Initialize the lock for the specified locked queue.
 */

void Shm_Initialize_lock(
  Shm_Locked_queue_Control *lq_cb
)
{
  lq_cb->lock = 
    ( lq_cb - Shm_Locked_queues ) / sizeof( Shm_Locked_queue_Control );
}

/*  Shm_Lock( &lq_cb )
 *
 *  This shared memory locked queue support routine locks the
 *  specified locked queue.  It disables interrupts to prevent
 *  a deadlock condition.
 */

void Shm_Lock(
  Shm_Locked_queue_Control *lq_cb
)
{
    rtems_unsigned32   isr_level;
    struct sembuf      sb;
    int                status;

    isr_level = 0;

    sb.sem_num = lq_cb->lock;
    sb.sem_op  = -1;
    sb.sem_flg = 0;
    rtems_interrupt_disable( isr_level );

    Shm_isrstat = isr_level;

    while (1) {
      status = semop(semid, &sb, 1);
      if ( status == 0 )
        break;
      if ( status == -1 && errno == EINTR )
        continue;

      fprintf( stderr, "shm lock (%d %d)\n", status, errno );
      exit( 0 );
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
    rtems_unsigned32   isr_level;
    struct sembuf      sb;
    int                status;

    isr_level = 0;

    sb.sem_num = lq_cb->lock;
    sb.sem_op  = 1;
    sb.sem_flg = 0;

    while (1) {
      status = semop(semid, &sb, 1);
      if ( status == 0 )
        break;
      if ( status == -1 && errno == EINTR )
        continue;

      fprintf( stderr, "shm lock (%d %d)\n", status, errno ); fflush( stderr );
      exit( 0 );
    }

    isr_level = Shm_isrstat;
    rtems_interrupt_enable( isr_level );
}
