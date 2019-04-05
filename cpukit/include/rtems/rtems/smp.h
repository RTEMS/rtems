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
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_SMP_H
#define _RTEMS_RTEMS_SMP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicSMP SMP Services
 *
 * @ingroup RTEMSAPIClassic
 *
 * This encapsulates functionality which is useful for SMP applications.
 *
 * @{
 */

/**
 * @brief Returns the count of processors in the system.
 *
 * On uni-processor configurations a value of one will be returned.
 *
 * On SMP configurations this returns the value of a global variable set during
 * system initialization to indicate the count of utilized processors.  The
 * processor count depends on the physically or virtually available processors
 * and application configuration.  The value will always be less than or equal
 * to the maximum count of application configured processors.
 *
 * @return The count of processors being utilized.
 */
uint32_t rtems_get_processor_count(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
