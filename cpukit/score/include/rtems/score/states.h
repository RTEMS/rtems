/*  states.h
 *
 *  This include file contains thread execution state information.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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

#define STATES_ALL_SET                         0xfffff /* all states */
#define STATES_READY                           0x00000 /* ready to run */
#define STATES_DORMANT                         0x00001 /* created not started */
#define STATES_SUSPENDED                       0x00002 /* waiting for resume */
#define STATES_TRANSIENT                       0x00004 /* in transition */
#define STATES_DELAYING                        0x00008 /* wait for timeout */
#define STATES_WAITING_FOR_TIME                0x00010 /* wait for TOD */
#define STATES_WAITING_FOR_BUFFER              0x00020 
#define STATES_WAITING_FOR_SEGMENT             0x00040
#define STATES_WAITING_FOR_MESSAGE             0x00080
#define STATES_WAITING_FOR_EVENT               0x00100
#define STATES_WAITING_FOR_SEMAPHORE           0x00200
#define STATES_WAITING_FOR_MUTEX               0x00400
#define STATES_WAITING_FOR_CONDITION_VARIABLE  0x00800
#define STATES_WAITING_FOR_JOIN_AT_EXIT        0x01000
#define STATES_WAITING_FOR_RPC_REPLY           0x02000
#define STATES_WAITING_FOR_PERIOD              0x04000
#define STATES_WAITING_FOR_SIGNAL              0x08000
#define STATES_INTERRUPTIBLE_BY_SIGNAL         0x10000

#define STATES_LOCALLY_BLOCKED ( STATES_WAITING_FOR_BUFFER             | \
                                 STATES_WAITING_FOR_SEGMENT            | \
                                 STATES_WAITING_FOR_MESSAGE            | \
                                 STATES_WAITING_FOR_SEMAPHORE          | \
                                 STATES_WAITING_FOR_MUTEX              | \
                                 STATES_WAITING_FOR_CONDITION_VARIABLE | \
                                 STATES_WAITING_FOR_JOIN_AT_EXIT       | \
                                 STATES_WAITING_FOR_SIGNAL             )

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
