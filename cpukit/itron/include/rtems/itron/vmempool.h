/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __RTEMS_ITRON_VARIABLE_MEMORYPOOL_h_
#define __RTEMS_ITRON_VARIABLE_MEMORYPOOL_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/itron/object.h>

/*
 *  The following defines the control block used to manage each variable
 *  memory pool.
 */

typedef struct {
  ITRON_Objects_Control   Object;
  unsigned32              XXX_more_stuff_goes_here;
}   ITRON_Variable_memory_pool_Control;

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */

ITRON_EXTERN Objects_Information  _ITRON_Variable_memory_pool_Information;

/*
 *  _ITRON_Variable_memory_pool_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _ITRON_Variable_memory_pool_Manager_initialization(
  unsigned32 maximum_variable_memory_pools
);

/*
 *  XXX insert private stuff here
 */

#include <rtems/itron/vmempool.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

