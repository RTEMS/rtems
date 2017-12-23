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
 * @ingroup ClassicRTEMS
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

/**
 * @brief Returns the index of the current processor.
 *
 * On uni-processor configurations a value of zero will be returned.
 *
 * On SMP configurations an architecture specific method is used to obtain the
 * index of the current processor in the system.  The set of processor indices
 * is the range of integers starting with zero up to the processor count minus
 * one.
 *
 * Outside of sections with disabled thread dispatching the current processor
 * index may change after every instruction since the thread may migrate from
 * one processor to another.  Sections with disabled interrupts are sections
 * with thread dispatching disabled.
 *
 * @return The index of the current processor.
 */
uint32_t rtems_get_current_processor(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
