/**
 * @file rtems/posix/key.h
 *
 * This include file contains all the private support information for
 * POSIX key.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_KEY_H
#define _RTEMS_POSIX_KEY_H

#include <rtems/score/object.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This is the data Structure used to manage a POSIX key.
 *
 *  @note The Values is a table indexed by the index portion of the
 *        ID of the currently executing thread.
 */
typedef struct {
   /** This field is the Object control structure. */
   Objects_Control     Object;
   /** This field points to the optional destructor method. */
   void              (*destructor)( void * );
   /** This field points to the values per thread. */
   void              **Values[ OBJECTS_APIS_LAST + 1 ];
}  POSIX_Keys_Control;

/**
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
POSIX_EXTERN Objects_Information  _POSIX_Keys_Information;

/**
 *  @brief _POSIX_Keys_Manager_initialization
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _POSIX_Key_Manager_initialization(void);

/**
 *  @brief _POSIX_Keys_Run_destructors
 *
 *  This function executes all the destructors associated with the thread's
 *  keys.  This function will execute until all values have been set to NULL.
 *
 *  @param[in] thread is the thread whose keys should have all their
 *             destructors run.
 *
 *  @note This is the routine executed when a thread exits to
 *        run through all the keys and do the destructor action.
 */
void _POSIX_Keys_Run_destructors(
  Thread_Control *thread
);

/**
 *  @brief Free Key Memory
 *
 *  This memory frees the key table memory associated with @a the_key.
 *
 *  @param[in] the_key is the POSIX key to free the table memory of.
 */
void _POSIX_Keys_Free_memory(
  POSIX_Keys_Control *the_key
);

/**
 *  @brief _POSIX_Keys_Free
 *
 *  This routine frees a keys control block to the
 *  inactive chain of free keys control blocks.
 *
 *  @param[in] the_key is the POSIX key to free.
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
