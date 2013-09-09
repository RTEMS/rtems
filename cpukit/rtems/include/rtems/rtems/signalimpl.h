/**
 * @file
 *
 * @ingroup ClassicSignalImpl
 *
 * @brief Signals Implementation
 */

/* COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_SIGNALIMPL_H
#define _RTEMS_RTEMS_SIGNALIMPL_H

#include <rtems/rtems/signal.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicSignalImpl Signals Implementation
 *
 * @ingroup ClassicSignal
 */
/**@{*/

/**
 *  @brief Signal Manager Initialization
 *
 *  Signal Manager
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _Signal_Manager_initialization( void );

/**@}*/

#ifdef __cplusplus
}
#endif

#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/signalmp.h>
#endif

#endif
/* end of include file */
