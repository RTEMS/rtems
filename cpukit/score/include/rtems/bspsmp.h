/**
 *  @file  rtems/bspsmp.h
 *
 *  @brief Interface Between RTEMS and an SMP Aware BSP
 *
 *  This include file defines the interface between RTEMS and an
 *  SMP aware BSP.  These methods will only be used when RTEMS
 *  is configured with SMP support enabled.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_BSPSMP_H
#define _RTEMS_BSPSMP_H

#include <rtems/score/cpuopts.h>

#if defined (RTEMS_SMP)
#include <rtems/score/percpu.h>

/**
 *  @defgroup RTEMS BSP SMP Interface
 *
 *  @ingroup Score
 *
 *  This defines the interface between RTEMS and the BSP for
 *  SMP support.  The interface uses the term primary
 *  to refer to the "boot" processor and secondary to refer
 *  to the "application" processors.  Different architectures
 *  use different terminology.
 *
 *  It is assumed that when the processor is reset and thus
 *  when RTEMS is initialized, that the primary processor is
 *  the only one executing.  The others are assumed to be in
 *  a quiescent or reset state awaiting a command to come online.
 */

/**@{*/

#ifdef __cplusplus
extern "C" {
#endif


#ifndef ASM

/**
 *  @brief Process the incoming interprocessor request.
 *
 *  This is the method called by the BSP's interrupt handler
 *  to process the incoming interprocessor request.
 */
void rtems_smp_process_interrupt(void);

#endif

#ifdef __cplusplus
}
#endif

#endif

/**@}*/
#endif

/* end of include file */
