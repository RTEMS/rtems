/*  config.h
 *
 *  This include file contains the table of user defined configuration
 *  parameters specific for the RTEMS API.
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

#ifndef __RTEMS_RTEMS_CONFIGURATION_h
#define __RTEMS_RTEMS_CONFIGURATION_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rtems/types.h>
#include <rtems/rtems/tasks.h>

/*
 *  The following records define the Configuration Table.  The
 *  information contained in this table is required in all
 *  RTEMS systems, whether single or multiprocessor.  This
 *  table primarily defines the following:
 *
 *     + required number of each object type
 */

typedef struct {
  unsigned32                        maximum_tasks;
  unsigned32                        maximum_timers;
  unsigned32                        maximum_semaphores;
  unsigned32                        maximum_message_queues;
  unsigned32                        maximum_partitions;
  unsigned32                        maximum_regions;
  unsigned32                        maximum_ports;
  unsigned32                        maximum_periods;
  unsigned32                        number_of_initialization_tasks;
  rtems_initialization_tasks_table *User_initialization_tasks_table;
} rtems_api_configuration_table;

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
