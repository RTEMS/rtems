/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/itron.h>
#include <rtems/config.h>
#include <rtems/itron/vmempool.h>

/*
 *  _ITRON_Variable_memory_pool_Manager_initialization
 *
 *  This routine initializes all variable memory pool manager related
 *  data structures.
 *
 *  Input parameters:
 *    maximum_variable_memory_pools - maximum configured variable memory pools
 *
 *  Output parameters:  NONE
 */

void _ITRON_Variable_memory_pool_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_ITRON_Variable_memory_pool_Information, /* object information table */
    OBJECTS_ITRON_API,                        /* object API */
    OBJECTS_ITRON_VARIABLE_MEMORY_POOLS,      /* object class */
    Configuration_ITRON_API.maximum_memory_pools,
                                /* maximum objects of this class */
    sizeof( ITRON_Variable_memory_pool_Control ),
                                /* size of this object's control block */
    FALSE,                      /* TRUE if names for this object are strings */
    ITRON_MAXIMUM_NAME_LENGTH   /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    FALSE,                      /* TRUE if this is a global object class */
    NULL                        /* Proxy extraction support callout */
#endif
  );

  /*
   *  Register the MP Process Packet routine.
   *
   *  NOTE: No MP Support YET in RTEMS ITRON implementation.
   */

}
