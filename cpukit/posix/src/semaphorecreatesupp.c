/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <limits.h>
#include <string.h>	/* strlen */

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/semaphore.h>
#include <rtems/posix/time.h>
#include <rtems/seterr.h>

/*
 *  _POSIX_Semaphore_Create_support
 *
 *  This routine does the actual creation and initialization of
 *  a poxix semaphore.  It is a support routine for sem_init and
 *  sem_open.
 */
int _POSIX_Semaphore_Create_support(
  const char                *name_arg,
  size_t                     name_len,
  int                        pshared,
  unsigned int               value,
  POSIX_Semaphore_Control  **the_sem
)
{
  POSIX_Semaphore_Control   *the_semaphore;
  CORE_semaphore_Attributes *the_sem_attr;
  char                      *name;

  /* Sharing semaphores among processes is not currently supported */
  if (pshared != 0)
    rtems_set_errno_and_return_minus_one( ENOSYS );

  _Thread_Disable_dispatch();

  the_semaphore = _POSIX_Semaphore_Allocate();
  if ( !the_semaphore ) {
    _Thread_Enable_dispatch();
    rtems_set_errno_and_return_minus_one( ENOSPC );
  }

  /*
   * Make a copy of the user's string for name just in case it was
   * dynamically constructed.
   */
  if ( name_arg != NULL ) {
    name = _Workspace_String_duplicate( name_arg, name_len );
    if ( !name ) {
      _POSIX_Semaphore_Free( the_semaphore );
      _Thread_Enable_dispatch();
      rtems_set_errno_and_return_minus_one( ENOMEM );
    }
  } else {
    name = NULL;
  }

  the_semaphore->process_shared  = pshared;

  if ( name ) {
    the_semaphore->named = true;
    the_semaphore->open_count = 1;
    the_semaphore->linked = true;
  } else {
    the_semaphore->named = false;
    the_semaphore->open_count = 0;
    the_semaphore->linked = false;
  }

  the_sem_attr = &the_semaphore->Semaphore.Attributes;

  /*
   *  POSIX does not appear to specify what the discipline for
   *  blocking tasks on this semaphore should be.  It could somehow
   *  be derived from the current scheduling policy.  One
   *  thing is certain, no matter what we decide, it won't be
   *  the same as  all other POSIX implementations. :)
   */
  the_sem_attr->discipline = CORE_SEMAPHORE_DISCIPLINES_FIFO;

  /*
   *  This effectively disables limit checking.
   */
  the_sem_attr->maximum_count = 0xFFFFFFFF;

  _CORE_semaphore_Initialize( &the_semaphore->Semaphore, the_sem_attr, value );

  /*
   *  Make the semaphore available for use.
   */
  _Objects_Open_string(
    &_POSIX_Semaphore_Information,
    &the_semaphore->Object,
    name
  );

  *the_sem = the_semaphore;

  _Thread_Enable_dispatch();
  return 0;
}
