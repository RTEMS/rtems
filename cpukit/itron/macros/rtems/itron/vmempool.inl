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

#ifndef __ITRON_VARIABLE_MEMORY_POOL_inl_
#define __ITRON_VARIABLE_MEMORY_POOL_inl_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  _ITRON_Variable_memory_pool_Allocate
 */

#define _ITRON_Variable_memory_pool_Allocate( _mplid ) \
  (ITRON_Variable_memory_pool_Control *)_ITRON_Objects_Allocate_by_index( \
    &_ITRON_Variable_memory_pool_Information, \
    (_mplid), \
    sizeof(ITRON_Variable_memory_pool_Control) \
  )

/*
 *  _ITRON_Variable_memory_pool_Clarify_allocation_id_error
 */

#define _ITRON_Variable_memory_pool_Clarify_allocation_id_error( _id ) \
  _ITRON_Objects_Clarify_allocation_id_error( \
      &_ITRON_Variable_memory_pool_Information, (_id) )

/*
 *  _ITRON_Variable_memory_pool_Clarify_get_id_error
 */

#define _ITRON_Variable_memory_pool_Clarify_get_id_error( _id ) \
 _ITRON_Objects_Clarify_get_id_error( &_ITRON_Variable_memory_pool_Information, (_id) )

/*
 *  _ITRON_Variable_memory_pool_Free
 */

#define _ITRON_Variable_memory_pool_Free( _the_variable_memory_pool ) \
  _ITRON_Objects_Free( \
    &_ITRON_Variable_memory_pool_Information, \
    &(_the_variable_memory_pool)->Object \
  )

/*PAGE
 *
 *  _ITRON_Variable_memory_pool_Get
 */

#define _ITRON_Variable_memory_pool_Get( _id, _location ) \
  (ITRON_Variable_memory_pool_Control *) _ITRON_Objects_Get( \
    &_ITRON_Variable_memory_pool_Information, \
    (_id), \
    (_location) \
  )

/*PAGE
 *
 *  _ITRON_Variable_memory_pool_Is_null
 */

#define _ITRON_Variable_memory_pool_Is_null( _the_variable_memory_pool ) \
  ( (_the_variable_memory_pool) == NULL )

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

