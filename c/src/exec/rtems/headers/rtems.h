/*  rtems.h
 *
 *  This include file contains information about RTEMS executive that
 *  is required by the application and is CPU independent.  It includes
 *  two (2) CPU dependent files to tailor its data structures for a
 *  particular processor.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __RTEMS_RTEMS_GENERIC_h
#define __RTEMS_RTEMS_GENERIC_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>

#include <rtems/config.h>
#include <rtems/init.h>
#include <rtems/rtems/tasks.h>
#include <rtems/rtems/intr.h>
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
#include <rtems/rtems/mp.h>

#include <rtems/rtems/support.h>
#include <rtems/core/sysstate.h>

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
 
#define RTEMS_OBJECT_ID_INITIAL_INDEX   OBJECTS_ID_INITIAL_INDEX
#define RTEMS_OBJECT_ID_FINAL_INDEX     OBJECTS_ID_FINAL_INDEX
 
#define RTEMS_OBJECT_ID_INITIAL(node)   OBJECTS_ID_INITIAL(node)
#define RTEMS_OBJECT_ID_FINAL           OBJECTS_ID_FINAL
 
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
 *  The following constant defines the number of unsigned32's
 *  in a packet which must be converted to native format in a
 *  heterogeneous system.  In packets longer than
 *  MP_PACKET_MINIMUN_HETERO_CONVERSION unsigned32's, some of the "extra" data
 *  may a user message buffer which is not automatically endian swapped.
 */
 
#define RTEMS_MINIMUN_HETERO_CONVERSION  MP_PACKET_MINIMUN_HETERO_CONVERSION


#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
