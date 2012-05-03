/*
 *  Dual Port Memory Manager
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/address.h>
#include <rtems/rtems/dpmem.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>

/*
 *  _Dual_ported_memory_Manager_initialization
 *
 *  This routine initializes all dual-ported memory manager related
 *  data structures.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 */

void _Dual_ported_memory_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_Dual_ported_memory_Information, /* object information table */
    OBJECTS_CLASSIC_API,              /* object API */
    OBJECTS_RTEMS_PORTS,              /* object class */
    Configuration_RTEMS_API.maximum_ports,
                                  /* maximum objects of this class */
    sizeof( Dual_ported_memory_Control ),
                                  /* size of this object's control block */
    false,                        /* true if names of this object are strings */
    RTEMS_MAXIMUM_NAME_LENGTH     /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    false,                        /* true if this is a global object class */
    NULL                          /* Proxy extraction support callout */
#endif
  );
}
