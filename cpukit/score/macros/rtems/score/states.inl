/*  states.inl
 *
 *  This file contains the macro implementation of the inlined
 *  routines associated with thread state information.
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

#ifndef __STATES_inl
#define __STATES_inl

/*PAGE
 *
 *  _States_Set
 *
 */

#define _States_Set( _states_to_set, _current_state ) \
   ((_current_state) | (_states_to_set))

/*PAGE
 *
 *  _States_Clear
 *
 */

#define _States_Clear( _states_to_clear, _current_state ) \
   ((_current_state) & ~(_states_to_clear))

/*PAGE
 *
 *  _States_Is_ready
 *
 */

#define _States_Is_ready( _the_states ) \
   ( (_the_states) == STATES_READY )

/*PAGE
 *
 *  _States_Is_only_dormant
 *
 */

#define _States_Is_only_dormant( _the_states ) \
   ( (_the_states) == STATES_DORMANT )

/*PAGE
 *
 *  _States_Is_dormant
 *
 */

#define _States_Is_dormant( _the_states ) \
   ( (_the_states) & STATES_DORMANT )

/*PAGE
 *
 *  _States_Is_suspended
 *
 */

#define _States_Is_suspended( _the_states ) \
   ( (_the_states) & STATES_SUSPENDED )

/*PAGE
 *
 *  _States_Is_Transient
 *
 */

#define _States_Is_transient( _the_states ) \
   ( (_the_states) & STATES_TRANSIENT )

/*PAGE
 *
 *  _States_Is_delaying
 *
 */

#define _States_Is_delaying( _the_states ) \
   ( (_the_states) & STATES_DELAYING )

/*PAGE
 *
 *  _States_Is_waiting_for_buffer
 *
 */

#define _States_Is_waiting_for_buffer( _the_states ) \
   ( (_the_states) & STATES_WAITING_FOR_BUFFER )

/*PAGE
 *
 *  _States_Is_waiting_for_segment
 *
 */

#define _States_Is_waiting_for_segment( _the_states ) \
   ( (_the_states) & STATES_WAITING_FOR_SEGMENT )

/*PAGE
 *
 *  _States_Is_waiting_for_message
 *
 */

#define _States_Is_waiting_for_message( _the_states ) \
   ( (_the_states) & STATES_WAITING_FOR_MESSAGE )

/*PAGE
 *
 *  _States_Is_waiting_for_event
 *
 */

#define _States_Is_waiting_for_event( _the_states ) \
   ( (_the_states) & STATES_WAITING_FOR_EVENT )

/*PAGE
 *
 *  _States_Is_waiting_for_mutex
 *
 */
 
#define _States_Is_waiting_for_mutex( _the_states ) \
   ( (_the_states) & STATES_WAITING_FOR_MUTEX )

/*PAGE
 *
 *  _States_Is_waiting_for_semaphore
 *
 */

#define _States_Is_waiting_for_semaphore( _the_states ) \
   ( (_the_states) & STATES_WAITING_FOR_SEMAPHORE )

/*PAGE
 *
 *  _States_Is_waiting_for_time
 *
 */

#define _States_Is_waiting_for_time( _the_states ) \
   ( (_the_states) & STATES_WAITING_FOR_TIME )

/*PAGE
 *
 *  _States_Is_waiting_for_rpc_reply
 *
 */

#define _States_Is_waiting_for_rpc_reply( _the_states ) \
   ( (_the_states) & STATES_WAITING_FOR_RPC_REPLY )

/*PAGE
 *
 *  _States_Is_waiting_for_period
 *
 */

#define _States_Is_waiting_for_period( _the_states ) \
   ( (_the_states) & STATES_WAITING_FOR_PERIOD )

/*PAGE
 *
 *  _States_Is_locally_blocked
 *
 */

#define _States_Is_locally_blocked( _the_states ) \
   ( (_the_states) & STATES_LOCALLY_BLOCKED )

/*PAGE
 *
 *  _States_Is_waiting_on_thread_queue
 *
 */

#define _States_Is_waiting_on_thread_queue( _the_states ) \
   ( (_the_states) & STATES_WAITING_ON_THREAD_QUEUE )

/*PAGE
 *
 *  _States_Is_blocked
 *
 */

#define _States_Is_blocked( _the_states ) \
   ( (_the_states) & STATES_BLOCKED )

/*PAGE
 *
 *  _States_Are_set
 *
 */

#define _States_Are_set( _the_states, _mask ) \
   ( ((_the_states) & (_mask)) != STATES_READY )

#endif
/* end of include file */
