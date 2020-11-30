/**
 * @file
 *
 * @ingroup RTEMSImplClassicPartition
 *
 * @brief This header file provides data structures used by the implementation
 *   and the @ref RTEMSImplApplConfig to define ::_Partition_Information.
 */

/* COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_PARTDATA_H
#define _RTEMS_RTEMS_PARTDATA_H

#include <rtems/rtems/part.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/objectdata.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSImplClassicPartition
 *
 * @{
 */

/**
 * @brief The Partition Control Block (PTCB) represents a partition.
 */
typedef struct {
  /**
   * @brief This member turns the PTCB into an object.
   */
  Objects_Control Object;

  /**
   * @brief This lock protects the chain of unallocated buffers and the number
   *   of allocated buffers.
   */
  ISR_LOCK_MEMBER( Lock )

  /**
   * @brief This member contains the physical starting address of the buffer
   *   area.
   */
  void *starting_address;

  /**
   * @brief This member contains the size of the buffer area in bytes.
   */
  uintptr_t length;

  /**
   * @brief This member contains the size of each buffer in bytes.
  */
  size_t buffer_size;

  /**
   * @brief This member contains the attribute set provided at creation time.
   */
  rtems_attribute attribute_set;

  /**
   * @brief This member contains the count of allocated buffers.
   */
  uintptr_t number_of_used_blocks;

  /**
   * @brief This chain is used to manage unallocated buffers.
   */
  Chain_Control Memory;
} Partition_Control;

/**
 * @brief The Partition Manager objects information is used to manage the
 *   objects of this class.
 *
 * If #CONFIGURE_MAXIMUM_PARTITIONS is greater than zero, then the object
 * information is defined by PARTITION_INFORMATION_DEFINE(), otherwise it is
 * defined by OBJECTS_INFORMATION_DEFINE_ZERO().
 */
extern Objects_Information _Partition_Information;

#if defined(RTEMS_MULTIPROCESSING)
/**
 * @brief Sends the extract proxy request.
 *
 * This routine is invoked when a task is deleted and it has a proxy which must
 * be removed from a thread queue and the remote node must be informed of this.
 *
 * @param[in, out] the_thread is the thread proxy.
 * @param id is the partition identifier.
 */
void _Partition_MP_Send_extract_proxy (
  Thread_Control *the_thread,
  Objects_Id      id
);
#endif

/**
 * @brief Defines the Partition Manager objects information.
 *
 * This macro should only be used by <rtems/confdefs/objectsclassic.h>.
 *
 * @param _max is the configured object maximum (the #OBJECTS_UNLIMITED_OBJECTS
 *   flag may be set).
 */
#define PARTITION_INFORMATION_DEFINE( _max ) \
  OBJECTS_INFORMATION_DEFINE( \
    _Partition, \
    OBJECTS_CLASSIC_API, \
    OBJECTS_RTEMS_PARTITIONS, \
    Partition_Control, \
    _max, \
    OBJECTS_NO_STRING_NAME, \
    _Partition_MP_Send_extract_proxy \
  )

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
