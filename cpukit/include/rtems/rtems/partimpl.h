/**
 * @file
 *
 * @ingroup ClassicPartImpl
 *
 * @brief This header file provides interfaces used by the Partition Manager
 *   implementation.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_PARTIMPL_H
#define _RTEMS_RTEMS_PARTIMPL_H

#include <rtems/rtems/partdata.h>
#include <rtems/score/address.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/objectimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicPartImpl Partition Manager Implementation
 *
 * @ingroup RTEMSImplClassic
 *
 * @{
 */

/**
 *  @brief Checks whether is on a valid buffer boundary for the_partition.
 *
 *  This function returns TRUE if the_buffer is on a valid buffer
 *  boundary for the_partition, and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Partition_Is_buffer_on_boundary (
  void              *the_buffer,
  Partition_Control *the_partition
)
{
  intptr_t offset;

  offset = _Addresses_Subtract(
    the_buffer,
    the_partition->starting_address
  );

  return ((offset % the_partition->buffer_size) == 0);
}

/**
 *  @brief Checks whether the_buffer is a valid buffer from the_partition.
 *
 *  This function returns TRUE if the_buffer is a valid buffer from
 *  the_partition, otherwise FALSE is returned.
 */
RTEMS_INLINE_ROUTINE bool _Partition_Is_buffer_valid (
   Chain_Node        *the_buffer,
   Partition_Control *the_partition
)
{
  void *starting;
  void *ending;

  starting = the_partition->starting_address;
  ending   = _Addresses_Add_offset( starting, the_partition->length );

  return (
    _Addresses_Is_in_range( the_buffer, starting, ending ) &&
    _Partition_Is_buffer_on_boundary( the_buffer, the_partition )
  );
}

RTEMS_INLINE_ROUTINE bool _Partition_Is_buffer_size_aligned(
  uint32_t buffer_size
)
{
  return (buffer_size % CPU_SIZEOF_POINTER) == 0;
}

RTEMS_INLINE_ROUTINE bool _Partition_Is_buffer_area_aligned(
  const void *starting_address
)
{
  return (((uintptr_t) starting_address) % CPU_SIZEOF_POINTER) == 0;
}

/**
 *  @brief Allocates a partition control block from the
 *  inactive chain of free partition control blocks.
 *
 *  This function allocates a partition control block from
 *  the inactive chain of free partition control blocks.
 */
RTEMS_INLINE_ROUTINE Partition_Control *_Partition_Allocate ( void )
{
  return (Partition_Control *) _Objects_Allocate( &_Partition_Information );
}

RTEMS_INLINE_ROUTINE void _Partition_Initialize(
  Partition_Control *the_partition,
  void              *starting_address,
  uint32_t           length,
  uint32_t           buffer_size,
  rtems_attribute    attribute_set
)
{
  the_partition->starting_address      = starting_address;
  the_partition->length                = length;
  the_partition->buffer_size           = buffer_size;
  the_partition->attribute_set         = attribute_set;
  the_partition->number_of_used_blocks = 0;

  _Chain_Initialize(
    &the_partition->Memory,
    starting_address,
    length / buffer_size,
    buffer_size
  );

  _ISR_lock_Initialize( &the_partition->Lock, "Partition" );
}

RTEMS_INLINE_ROUTINE void _Partition_Destroy(
  Partition_Control *the_partition
)
{
  _ISR_lock_Destroy( &the_partition->Lock );
}

/**
 *  @brief Frees a partition control block to the
 *  inactive chain of free partition control blocks.
 *
 *  This routine frees a partition control block to the
 *  inactive chain of free partition control blocks.
 */
RTEMS_INLINE_ROUTINE void _Partition_Free (
   Partition_Control *the_partition
)
{
  _Objects_Free( &_Partition_Information, &the_partition->Object );
}

/**
 * @brief Calls _Objects_Get() using the ::_Partition_Information.
 *
 * @param id is the object identifier.
 * @param[out] lock_context is the lock context.
 *
 * @return See _Objects_Get().
 */
RTEMS_INLINE_ROUTINE Partition_Control *_Partition_Get(
  Objects_Id         id,
  ISR_lock_Context  *lock_context
)
{
  return (Partition_Control *) _Objects_Get(
    id,
    lock_context,
    &_Partition_Information
  );
}

/**
 * @brief Acquires the partition lock in an ISR disabled section.
 *
 * @param[in, out] the_partition is the partition control block.
 *
 * @param[in, out] lock_context is the lock context set up by _Partition_Get().
 */
RTEMS_INLINE_ROUTINE void _Partition_Acquire_critical(
  Partition_Control *the_partition,
  ISR_lock_Context  *lock_context
)
{
  _ISR_lock_Acquire( &the_partition->Lock, lock_context );
}

/**
 * @brief Releases the partition lock and restores the ISR level.
 *
 * @param[in, out] the_partition is the partition control block.
 *
 * @param[in, out] lock_context is the lock context set up by _Partition_Get().
 */
RTEMS_INLINE_ROUTINE void _Partition_Release(
  Partition_Control *the_partition,
  ISR_lock_Context  *lock_context
)
{
  _ISR_lock_Release_and_ISR_enable( &the_partition->Lock, lock_context );
}

/**@}*/

#ifdef __cplusplus
}
#endif

#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/partmp.h>
#endif

#endif
/* end of include file */
