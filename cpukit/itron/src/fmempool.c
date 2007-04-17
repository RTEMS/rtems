/*
 *  COPYRIGHT (c) 1989-2007.
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

#include <rtems/itron/fmempool.h>

/*
 *  _ITRON_Fixed_memory_pool_Manager_initialization
 *
 *  This routine initializes all fixed memory pool manager related
 *  data structures.
 *
 *  Input parameters:
 *    maximum_fixed_memory_pools - maximum configured fixed memory pools
 *
 *  Output parameters:  NONE
 */

void _ITRON_Fixed_memory_pool_Manager_initialization(
  uint32_t   maximum_fixed_memory_pools
)
{
  _Objects_Initialize_information(
    &_ITRON_Fixed_memory_pool_Information, /* object information table */
    OBJECTS_ITRON_API,                 /* object API */
    OBJECTS_ITRON_FIXED_MEMORY_POOLS,  /* object class */
    maximum_fixed_memory_pools,        /* maximum objects of this class */
    sizeof( ITRON_Fixed_memory_pool_Control ),
                                 /* size of this object's control block */
    FALSE,                       /* TRUE if names for this object are strings */
    ITRON_MAXIMUM_NAME_LENGTH    /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    FALSE,                       /* TRUE if this is a global object class */
    NULL                         /* Proxy extraction support callout */
#endif
  );

  /*
   *  Register the MP Process Packet routine.
   *
   *  NOTE: No MP Support YET in RTEMS ITRON implementation.
   */

}
