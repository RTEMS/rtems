/*  init.h
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
 *
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

#ifndef __RTEMS_INIT_h
#define __RTEMS_INIT_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rtems/types.h>
#include <rtems/config.h>
#include <rtems/rtems/intr.h>

/*
 *  The following defines the default Multiprocessing Configuration
 *  Table.  This table is used in a single processor system.
 */

extern const rtems_multiprocessing_table
  _Initialization_Default_multiprocessing_table;

/*
 *  rtems_initialize_executive
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_initialize_executive directive.  This
 *  directive is invoked at system startup to initialize the RTEMS
 *  multitasking environment.
 */

void rtems_initialize_executive(
  rtems_configuration_table *configuration_table,
  rtems_cpu_table           *cpu_table
);

/*
 *  rtems_initialize_executive_early
 *
 *  DESCRIPTION:
 *
 *  This routine implements the early portion of rtems_initialize_executive
 *  directive up to the pretasking hook. This directive is invoked at system
 *  startup to initialize the RTEMS multitasking environment.
 */

rtems_interrupt_level rtems_initialize_executive_early(
  rtems_configuration_table *configuration_table,
  rtems_cpu_table           *cpu_table
);

/*
 *  rtems_initialize_executive_late
 *
 *  DESCRIPTION:
 *
 *  This routine implements the early portion of rtems_initialize_executive
 *  directive up to the pretasking hook. This directive is invoked at system
 *  startup to initialize the RTEMS multitasking environment.
 */

void rtems_initialize_executive_late(
  rtems_interrupt_level bsp_level
);

/*
 *  rtems_shutdown_executive
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_shutdown_executive directive.  The
 *  invocation of this directive results in the RTEMS environment being
 *  shutdown and multitasking halted.  From the application's perspective,
 *  invocation of this directive results in the rtems_initialize_executive
 *  directive exitting to the startup code which invoked it.
 */

void rtems_shutdown_executive(
  unsigned32 result
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
