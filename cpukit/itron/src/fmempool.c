/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <itron.h>

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
  unsigned32 maximum_fixed_memory_pools
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

/*
 *  cre_mpf - Create Fixed-Size Memorypool
 */

ER cre_mpf(
  ID      mpfid,
  T_CMPF *pk_cmpf
)
{
  return E_OK;
}

/*
 *  del_mpf - Delete Fixed-Size Memorypool
 */

ER del_mpf(
  ID mpfid
)
{
  return E_OK;
}

/*
 *  get_blf - Get Fixed-Size Memory Block
 */

ER get_blf(
  VP *p_blf,
  ID  mpfid
)
{
  return E_OK;
}

/*
 *  pget_blf - Poll and Get Fixed-Size Memory Block
 */

ER pget_blf(
  VP *p_blf,
  ID  mpfid
)
{
  return E_OK;
}

/*
 *  tget_blf - Get Fixed-Size Memory Block with Timeout
 */

ER tget_blf(
  VP *p_blf,
  ID  mpfid,
  TMO tmout
)
{
  return E_OK;
}

/*
 *  rel_blf - Release Fixed-Size Memory Block
 */

ER rel_blf(
  ID mpfid,
  VP blf
)
{
  return E_OK;
}

/*
 *  ref_mpf - Reference Fixed-Size Memorypool Status
 */

ER ref_mpf(
  T_RMPF *pk_rmpf,
  ID      mpfid
)
{
  return E_OK;
}

