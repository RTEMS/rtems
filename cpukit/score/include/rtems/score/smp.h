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
 *  http://www.rtems.com/license/LICENSE.
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

/**
 *  This defines the bit which indicates the interprocessor interrupt
 *  has been requested so that RTEMS will reschedule on this CPU
 *  because the currently executing thread needs to be switched out.
 */
#define RTEMS_BSP_SMP_CONTEXT_SWITCH_NECESSARY  0x01

/**
 *  This defines the bit which indicates the interprocessor interrupt
 *  has been requested so that RTEMS will reschedule on this CPU
 *  because the currently executing thread has been sent a signal.
 */
#define RTEMS_BSP_SMP_SIGNAL_TO_SELF            0x02

/**
 *  This defines the bit which indicates the interprocessor interrupt
 *  has been requested so that this CPU will be shutdown.  This is done
 *  as part of rtems_executive_shutdown().
 */
#define RTEMS_BSP_SMP_SHUTDOWN                  0x04

#if !defined( ASM )

#if defined( RTEMS_SMP )
  SCORE_EXTERN uint32_t _SMP_Processor_count;

  static inline uint32_t _SMP_Get_processor_count( void )
  {
    return _SMP_Processor_count;
  }
#else
  #define _SMP_Get_processor_count() ( ( uint32_t ) 1 )
#endif

#if defined( RTEMS_SMP )

/**
 *  @brief Sends a SMP message to a processor.
 *
 *  The target processor may be the sending processor.
 *
 *  @param[in] cpu The target processor of the message.
 *  @param[in] message The message.
 */
void _SMP_Send_message( int cpu, uint32_t message );

/**
 *  @brief Request of others CPUs.
 *
 *  This method is invoked by RTEMS when it needs to make a request
 *  of the other CPUs.  It should be implemented using some type of
 *  interprocessor interrupt. CPUs not including the originating
 *  CPU should receive the message.
 *
 *  @param [in] message is message to send
 */
void _SMP_Broadcast_message(
  uint32_t  message
);

/**
 *  @brief Request other cores to perform first context switch.
 *
 *  Send message to other cores requesting them to perform
 *  their first context switch operation.
 */
void _SMP_Request_other_cores_to_perform_first_context_switch(void);

/**
 *  @brief Request dispatch on other cores.
 *
 *  Send message to other cores requesting them to perform
 *  a thread dispatch operation.
 */
void _SMP_Request_other_cores_to_dispatch(void);

/**
 *  @brief Request other cores to shutdown.
 *
 *  Send message to other cores requesting them to shutdown.
 */
void _SMP_Request_other_cores_to_shutdown(void);

#endif /* defined( RTEMS_SMP ) */

#endif /* !defined( ASM ) */

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
