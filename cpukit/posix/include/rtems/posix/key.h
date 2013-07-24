/**
 * @file
 * 
 * @brief POSIX Key Private Support
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

#include <rtems/score/objectimpl.h>

/**
 * @defgroup POSIX_KEY POSIX Key
 *
 * @ingroup POSIXAPI
 * 
 */
/**@{**/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This is the data Structure used to manage a POSIX key.
 *
 * NOTE: The Values is a table indexed by the index portion of the
 *       ID of the currently executing thread.
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
 * The following defines the information control block used to manage
 * this class of objects.
 */
POSIX_EXTERN Objects_Information  _POSIX_Keys_Information;

/**
 * @brief POSIX keys manager initialization.
 *
 * This routine performs the initialization necessary for this manager.
 */
void _POSIX_Key_Manager_initialization(void);

/**
 * @brief Create thread-specific data POSIX key.
 *
 * This function executes all the destructors associated with the thread's
 * keys.  This function will execute until all values have been set to NULL.
 *
 * @param[in] thread is a pointer to the thread whose keys should have 
 *            all their destructors run.
 *
 * NOTE: This is the routine executed when a thread exits to
 *       run through all the keys and do the destructor action.
 */
void _POSIX_Keys_Run_destructors(
  Thread_Control *thread
);

/**
 * @brief Free a POSIX key table memory.
 *
 * This memory frees the key table memory associated with @a the_key.
 *
 * @param[in] the_key is a pointer to the POSIX key to free
 * the table memory of.
 */
void _POSIX_Keys_Free_memory(
  POSIX_Keys_Control *the_key
);

/**
 * @brief Free a POSIX keys control block.
 *
 * This routine frees a keys control block to the
 * inactive chain of free keys control blocks.
 *
 * @param[in] the_key is a pointer to the POSIX key to free.
 */
RTEMS_INLINE_ROUTINE void _POSIX_Keys_Free (
  POSIX_Keys_Control *the_key
);

#include <rtems/posix/key.inl>

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
