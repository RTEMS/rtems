/**
 * @file rtems/rtems/config.h
 *
 *  This include file contains the table of user defined configuration
 *  parameters specific for the RTEMS API.
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

#ifndef _RTEMS_RTEMS_CONFIG_H
#define _RTEMS_RTEMS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rtems/types.h>
#include <rtems/rtems/tasks.h>

/**
 *  @defgroup ClassicConfig Classic API Configuration
 *
 *  This encapsulates functionality which XXX
 */
/**@{*/

/**
 *  The following records define the Configuration Table.  The
 *  information contained in this table is required in all
 *  RTEMS systems, whether single or multiprocessor.  This
 *  table primarily defines the following:
 *
 *     + required number of each object type
 */
typedef struct {
  uint32_t                    maximum_tasks;
  uint32_t                    maximum_timers;
  uint32_t                    maximum_semaphores;
  uint32_t                    maximum_message_queues;
  uint32_t                    maximum_partitions;
  uint32_t                    maximum_regions;
  uint32_t                    maximum_ports;
  uint32_t                    maximum_periods;
  uint32_t                    maximum_barriers;
  uint32_t                    number_of_initialization_tasks;
  rtems_initialization_tasks_table *User_initialization_tasks_table;
} rtems_api_configuration_table;

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
