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

#ifndef __ITRON_FIXED_MEMORY_POOL_inl_
#define __ITRON_FIXED_MEMORY_POOL_inl_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  _ITRON_Fixed_memory_pool_Allocate
 */

#define _ITRON_Fixed_memory_pool_Allocate( _mpfid ) \
  (ITRON_Fixed_memory_pool_Control *)_ITRON_Objects_Allocate_by_index( \
    &_ITRON_Fixed_memory_pool_Information, \
    (_mpfid), \
    sizeof(ITRON_Fixed_memory_pool_Control) \
  )

/*
 *  _ITRON_Fixed_memory_pool_Clarify_allocation_id_error
 */

#define _ITRON_Fixed_memory_pool_Clarify_allocation_id_error( _id ) \
  _ITRON_Objects_Clarify_allocation_id_error( \
      &_ITRON_Fixed_memory_pool_Information, (_id) )

/*
 *  _ITRON_Fixed_memory_pool_Clarify_get_id_error
 */

#define _ITRON_Fixed_memory_pool_Clarify_get_id_error( _id ) \
 _ITRON_Objects_Clarify_get_id_error( \
    &_ITRON_Fixed_memory_pool_Information, (_id) )

/*
 *  _ITRON_Fixed_memory_pool_Free
 */

#define _ITRON_Fixed_memory_pool_Free( _the_fixed_memory_pool ) \
  _ITRON_Objects_Free( \
    &_ITRON_Fixed_memory_pool_Information, \
    &(_the_fixed_memory_pool)->Object \
  )

/*
 *  _ITRON_Fixed_memory_pool_Get
 */

#define _ITRON_Fixed_memory_pool_Get( _id, _location ) \
  (ITRON_Fixed_memory_pool_Control *) \
    _ITRON_Objects_Get( &_ITRON_Fixed_memory_pool_Information, \
        (_id), (_location) )

/*
 *  _ITRON_Fixed_memory_pool_Is_null
 */

#define _ITRON_Fixed_memory_pool_Is_null( _the_fixed_memory_pool ) \
  ( (_the_fixed_memory_pool) == NULL )

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

