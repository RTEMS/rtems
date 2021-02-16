/**
 * @file
 *
 * @ingroup RTEMSImplClassicSignal
 *
 * @brief This header file provides the implementation interfaces of
 *   the @ref RTEMSImplClassicSignal.
 */

/* COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_SIGNALIMPL_H
#define _RTEMS_RTEMS_SIGNALIMPL_H

#include <rtems/rtems/signal.h>
#include <rtems/score/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSImplClassicSignal Signal Manager
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the Signal Manager implementation.
 */

#ifdef __cplusplus
}
#endif

#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/signalmp.h>
#endif

#endif
/* end of include file */
