/*  tqdata.inl
 *
 *  This file contains the macro implementation of the inlined
 *  routines needed to support the Thread Queue Data.
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

#ifndef __THREAD_QUEUE_DATA_inl
#define __THREAD_QUEUE_DATA_inl

/*PAGE
 *
 *  _Thread_queue_Header_number
 *
 */

#define _Thread_queue_Header_number( _the_priority ) \
     ( (_the_priority) >> 6 )

/*PAGE
 *
 *  _Thread_queue_Is_reverse_search
 *
 */

#define _Thread_queue_Is_reverse_search( _the_priority ) \
     ( (_the_priority) & 0x20 )

/*PAGE
 *
 *  _Thread_queue_Get_number_waiting
 *
 */
 
#define _Thread_queue_Get_number_waiting( _the_thread_queue ) \
  ( (_the_thread_queue)->count )

/*PAGE
 *
 *  _Thread_queue_Enter_critical_section
 *
 */
 
#define _Thread_queue_Enter_critical_section( _the_thread_queue ) \
  do { \
    (_the_thread_queue)->sync = TRUE; \
    (_the_thread_queue)->sync_state = THREAD_QUEUE_NOTHING_HAPPENED; \
  } while ( 0 )

#endif
/* end of include file */
