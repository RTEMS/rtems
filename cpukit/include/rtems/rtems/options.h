/**
 * @file rtems/rtems/options.h
 *
 * @defgroup ClassicOptions Classic API Options
 *
 * @ingroup ClassicRTEMS
 * @brief Options Available on Many Directives
 *
 * This include file contains information which defines the
 * options available on many directives.
 */

/* COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_OPTIONS_H
#define _RTEMS_RTEMS_OPTIONS_H

#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicOptions Classic API Options
 *
 * @ingroup ClassicRTEMS
 *
 * This encapsulates functionality related to the options argument
 * to Classic API blocking operations. The primary option is whether
 * or not a task is willing to wait for the operation to complete.
 */
/**@{*/

/**
 *  The following type defines the control block used to manage
 *  option sets.
 */
typedef uint32_t   rtems_option;

/**
 *  The following constants define the individual options which may
 *  be used to compose an option set.
 */
#define RTEMS_DEFAULT_OPTIONS   0x00000000

/**
 *  This option constants indicates that the task is to wait on resource.
 */
#define RTEMS_WAIT      0x00000000
/**
 *  This option constants indicates that the task is to not wait on
 *  the resource.  If it is not available, return immediately with
 *  a status to indicate unsatisfied.
 */
#define RTEMS_NO_WAIT   0x00000001

/**
 *  This option constants indicates that the task wishes to wait until
 *  all events of interest are available.
 */
#define RTEMS_EVENT_ALL 0x00000000

/**
 *  This option constants indicates that the task wishes to wait until
 *  ANY events of interest are available.
 */
#define RTEMS_EVENT_ANY 0x00000002

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
