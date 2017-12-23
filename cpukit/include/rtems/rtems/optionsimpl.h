/**
 * @file
 *
 * @ingroup ClassicOptionsImpl
 *
 * @brief Classic Options Implementation
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_OPTIONSIMPL_H
#define _RTEMS_RTEMS_OPTIONSIMPL_H

#include <rtems/rtems/options.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicOptionsImpl Classic Options Implementation
 *
 * @ingroup ClassicOptions
 *
 * @{
 */

/**
 *  @brief Checks if the RTEMS_NO_WAIT option is enabled in option_set.
 *
 *  This function returns TRUE if the RTEMS_NO_WAIT option is enabled in
 *  option_set, and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Options_Is_no_wait (
  rtems_option option_set
)
{
   return (option_set & RTEMS_NO_WAIT) ? true : false;
}

/**
 *  @brief Checks if the RTEMS_EVENT_ANY option is enabled in OPTION_SET.
 *
 *  This function returns TRUE if the RTEMS_EVENT_ANY option is enabled in
 *  OPTION_SET, and FALSE otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Options_Is_any (
  rtems_option option_set
)
{
   return (option_set & RTEMS_EVENT_ANY) ? true : false;
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
