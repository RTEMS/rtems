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

void _ITRON_Variable_memory_pool_Manager_initialization(
  unsigned32 maximum_variable_memory_pools
) 
{
  _Objects_Initialize_information(
    &_ITRON_Variable_memory_pool_Information, /* object information table */
    OBJECTS_ITRON_VARIABLE_MEMORY_POOLS,      /* object class */
    FALSE,                             /* TRUE if this is a global */
                                       /*   object class */
    maximum_variable_memory_pools,     /* maximum objects of this class */
    sizeof( ITRON_Variable_memory_pool_Control ),
                                       /* size of this object's control block */
    FALSE,                             /* TRUE if names for this object */
                                       /*   are strings */
    ITRON_MAXIMUM_NAME_LENGTH,         /* maximum length of each object's */
                                       /*   name */
    FALSE                              /* TRUE if this class is threads */
  ); 
    
  /*
   *  Register the MP Process Packet routine.
   *
   *  NOTE: No MP Support YET in RTEMS ITRON implementation.
   */
 
}   

/*
 *  cre_mpl - Create Variable-Size Memorypool
 */

ER cre_mpl(
  ID      mplid,
  T_CMPL *pk_cmpl
)
{
  return E_OK;
}

/*
 *  del_mpl - Delete Variable-Size Memorypool
 */

ER del_mpl(
  ID mplid
)
{
  return E_OK;
}

/*
 *  get_blk - Get Variable-Size Memory Block
 */

ER get_blk(
  VP  *p_blk,
  ID   mplid,
  INT  blksz
)
{
  return E_OK;
}

/*
 *  pget_blk - Poll and Get Variable-Size Memory Block
 */

ER pget_blk(
  VP  *p_blk,
  ID   mplid,
  INT  blksz
)
{
  return E_OK;
}

/*
 *  tget_blk - Get Variable-Size Memory Block with Timeout
 */

ER tget_blk(
  VP  *p_blk,
  ID   mplid,
  INT  blksz,
  TMO  tmout
)
{
  return E_OK;
}

/*
 *  rel_blk - Release Variable-Size Memory Block
 */

ER rel_blk(
  ID  mplid,
  VP  blk
)
{
  return E_OK;
}

/*
 *  ref_mpl - Reference Variable-Size Memorypool Status
 */

ER ref_mpl(
  T_RMPL *pk_rmpl,
  ID      mplid
)
{
  return E_OK;
}

