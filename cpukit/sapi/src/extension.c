/*
 *  Extension Manager
 *
 *
 *  COPYRIGHT (c) 1989-2002.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/support.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/extension.h>

/*PAGE
 *
 *  _Extension_Manager_initialization
 *
 *  This routine initializes all extension manager related data structures.
 *
 *  Input parameters:
 *    maximum_extensions - number of extensions to initialize
 *
 *  Output parameters:  NONE
 */

void _Extension_Manager_initialization(
  unsigned32 maximum_extensions
)
{
  _Objects_Initialize_information(
    &_Extension_Information,
    OBJECTS_RTEMS_EXTENSIONS,
    FALSE,
    maximum_extensions,
    sizeof( Extension_Control ),
    FALSE,
    RTEMS_MAXIMUM_NAME_LENGTH,
    FALSE
  );
}
