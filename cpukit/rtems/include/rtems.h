/**
 * @file
 *
 * @defgroup ClassicRTEMS RTEMS Classic API
 *
 * @brief RTEMS Classic API
 *
 * the Public Interface to the RTEMS Classic API
 */

/* COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_H
#define _RTEMS_H

/**
 * @defgroup ClassicRTEMS RTEMS Classic API
 *
 * RTEMS Classic API definitions and modules.
 */
/**@{*/

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>

#include <rtems/config.h>
#include <rtems/debug.h>
#include <rtems/init.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/tasks.h>
#include <rtems/rtems/intr.h>
#include <rtems/rtems/barrier.h>
#include <rtems/rtems/cache.h>
#include <rtems/rtems/clock.h>
#include <rtems/extension.h>
#include <rtems/rtems/timer.h>
#include <rtems/rtems/sem.h>
#include <rtems/rtems/message.h>
#include <rtems/rtems/event.h>
#include <rtems/rtems/signal.h>
#include <rtems/rtems/event.h>
#include <rtems/rtems/object.h>
#include <rtems/rtems/part.h>
#include <rtems/rtems/region.h>
#include <rtems/rtems/dpmem.h>
#include <rtems/io.h>
#include <rtems/fatal.h>
#include <rtems/rtems/ratemon.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/mp.h>
#endif
#include <rtems/rtems/smp.h>

#include <rtems/rtems/support.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns the pointer to the RTEMS version string.
 */
const char *rtems_get_version_string(void);

/**
 * @brief Indicates whether this processor variant has hardware floating point
 * support.
 */
#define RTEMS_HAS_HARDWARE_FP CPU_HARDWARE_FP

/**********************************************************************
 *      CONSTANTS WHICH MAY BE USED IN OBJECT NAME TO ID SEARCHES
 **********************************************************************/

/**
 * @brief Indicates that a search is across all nodes.
 */
#define RTEMS_SEARCH_ALL_NODES   OBJECTS_SEARCH_ALL_NODES

/**
 * @brief Indicates that a search is across all nodes except the one the call
 * is made from.
 */
#define RTEMS_SEARCH_OTHER_NODES OBJECTS_SEARCH_OTHER_NODES

/**
 * @brief Indicates that the search is to be restricted to the local node.
 */
#define RTEMS_SEARCH_LOCAL_NODE  OBJECTS_SEARCH_LOCAL_NODE

/**
 * @brief Indicates that the caller wants to obtain the name of the currently
 * executing thread.
 *
 * This constant is only meaningful when obtaining the name of a task.
 */
#define RTEMS_WHO_AM_I           OBJECTS_WHO_AM_I

/**********************************************************************
 *        Parameters and return Id's for _Objects_Get_next
 **********************************************************************/

/**
 * @brief Lowest valid index value for the index portion of an object
 * identifier.
 */
#define RTEMS_OBJECT_ID_INITIAL_INDEX        OBJECTS_ID_INITIAL_INDEX

/**
 * @brief Maximum valid index value for the index portion of an object
 * identifier.
 */
#define RTEMS_OBJECT_ID_FINAL_INDEX          OBJECTS_ID_FINAL_INDEX

/**
 * @brief Returns the identifier of the object with the lowest valid index
 * value.
 *
 * The object is specified by the API @a _api, the object class @a _class and
 * the node @a _node where the object resides.
 */
#define RTEMS_OBJECT_ID_INITIAL(_api, _class, _node) \
   OBJECTS_ID_INITIAL(_api, _class, _node)

/**
 * @brief Maximum valid object identifier.
 */
#define RTEMS_OBJECT_ID_FINAL                OBJECTS_ID_FINAL

/**
 * @brief Minimum stack size which every thread must exceed.
 *
 * It is the minimum stack size recommended for use on this processor. This
 * value is selected by the RTEMS developers conservatively to minimize the
 * risk of blown stacks for most user applications. Using this constant when
 * specifying the task stack size, indicates that the stack size will be at
 * least RTEMS_MINIMUM_STACK_SIZE bytes in size. If the user configured minimum
 * stack size is larger than the recommended minimum, then it will be used.
 */
#define RTEMS_MINIMUM_STACK_SIZE  STACK_MINIMUM_SIZE

/**
 * @brief Specifies that the task should be created with the configured minimum
 * stack size.
 *
 * Using this constant when specifying the task stack size indicates that this
 * task is to be created with a stack size of the minimum stack size that was
 * configured by the application. If not explicitly configured by the
 * application, the default configured minimum stack size is the processor
 * dependent value RTEMS_MINIMUM_STACK_SIZE. Since this uses the configured
 * minimum stack size value, you may get a stack size that is smaller or larger
 * than the recommended minimum. This can be used to provide large stacks for
 * all tasks on complex applications or small stacks on applications that are
 * trying to conserve memory.
 */
#define RTEMS_CONFIGURED_MINIMUM_STACK_SIZE  0

/**
 * @brief Constant for indefinite wait.
 *
 * This is actually an illegal interval value.
 */
#define RTEMS_NO_TIMEOUT  WATCHDOG_NO_TIMEOUT

/**
 * @brief An MPCI must support packets of at least this size.
 */
#define RTEMS_MINIMUM_PACKET_SIZE  MP_PACKET_MINIMUM_PACKET_SIZE

/**
 * @brief Defines the count of @c uint32_t numbers in a packet which must be
 * converted to native format in a heterogeneous system.
 *
 * In packets longer than this value, some of the extra data may be a user
 * message buffer which is not automatically endian swapped.
 */
#define RTEMS_MINIMUN_HETERO_CONVERSION  MP_PACKET_MINIMUN_HETERO_CONVERSION

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
