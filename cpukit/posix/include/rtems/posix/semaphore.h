/*  rtems/posix/semaphore.h
 *
 *  This include file contains all the private support information for
 *  POSIX Semaphores.
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
 
#ifndef __RTEMS_POSIX_SEMAPHORE_h
#define __RTEMS_POSIX_SEMAPHORE_h
 
#ifdef __cplusplus
extern "C" {
#endif

#include <semaphore.h>
#include <rtems/score/coresem.h>

#define SEM_FAILED (sem_t *) -1

/*
 *  Data Structure used to manage a POSIX semaphore
 */
 
typedef struct {
   Objects_Control         Object;
   int                     process_shared;
   boolean                 named;
   boolean                 linked;
   unsigned32              open_count;
   CORE_semaphore_Control  Semaphore;
}  POSIX_Semaphore_Control;

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
 
POSIX_EXTERN Objects_Information  _POSIX_Semaphore_Information;
 
/*
 *  _POSIX_Semaphore_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */
 
void _POSIX_Semaphore_Manager_initialization(
  unsigned32 maximum_semaphorees
);
 
/*
 *  _POSIX_Semaphore_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a semaphore control block from
 *  the inactive chain of free semaphore control blocks.
 */
 
RTEMS_INLINE_ROUTINE POSIX_Semaphore_Control *_POSIX_Semaphore_Allocate( void );
 
/*
 *  _POSIX_Semaphore_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a semaphore control block to the
 *  inactive chain of free semaphore control blocks.
 */
 
RTEMS_INLINE_ROUTINE void _POSIX_Semaphore_Free (
  POSIX_Semaphore_Control *the_semaphore
);
 
/*
 *  _POSIX_Semaphore_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps semaphore IDs to semaphore control blocks.
 *  If ID corresponds to a local semaphore, then it returns
 *  the_semaphore control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the semaphore ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_semaphore is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_semaphore is undefined.
 */
 
RTEMS_INLINE_ROUTINE POSIX_Semaphore_Control *_POSIX_Semaphore_Get (
  sem_t        *id,
  Objects_Locations *location
);
 
/*
 *  _POSIX_Semaphore_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_semaphore is NULL and FALSE otherwise.
 */
 
RTEMS_INLINE_ROUTINE boolean _POSIX_Semaphore_Is_null (
  POSIX_Semaphore_Control *the_semaphore
);

/*
 *  _POSIX_Semaphore_Create_support
 *
 *  DESCRIPTION:
 *
 *  This routine supports the sem_init and sem_open routines.
 */

int _POSIX_Semaphore_Create_support(
  const char                *name,
  int                        pshared,
  unsigned int               value,
  POSIX_Semaphore_Control  **the_sem
);

/*
 *  _POSIX_Semaphore_Delete
 *
 *  DESCRIPTION:
 *
 *  This routine supports the sem_close and sem_unlink routines.
 */

void _POSIX_Semaphore_Delete(
  POSIX_Semaphore_Control *the_semaphore
);

/*
 *  _POSIX_Semaphore_Wait_support
 *
 *  DESCRIPTION:
 *
 *  This routine supports the sem_wait, sem_trywait, and sem_timedwait
 *  services.
 */

int _POSIX_Semaphore_Wait_support(
  sem_t              *sem,
  boolean             blocking,
  Watchdog_Interval   timeout
);

/*
 *  _POSIX_Semaphore_Name_to_id
 *
 *  DESCRIPTION:
 *
 *  This routine performs name to id translation.
 */

int _POSIX_Semaphore_Name_to_id(
  const char          *name,
  sem_t          *id
);

#include <rtems/posix/semaphore.inl>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/posix/semaphoremp.h>
#endif

#ifdef __cplusplus
}
#endif
 
#endif
/*  end of include file */

