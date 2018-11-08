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

#include <rtems/rtems/types.h>
#include <rtems/config.h>
#include <rtems/rtems/intr.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the system and starts multitasking.
 *
 * Iterates through the system initialization linker set and invokes the
 * registered handlers.  The final step is to start multitasking.
 *
 * This directive should be called by boot_card() only.
 *
 * This directive does not return.
 */
void rtems_initialize_executive(void)
  RTEMS_NO_RETURN;

/**
 * @brief Shutdown the RTEMS environment.
 *
 * This routine implements the rtems_shutdown_executive directive.  The
 * invocation of this directive results in the RTEMS environment being
 * shutdown and multitasking halted.  The system is terminated with a fatal
 * source of RTEMS_FATAL_SOURCE_EXIT and the specified result code.
 */
void rtems_shutdown_executive(
  uint32_t   result
) RTEMS_NO_RETURN;

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
