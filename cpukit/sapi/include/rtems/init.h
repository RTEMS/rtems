/**
 * @file rtems/init.h
 *
 *
 *  This include file contains all the constants and structures associated
 *  with the Initialization Manager.  This manager is responsible for
 *  initializing RTEMS, creating and starting all configured initialization
 *  tasks, invoking the initialization routine for each user-supplied device
 *  driver, and initializing the optional multiprocessor layer.
 *
 *  This manager provides directives to:
 *
 *     + initialize the RTEMS executive
 *     + shutdown the RTEMS executive
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_INIT_H
#define _RTEMS_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rtems/types.h>
#include <rtems/config.h>
#include <rtems/rtems/intr.h>

#if defined(RTEMS_MULTIPROCESSING)
/**
 *  The following defines the default Multiprocessing Configuration
 *  Table.  This table is used in a single processor system.
 */
extern const rtems_multiprocessing_table
  _Initialization_Default_multiprocessing_table;
#endif

/**
 *  @brief rtems_initialize_data_structures
 *
 *  This routine implements the portion of the RTEMS initializatin process
 *  that involves initializing data structures to a state that scheduling
 *  can occur in a consistent manner.
 */
void rtems_initialize_data_structures(void);

/**
 *  @brief rtems_initialize_before_drivers
 *
 *  This routine implements the portion of RTEMS initialization that
 *  is done immediately before device drivers are initialized.
 */
void rtems_initialize_before_drivers(void);

/**
 *  @brief rtems_initialize_device_drivers
 *
 *  This routine implements the portion of RTEMS initialization that
 *  initializes all device drivers.
 */
void rtems_initialize_device_drivers(void);

/**
 *  @brief rtems_initialize_start_multitasking
 *
 *  This routine implements the early portion of rtems_initialize_executive
 *  directive up to the pretasking hook. This directive is invoked at system
 *  startup to initialize the RTEMS multitasking environment.
 *
 *  @return This method returns the status code passed into the 
 *          @ref rtems_shutdown_executive directive.
 */
uint32_t rtems_initialize_start_multitasking(void);

/**
 *  @brief rtems_shutdown_executive
 *
 *  This routine implements the rtems_shutdown_executive directive.  The
 *  invocation of this directive results in the RTEMS environment being
 *  shutdown and multitasking halted.  From the application's perspective,
 *  invocation of this directive results in the rtems_initialize_executive
 *  directive exitting to the startup code which invoked it.
 */
void rtems_shutdown_executive(
  uint32_t   result
) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
