/*  config.h
 *
 *  This include file contains the table of user defined configuration
 *  parameters specific for the RTEMS API.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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
