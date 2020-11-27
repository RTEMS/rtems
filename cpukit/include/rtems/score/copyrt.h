/**
 * @file
 *
 * @ingroup RTEMSSuperCoreCopyright
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreCopyright.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_COPYRT_H
#define _RTEMS_SCORE_COPYRT_H

/**
 * @defgroup RTEMSScoreCopyright Copyright Notice
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the implementation to provide a copyright notice
 *   and the RTEMS version.
 *
 * @{
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This is the copyright string for RTEMS.
 */
extern const char _Copyright_Notice[];

/**
 * @brief This constant provides the RTEMS version string.
 *
 * The constant name does not follow the naming conventions.  Do not change it
 * for backward compatibility reasons.
 *
 * @see rtems_get_version_string()
 */
extern const char _RTEMS_version[];

#ifdef __cplusplus
}
#endif

/** @} */

#endif
/* end of include file */
