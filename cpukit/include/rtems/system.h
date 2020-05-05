/**
 * @file
 *
 * @brief Information Included in Every Function in the Executive
 *
 * This include file contains information that is included in every
 * function in the executive.  This must be the first include file
 * included in all internal RTEMS files.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SYSTEM_H
#define _RTEMS_SYSTEM_H

#include <rtems/score/cpu.h>

#ifndef _RTEMS_H
#warning "This header file is deprecated and will be removed in RTEMS 6"
#endif

/**
 *  @defgroup RTEMSScoreSystem System Information
 *
 *  @ingroup RTEMSScore
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ASM

/*
 * The use of this variable is deprecated, use rtems_get_version_string()
 * instead.
 */
extern RTEMS_DEPRECATED const char _RTEMS_version[];

/**
 *  The following is the extern for the RTEMS copyright string.
 */
extern RTEMS_DEPRECATED const char _Copyright_Notice[];

typedef RTEMS_DEPRECATED uint32_t RTEMS_MAXIMUM_NAME_LENGTH;
/* The use of this define is deprecated, use sizeof(rtems_name) instead */
#define RTEMS_MAXIMUM_NAME_LENGTH sizeof(RTEMS_MAXIMUM_NAME_LENGTH)

#endif /* ASM */

#ifdef __cplusplus
}
#endif

/**@}*/
#endif
/* end of include file */
