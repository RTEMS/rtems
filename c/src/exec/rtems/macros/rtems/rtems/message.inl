/*  message.inl
 *
 *  This include file contains the macro implementation of all
 *  inlined routines in the Message Manager.
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

#ifndef __MESSAGE_QUEUE_inl
#define __MESSAGE_QUEUE_inl

/*PAGE
 *
 *  _Message_queue_Is_null
 *
 */

#define _Message_queue_Is_null( _the_message_queue ) \
   ( (_the_message_queue) == NULL  )

/*PAGE
 *
 *  _Message_queue_Free
 *
 */

#define _Message_queue_Free( _the_message_queue ) \
  _Objects_Free( &_Message_queue_Information, &(_the_message_queue)->Object )

/*PAGE
 *
 *  _Message_queue_Get
 *
 */

#define _Message_queue_Get( _id, _location ) \
  (Message_queue_Control *) \
     _Objects_Get( &_Message_queue_Information, (_id), (_location) )

#endif
/* end of include file */
