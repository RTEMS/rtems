/**
 * @file
 *
 * @ingroup RTEMSScorePerCPU
 *
 * @brief This source file contains the uniprocessor definition of
 *   ::_Per_CPU_Information and some static assertions.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/percpu.h>

RTEMS_STATIC_ASSERT(
  sizeof( CPU_Uint32ptr ) >= sizeof( uintptr_t ),
  CPU_Uint32ptr_greater_equal_uintptr_t
);

RTEMS_STATIC_ASSERT(
  sizeof( CPU_Uint32ptr ) >= sizeof( uint32_t ),
  CPU_Uint32ptr_greater_equal_uint32_t
);

#if !defined(RTEMS_SMP)
  /*
   * On single core systems, we can efficiently directly access a single
   * statically allocated per cpu structure.  And the fields are initialized
   * as individual elements just like it has always been done.
   */
  Per_CPU_Control_envelope _Per_CPU_Information[1];
#endif
