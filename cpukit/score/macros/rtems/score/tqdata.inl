/*  tqdata.inl
 *
 *  This file contains the macro implementation of the inlined
 *  routines needed to support the Thread Queue Data.
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

#ifndef __THREAD_QUEUE_DATA_inl
#define __THREAD_QUEUE_DATA_inl

/*PAGE
 *
 *  _Thread_queue_Header_number
 *
 */

#define _Thread_queue_Header_number( _the_priority ) \
    ((_the_priority) / TASK_QUEUE_DATA_PRIORITIES_PER_HEADER)

/*PAGE
 *
 *  _Thread_queue_Is_reverse_search
 *
 */

#define _Thread_queue_Is_reverse_search( _the_priority ) \
     ( (_the_priority) & TASK_QUEUE_DATA_REVERSE_SEARCH_MASK )

/*PAGE
 *
 *  _Thread_queue_Enter_critical_section
 *
 */
 
#define _Thread_queue_Enter_critical_section( _the_thread_queue ) \
  do { \
    (_the_thread_queue)->sync_state = THREAD_QUEUE_NOTHING_HAPPENED; \
  } while ( 0 )

#endif
/* end of include file */
