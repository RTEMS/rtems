/**
 * @file rtems/rtems/smp.h
 *
 * @defgroup ClassicSMP Classic API SMP Services
 *
 * @ingroup ClassicRTEMS
 * @brief SMP information and services.
 *
 * Most of the SMP interface is hidden from the application
 * and exists between the BSP and RTEMS.
 */

/* COPYRIGHT (c) 1989-2011.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_SMP_H
#define _RTEMS_RTEMS_SMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/smp.h>

/**
 *  @defgroup ClassicSMP Classic API SMP Services
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates functionality which is useful for SMP applications.
 */
/**@{*/

/**
 * @brief Obtain Number of Cores in System
 *
 * This method returns the number of CPU cores that are currently in
 * the system. This will always be less than or equal to the number
 * of maximum number of cores which were configured.
 *
 * @retval This method returns the number of cores in this system.
 */
#define rtems_smp_get_number_of_processors() \
    (_SMP_Processor_count)

/**
 * @brief Obtain Current Core Number
 *
 * This method returns the id of the current CPU core.
 *
 * @retval This method returns the id of the current CPU core.
 */
#define rtems_smp_get_current_processor() \
    bsp_smp_processor_id()

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
