/**
 * @file
 *
 * @ingroup ScoreSMP
 *
 * @brief SuperCore SMP Support API
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SMP_H
#define _RTEMS_SCORE_SMP_H

#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ScoreSMP SMP Support
 *
 * @ingroup Score
 *
 * This defines the interface of the SuperCore SMP support.
 *
 * @{
 */

#if defined( RTEMS_SMP )
  SCORE_EXTERN uint32_t _SMP_Processor_count;

  static inline uint32_t _SMP_Get_processor_count( void )
  {
    return _SMP_Processor_count;
  }
#else
  #define _SMP_Get_processor_count() UINT32_C(1)
#endif

#if defined( RTEMS_SMP )
  static inline uint32_t _SMP_Get_current_processor( void )
  {
    return _CPU_SMP_Get_current_processor();
  }
#else
  #define _SMP_Get_current_processor() UINT32_C(0)
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
