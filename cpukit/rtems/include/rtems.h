/**
 * @file  rtems.h
 */
 
/*
 *  This include file contains information about RTEMS executive that
 *  is required by the application and is CPU independent.  It includes
 *  two (2) CPU dependent files to tailor its data structures for a
 *  particular processor.
 *
 *  COPYRIGHT (c) 1989-1999.
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

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Unless told otherwise, the RTEMS include files will hide some stuff
 *  from normal application code.  Defining this crosses a boundary which
 *  is undesirable since it means your application is using RTEMS features
 *  which are not included in the formally defined and supported API.
 *  Define this at your own risk.
 */

#if (!defined(__RTEMS_VIOLATE_KERNEL_VISIBILITY__)) && (!defined(__RTEMS_INSIDE__))
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

/*
 *  Obtain the RTEMS Version String
 *
 *  This method may be used to obtain the RTEMS version string.
 *
 *  This method returns a pointer to the version string.
 */
const char *rtems_get_version_string(void);

#define RTEMS_HAS_HARDWARE_FP CPU_HARDWARE_FP

/*
 *  The following define the constants which may be used in name searches.
 */

#define RTEMS_SEARCH_ALL_NODES   OBJECTS_SEARCH_ALL_NODES
#define RTEMS_SEARCH_OTHER_NODES OBJECTS_SEARCH_OTHER_NODES
#define RTEMS_SEARCH_LOCAL_NODE  OBJECTS_SEARCH_LOCAL_NODE
#define RTEMS_WHO_AM_I           OBJECTS_WHO_AM_I

/*
 * Parameters and return id's for _Objects_Get_next
 */

#define RTEMS_OBJECT_ID_INITIAL_INDEX        OBJECTS_ID_INITIAL_INDEX
#define RTEMS_OBJECT_ID_FINAL_INDEX          OBJECTS_ID_FINAL_INDEX
#define RTEMS_OBJECT_ID_INITIAL(api, class, node) OBJECTS_ID_INITIAL(api, class, node)

#define RTEMS_OBJECT_ID_FINAL                OBJECTS_ID_FINAL

/*
 *  The following constant defines the minimum stack size which every
 *  thread must exceed.
 */

#define RTEMS_MINIMUM_STACK_SIZE  STACK_MINIMUM_SIZE

/*
 *  Constant for indefinite wait.  (actually an illegal interval)
 */

#define RTEMS_NO_TIMEOUT  WATCHDOG_NO_TIMEOUT

/*
 *  An MPCI must support packets of at least this size.
 */

#define RTEMS_MINIMUM_PACKET_SIZE  MP_PACKET_MINIMUM_PACKET_SIZE

/*
 *  The following constant defines the number of uint32_t  's
 *  in a packet which must be converted to native format in a
 *  heterogeneous system.  In packets longer than
 *  MP_PACKET_MINIMUN_HETERO_CONVERSION uint32_t  's, some of the "extra" data
 *  may a user message buffer which is not automatically endian swapped.
 */

#define RTEMS_MINIMUN_HETERO_CONVERSION  MP_PACKET_MINIMUN_HETERO_CONVERSION

/*
 *  rtems_object_id_to_name
 *
 *  This directive returns the name associated with the specified
 *  object ID.
 *
 *  Input parameters:
 *    id   - object id
 *
 *  Output parameters:
 *    *name            - user defined object name
 *    RTEMS_SUCCESSFUL - if successful
 *    error code       - if unsuccessful
 */

rtems_status_code rtems_object_id_to_name(
  rtems_id      id,
  rtems_name   *name
);

/*
 *  rtems_object_get_name
 *
 *  This directive returns the name associated with the specified
 *  object ID.
 *
 *  Input parameters:
 *    id   - object id
 *
 *  Output parameters:
 *    *name            - user defined object name
 *    name             - if successful
 *    NULL             - if unsuccessful
 */

char *rtems_object_get_name(
  rtems_id       id,
  size_t         length,
  char          *name
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
