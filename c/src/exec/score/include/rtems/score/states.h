/*  states.h
 *
 *  This include file contains thread execution state information.
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

#ifndef __RTEMS_STATES_h
#define __RTEMS_STATES_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The following type defines the control block used to manage a
 *  thread's state.
 */

typedef unsigned32 States_Control;

/*
 *  The following constants define the individual states which may be
 *  be used to compose and manipulate a thread's state.
 */

#define STATES_ALL_SET                         0xffff /* all states */
#define STATES_READY                           0x0000 /* ready to run */
#define STATES_DORMANT                         0x0001 /* created not started */
#define STATES_SUSPENDED                       0x0002 /* waiting for resume */
#define STATES_TRANSIENT                       0x0004 /* thread in transition */
#define STATES_DELAYING                        0x0008 /* wait for timeout */
#define STATES_WAITING_FOR_TIME                0x0010 /* wait for TOD */
#define STATES_WAITING_FOR_BUFFER              0x0020 
#define STATES_WAITING_FOR_SEGMENT             0x0040
#define STATES_WAITING_FOR_MESSAGE             0x0080
#define STATES_WAITING_FOR_EVENT               0x0100
#define STATES_WAITING_FOR_SEMAPHORE           0x0200
#define STATES_WAITING_FOR_MUTEX               0x0400
#define STATES_WAITING_FOR_CONDITION_VARIABLE  0x0800
#define STATES_WAITING_FOR_JOIN_AT_EXIT        0x1000
#define STATES_WAITING_FOR_RPC_REPLY           0x2000
#define STATES_WAITING_FOR_PERIOD              0x4000
#define STATES_INTERRUPTIBLE_BY_SIGNAL         0x8000

#define STATES_LOCALLY_BLOCKED ( STATES_WAITING_FOR_BUFFER             | \
                                 STATES_WAITING_FOR_SEGMENT            | \
                                 STATES_WAITING_FOR_MESSAGE            | \
                                 STATES_WAITING_FOR_SEMAPHORE          | \
                                 STATES_WAITING_FOR_MUTEX              | \
                                 STATES_WAITING_FOR_CONDITION_VARIABLE | \
                                 STATES_WAITING_FOR_JOIN_AT_EXIT       )

#define STATES_WAITING_ON_THREAD_QUEUE \
                               ( STATES_LOCALLY_BLOCKED         | \
                                 STATES_WAITING_FOR_RPC_REPLY   )

#define STATES_BLOCKED         ( STATES_DELAYING                | \
                                 STATES_WAITING_FOR_TIME        | \
                                 STATES_WAITING_FOR_PERIOD      | \
                                 STATES_WAITING_FOR_EVENT       | \
                                 STATES_WAITING_ON_THREAD_QUEUE | \
                                 STATES_INTERRUPTIBLE_BY_SIGNAL )

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/states.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
