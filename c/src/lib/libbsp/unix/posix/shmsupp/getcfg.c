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
 *  NOTES:  The MP interrupt used is the Runway bus' ability to directly
 *          address the control registers of up to four CPUs and cause
 *          interrupts on them.
 *
 *          The following table illustrates the configuration limitations:
 *
 *                                   BUS     MAX
 *                          MODE    ENDIAN  NODES
 *                        ========= ====== =======
 *                         POLLED    BIG    2+
 *                        INTERRUPT  BIG    2..4
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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define INTERRUPT 0        /* can be interrupt or polling */
#define POLLING   1

shm_config_table BSP_shm_cfgtbl;

int              semid;

void Shm_Cause_interrupt_unix(
  rtems_unsigned32 node
);

void Shm_Get_configuration(
  rtems_unsigned32   localnode,
  shm_config_table **shmcfg
)
{
    rtems_unsigned32 maximum_nodes;
    int          i;
    int          shmid;
    char        *shm_addr;
    key_t        shm_key;
    key_t        sem_key;
    struct sembuf  sb;
    int          status;

    shm_key = 0xa000;

    shmid = shmget(shm_key, 16 * KILOBYTE, IPC_CREAT | 0660);
    if ( shmid == -1 ) {
       perror( "shmget" );
       exit( 0 );
    }

    shm_addr = shmat(shmid, (char *)0, SHM_RND);
    if ( shm_addr == (void *)-1 ) {
       perror( "shmat" );
       exit( 0 );
    }

    sem_key = 0xa001;

    maximum_nodes = Shm_RTEMS_MP_Configuration->maximum_nodes;
    semid = semget(sem_key, maximum_nodes + 1, IPC_CREAT | 0660);
    if ( semid == -1 ) {
       perror( "semget" );
       exit( 0 );
    }

    if ( Shm_Is_master_node() ) {
      for ( i=0 ; i <= maximum_nodes ; i++ ) {
        sb.sem_op  = 1;
        sb.sem_num = i;
        sb.sem_flg = 0;
        status = semop( semid, &sb, 1 );
        if ( status == -1 ) {
          fprintf( stderr, "Sem init failed %d\n", errno ); fflush( stderr );
          exit( 0 );
        }
      }
    }

    BSP_shm_cfgtbl.base         = (vol_u32 *)shm_addr;
    BSP_shm_cfgtbl.length       = 16 * KILOBYTE;
    BSP_shm_cfgtbl.format       = SHM_BIG;

    BSP_shm_cfgtbl.cause_intr   = Shm_Cause_interrupt_unix;

#ifdef NEUTRAL_BIG
    BSP_shm_cfgtbl.convert      = NULL_CONVERT;
#else
    BSP_shm_cfgtbl.convert      = CPU_swap_u32;
#endif

#if ( POLLING == 1 )
    BSP_shm_cfgtbl.poll_intr    = POLLED_MODE;
    BSP_shm_cfgtbl.Intr.address = NO_INTERRUPT;
    BSP_shm_cfgtbl.Intr.value   = NO_INTERRUPT;
    BSP_shm_cfgtbl.Intr.length  = NO_INTERRUPT;
#else
    BSP_shm_cfgtbl.poll_intr    = INTR_MODE;
    BSP_shm_cfgtbl.Intr.address = 0;    /* process id? */
    BSP_shm_cfgtbl.Intr.value   = 0;    /* signal to send? */
    BSP_shm_cfgtbl.Intr.length  = LONG;
#endif

    *shmcfg = &BSP_shm_cfgtbl;
}
