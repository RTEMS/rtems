/**
 * @file
 *
 * @brief Constants and Structures Associated
 * with the Initialization Manager
 *
 * This include file contains all the constants and structures associated
 * with the Initialization Manager.  This manager is responsible for
 * initializing RTEMS, creating and starting all configured initialization
 * tasks, invoking the initialization routine for each user-supplied device
 * driver, and initializing the optional multiprocessor layer.
 *
 * This manager provides directives to:
 *
 *    + initialize the RTEMS executive
 *    + shutdown the RTEMS executive
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
 * The following defines the default Multiprocessing Configuration
 * Table.  This table is used in a single processor system.
 */
extern const rtems_multiprocessing_table
  _Initialization_Default_multiprocessing_table;
#endif

/**
 * @brief RTEMS data structures initialization.
 *
 * This routine implements the portion of the RTEMS initializatin process
 * that involves initializing data structures to a state that scheduling
 * can occur in a consistent manner.
 */
void rtems_initialize_data_structures(void);

/**
 * @brief RTEMS initialization before the device drivers are initialized.
 *
 * This routine implements the portion of RTEMS initialization that
 * is done immediately before device drivers are initialized.
 */
void rtems_initialize_before_drivers(void);

/**
 * @brief RTEMS initialization that initializes all device drivers.
 *
 * This routine implements the portion of RTEMS initialization that
 * initializes all device drivers.
 */
void rtems_initialize_device_drivers(void);

/**
 * @brief Starts the multitasking.
 *
 * This directive initiates multitasking and performs a context switch to the
 * first user application task and may enable interrupts as a side-effect of
 * that context switch.  The context switch saves the executing context.  The
 * application runs now.  The directive rtems_shutdown_executive() will return
 * to the saved context.  The exit() function will use this directive.
 *
 * After a return to the saved context a fatal system state is reached.  The
 * fatal source is RTEMS_FATAL_SOURCE_EXIT with a fatal code set to the value
 * passed to rtems_shutdown_executive().
 *
 * This directive does not return.
 */
void rtems_initialize_start_multitasking(void)
  RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

/**
 * @brief Shutdown the RTEMS environment.
 *
 * This routine implements the rtems_shutdown_executive directive.  The
 * invocation of this directive results in the RTEMS environment being
 * shutdown and multitasking halted.  From the application's perspective,
 * invocation of this directive results in the rtems_initialize_executive
 * directive exitting to the startup code which invoked it.
 */
void rtems_shutdown_executive(
  uint32_t   result
) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
