/**
 * @file  rtems.h
 *
 *  This include file contains information about RTEMS executive that
 *  is required by the application and is CPU independent.  It includes
 *  two (2) CPU dependent files to tailor its data structures for a
 *  particular processor.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_H
#define _RTEMS_H

/**
 *  @defgroup ClassicRTEMS Classic API RTEMS Header
 *
 *  This encapsulates functionality which XXX
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#if (!defined(__RTEMS_VIOLATE_KERNEL_VISIBILITY__)) && \
    (!defined(__RTEMS_INSIDE__))
/**
 *  @brief Compiling RTEMS Application Macro
 *
 *  Unless told otherwise, the RTEMS include files will hide some stuff
 *  from normal application code.  Defining this crosses a boundary which
 *  is undesirable since it means your application is using RTEMS features
 *  which are not included in the formally defined and supported API.
 *  Define this at your own risk.
 */

#define __RTEMS_APPLICATION__
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>

#include <rtems/config.h>
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

#include <rtems/rtems/support.h>
#include <rtems/score/sysstate.h>

/**
 *  This constant indicates whether this processor variant has
 *  hardware floating point support.
 */
#define RTEMS_HAS_HARDWARE_FP CPU_HARDWARE_FP

/*
 *  The following define the constants which may be used in name searches.
 */

/**
 * This constant indicates that the search is across all nodes.
 */
#define RTEMS_SEARCH_ALL_NODES   OBJECTS_SEARCH_ALL_NODES

/**
 *  This constant indicates that the search is across all nodes
 *  except the one the call is made from.
 */
#define RTEMS_SEARCH_OTHER_NODES OBJECTS_SEARCH_OTHER_NODES

/**
 *  This constant indicates that the search is to be restricted
 *  to the local node.
 */
#define RTEMS_SEARCH_LOCAL_NODE  OBJECTS_SEARCH_LOCAL_NODE

/**
 *  This constant indicates that the caller wants to obtain the
 *  name of the currently executing thread.
 *
 *  @note This constant is only meaningful when obtaining the name
 *        of a task.
 */
#define RTEMS_WHO_AM_I           OBJECTS_WHO_AM_I

/*
 * Parameters and return id's for _Objects_Get_next
 */

/**
 *  This constant is the lowest valid valid for the index portion
 *  of an object Id.
 */
#define RTEMS_OBJECT_ID_INITIAL_INDEX        OBJECTS_ID_INITIAL_INDEX

/**
 *  This constant is the maximum valid valid for the index portion
 *  of an object Id.
 */
#define RTEMS_OBJECT_ID_FINAL_INDEX          OBJECTS_ID_FINAL_INDEX

/**
 *  This method returns the Id of the object with the lowest
 *  valid index valud.
 *
 *  @param[in] _api is the API of the object
 *  @param[in] _class is the Object Class of the object
 *  @param[in] _node is the node where the object resides
 */
#define RTEMS_OBJECT_ID_INITIAL(_api, _class, _node) \
   OBJECTS_ID_INITIAL(_api, _class, _node)

/**
 *  This constant is the maximum valid object Id.
 */
#define RTEMS_OBJECT_ID_FINAL                OBJECTS_ID_FINAL

/**
 *  The following constant defines the minimum stack size which every
 *  thread must exceed.
 */
#define RTEMS_MINIMUM_STACK_SIZE  STACK_MINIMUM_SIZE

/**
 *  Constant for indefinite wait.  (actually an illegal interval)
 */
#define RTEMS_NO_TIMEOUT  WATCHDOG_NO_TIMEOUT

/**
 *  An MPCI must support packets of at least this size.
 */
#define RTEMS_MINIMUM_PACKET_SIZE  MP_PACKET_MINIMUM_PACKET_SIZE

/**
 *  The following constant defines the number of uint32_t's
 *  in a packet which must be converted to native format in a
 *  heterogeneous system.  In packets longer than
 *  MP_PACKET_MINIMUN_HETERO_CONVERSION uint32_t's, some of the "extra" data
 *  may a user message buffer which is not automatically endian swapped.
 */
#define RTEMS_MINIMUN_HETERO_CONVERSION  MP_PACKET_MINIMUN_HETERO_CONVERSION

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
