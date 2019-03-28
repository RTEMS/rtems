/**
 * @file
 *
 * @ingroup ClassicConfig
 *
 * This include file contains the table of user defined configuration
 * parameters specific for the RTEMS API.
 */

/* COPYRIGHT (c) 1989-2013.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_CONFIG_H
#define _RTEMS_RTEMS_CONFIG_H

#include <rtems/rtems/types.h>
#include <rtems/rtems/tasks.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ClassicConfig Configuration
 *
 *  @ingroup RTEMSAPIClassic
 *
 *  This encapsulates functionality related to the application's configuration
 *  of the Classic API including the maximum number of each class of objects.
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

  /**
   * This field contains the number of Classic API Initialization
   * Tasks which are configured for this application.
   */
  uint32_t                    number_of_initialization_tasks;

  /**
   * This field is the set of Classic API Initialization
   * Tasks which are configured for this application.
   */
  rtems_initialization_tasks_table *User_initialization_tasks_table;
} rtems_api_configuration_table;

/**
 *  @brief RTEMS API Configuration Table
 *
 *  This is the RTEMS API Configuration Table expected to be generated
 *  by confdefs.h.
 */
extern rtems_api_configuration_table Configuration_RTEMS_API;

/**@}*/

uint32_t rtems_configuration_get_maximum_barriers( void );

uint32_t rtems_configuration_get_maximum_message_queues( void );

uint32_t rtems_configuration_get_maximum_partitions( void );

uint32_t rtems_configuration_get_maximum_periods( void );

uint32_t rtems_configuration_get_maximum_ports( void );

uint32_t rtems_configuration_get_maximum_regions( void );

uint32_t rtems_configuration_get_maximum_semaphores( void );

uint32_t rtems_configuration_get_maximum_timers( void );

uint32_t rtems_configuration_get_maximum_tasks( void );

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
