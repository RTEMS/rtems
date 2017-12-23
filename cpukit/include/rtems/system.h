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

/**
 *  @defgroup ScoreSystem System Information
 *
 *  @ingroup Score
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ASM

#ifdef RTEMS_POSIX_API
/** The following is used by the POSIX implementation to catch bad paths.  */
int POSIX_NOT_IMPLEMENTED( void );
#endif

/**
 *  The following is the extern for the RTEMS version string.
 *
 *  @note The contents of this string are CPU specific.
 */
extern const char _RTEMS_version[];

/**
 *  The following is the extern for the RTEMS copyright string.
 */
extern const char _Copyright_Notice[];

/** This macro defines the maximum length of a Classic API name. */
#define RTEMS_MAXIMUM_NAME_LENGTH sizeof(rtems_name)

#endif /* ASM */

#ifdef __cplusplus
}
#endif

/**@}*/
#endif
/* end of include file */
