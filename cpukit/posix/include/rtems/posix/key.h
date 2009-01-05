/**
 * @file rtems/posix/key.h
 */

/*  rtems/posix/key.h
 *
 *  This include file contains all the private support information for
 *  POSIX key.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_POSIX_KEY_H
#define _RTEMS_POSIX_KEY_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Data Structure used to manage a POSIX key
 *
 *  NOTE:  The Values is a table indexed by the index portion of the
 *         ID of the currently executing thread.
 */

typedef struct {
   Objects_Control     Object;
   bool                is_active;
   void              (*destructor)( void * );
   void              **Values[ OBJECTS_APIS_LAST + 1 ];
}  POSIX_Keys_Control;

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */

POSIX_EXTERN Objects_Information  _POSIX_Keys_Information;

/*
 *  _POSIX_Keys_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _POSIX_Key_Manager_initialization(void);

/*
 *  _POSIX_Keys_Run_destructors
 *
 *  DESCRIPTION:
 *
 *  This function executes all the destructors associated with the thread's
 *  keys.  This function will execute until all values have been set to NULL.
 *
 *  NOTE:  This is the routine executed when a thread exits to
 *         run through all the keys and do the destructor action.
 */

void _POSIX_Keys_Run_destructors(
  Thread_Control *thread
);

/*
 *  _POSIX_Keys_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a keys control block to the
 *  inactive chain of free keys control blocks.
 */

RTEMS_INLINE_ROUTINE void _POSIX_Keys_Free (
  POSIX_Keys_Control *the_key
);

#include <rtems/posix/key.inl>

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
