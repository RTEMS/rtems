/*  void Shm_get_config( localnode, &shmcfg )
 *
 *  This routine initializes, if necessary, and returns a pointer
 *  to the Shared Memory Configuration Table for the UNIX
 *  simulator.
 *
 *  INPUT PARAMETERS:
 *    localnode - local node number
 *    shmcfg    - address of pointer to SHM Config Table
 *
 *  OUTPUT PARAMETERS:
 *    *shmcfg   - pointer to SHM Config Table
 *
 *  NOTES:  This driver is capable of supporting a practically unlimited
 *          number of nodes.
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

#include <shm.h>
#include <bsp.h>

#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

shm_config_table BSP_shm_cfgtbl;

int              semid;

void Shm_Cause_interrupt_unix(
  rtems_unsigned32 node
);

void fix_semaphores( void )
{
    rtems_unsigned32 maximum_nodes;
    int          i;

    int          shmid;
    char        *shm_addr;
    key_t        shm_key;
    key_t        sem_key;
    int          status;
    int          shm_size;

    if (getenv("RTEMS_SHM_KEY"))
        shm_key = strtol(getenv("RTEMS_SHM_KEY"), 0, 0);
    else
#ifdef RTEMS_SHM_KEY
        shm_key = RTEMS_SHM_KEY;
#else
        shm_key = 0xa000;
#endif
 
    if (getenv("RTEMS_SHM_SIZE"))
        shm_size = strtol(getenv("RTEMS_SHM_SIZE"), 0, 0);
    else
#ifdef RTEMS_SHM_SIZE
        shm_size = RTEMS_SHM_SIZE;
#else
        shm_size = 64 * KILOBYTE;
#endif
 
    if (getenv("RTEMS_SHM_SEMAPHORE_KEY"))
        sem_key = strtol(getenv("RTEMS_SHM_SEMAPHORE_KEY"), 0, 0);
    else
#ifdef RTEMS_SHM_SEMAPHORE_KEY
        sem_key = RTEMS_SHM_SEMAPHORE_KEY;
#else
        sem_key = 0xa001;
#endif

    shmid = shmget(shm_key, shm_size, IPC_CREAT | 0660);
    if ( shmid == -1 ) {
       fix_syscall_errno(); /* in case of newlib */
       perror( "shmget" );
       rtems_fatal_error_occurred(RTEMS_UNSATISFIED);
    }

    shm_addr = shmat(shmid, (char *)0, SHM_RND);
    if ( shm_addr == (void *)-1 ) {
       fix_syscall_errno(); /* in case of newlib */
       perror( "shmat" );
       rtems_fatal_error_occurred(RTEMS_UNSATISFIED);
    }

    maximum_nodes = Shm_RTEMS_MP_Configuration->maximum_nodes;
    semid = semget(sem_key, maximum_nodes + 1, IPC_CREAT | 0660);
    if ( semid == -1 ) {
       fix_syscall_errno(); /* in case of newlib */
       perror( "semget" );
       rtems_fatal_error_occurred(RTEMS_UNSATISFIED);
    }

    if ( Shm_Is_master_node() ) {
      for ( i=0 ; i <= maximum_nodes ; i++ ) {

#if defined(solaris2)
        union semun {
          int val;
          struct semid_ds *buf;
          ushort *array;
        } help;
 
        help.val = 1;
        semctl( semid, i, SETVAL, help );
#endif
#if defined(hpux)
        semctl( semid, i, SETVAL, 1 );
#endif

        fix_syscall_errno(); /* in case of newlib */
        if ( status == -1 ) {
          fprintf( stderr, "Sem init failed %d\n", errno ); fflush( stderr );
          rtems_fatal_error_occurred(RTEMS_UNSATISFIED);
        }
      }
    }

    BSP_shm_cfgtbl.base         = (vol_u32 *)shm_addr;
    BSP_shm_cfgtbl.length       = shm_size;
}

void Shm_Get_configuration(
  rtems_unsigned32   localnode,
  shm_config_table **shmcfg
)
{
    fix_semaphores();

    BSP_shm_cfgtbl.format       = SHM_BIG;

    BSP_shm_cfgtbl.cause_intr   = Shm_Cause_interrupt_unix;

#ifdef NEUTRAL_BIG
    BSP_shm_cfgtbl.convert      = NULL_CONVERT;
#else
    BSP_shm_cfgtbl.convert      = CPU_swap_u32;
#endif

#ifdef INTERRUPT_EXTERNAL_MPCI
    BSP_shm_cfgtbl.poll_intr    = INTR_MODE;
    BSP_shm_cfgtbl.Intr.address = (vol_u32 *) getpid();    /* process id */
    BSP_shm_cfgtbl.Intr.value   = INTERRUPT_EXTERNAL_MPCI; /* signal to send */
    BSP_shm_cfgtbl.Intr.length  = LONG;
#else
    BSP_shm_cfgtbl.poll_intr    = POLLED_MODE;
    BSP_shm_cfgtbl.Intr.address = NO_INTERRUPT;
    BSP_shm_cfgtbl.Intr.value   = NO_INTERRUPT;
    BSP_shm_cfgtbl.Intr.length  = NO_INTERRUPT;
#endif

    *shmcfg = &BSP_shm_cfgtbl;
}
