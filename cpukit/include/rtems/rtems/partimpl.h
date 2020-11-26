/**
 * @file
 *
 * @ingroup RTEMSImplClassicPartition
 *
 * @brief This header file provides the implementation interfaces of
 *   the @ref RTEMSImplClassicPartition.
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
#include <rtems/score/objectimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSImplClassicPartition Partition Manager
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the Partition Manager implementation.
 *
 * @{
 */

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
