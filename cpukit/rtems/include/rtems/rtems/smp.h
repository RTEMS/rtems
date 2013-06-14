/**
 * @file
 *
 * @ingroup ClassicSMP
 *
 * @brief SMP Services API
 */

/*
 * COPYRIGHT (c) 1989-2011.
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
 * @defgroup ClassicSMP SMP Services
 *
 * @ingroup ClassicRTEMS
 *
 * This encapsulates functionality which is useful for SMP applications.
 *
 * This API is also available on uni-processor configurations.  Here compile
 * time constants are used whenever possible.
 *
 * @{
 */

/**
 * @brief Returns the count of processors in the system.
 *
 * On uni-processor configurations this is a compile time constant and defined
 * to be one.
 *
 * On SMP configurations this returns the value of a global variable set during
 * system initialization to indicate the count of processors.  The processor
 * count depends on the hardware and application configuration.  The value will
 * always be less than or equal to the maximum count of application configured
 * processors.
 *
 * @return The count of processors in the system.
 */
#define rtems_smp_get_processor_count() \
  _SMP_Get_processor_count()

/**
 * @brief Obtain Current Core Number
 *
 * This method returns the id of the current CPU core.
 *
 * @retval This method returns the id of the current CPU core.
 */
#define rtems_smp_get_current_processor() \
    bsp_smp_processor_id()

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
