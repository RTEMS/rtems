/*  tqdata.inl
 *
 *  This file contains the static inline implementation of the inlined
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

STATIC INLINE unsigned32 _Thread_queue_Header_number (
  Priority_Control the_priority
)
{
  return ( the_priority >> 6 );
}

/*PAGE
 *
 *  _Thread_queue_Is_reverse_search
 *
 */

STATIC INLINE boolean _Thread_queue_Is_reverse_search (
  Priority_Control the_priority
)
{
  return ( the_priority & 0x20 );
}

/*PAGE
 *
 *  _Thread_queue_Get_number_waiting
 *
 */
 
STATIC INLINE unsigned32 _Thread_queue_Get_number_waiting (
  Thread_queue_Control *the_thread_queue
)
{
  return ( the_thread_queue->count );
}

/*PAGE
 *
 *  _Thread_queue_Enter_critical_section
 *
 */
 
STATIC INLINE void _Thread_queue_Enter_critical_section (
  Thread_queue_Control *the_thread_queue
)
{
  the_thread_queue->sync_state = THREAD_QUEUE_NOTHING_HAPPENED;
}

#endif
/* end of include file */
