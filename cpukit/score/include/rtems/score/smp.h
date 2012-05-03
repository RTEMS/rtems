/**
 *  @file  rtems/score/smp.h
 *
 *  This include file defines the interface to the SuperCore
 *  SMP support that is used internally to RTEMS.
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

#if defined (RTEMS_SMP)
#include <rtems/score/percpu.h>

/**
 *  @defgroup SuperCore SMP Support
 *
 *  @ingroup Score
 *
 *  This defines the interface of the SuperCore support
 *  code for SMP support.
 */

/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

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

/**
 *  This defines the bit which indicates the interprocessor interrupt
 *  has been requested that the receiving CPU needs to perform a context
 *  switch to the first task.
 */
#define RTEMS_BSP_SMP_FIRST_TASK                0x08

#ifndef ASM
/**
 *  @brief Number of CPUs in SMP System
 *
 *  This variable is set during the SMP initialization sequence to
 *  indicate the number of CPUs in this system.
 */
SCORE_EXTERN uint32_t _SMP_Processor_count;

/**
 *  @brief Make Request of Others CPUs
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
 *  @brief Request Other Cores to Perform First Context Switch
 *
 *  Send message to other cores requesting them to perform
 *  their first context switch operation.
 */
void _SMP_Request_other_cores_to_perform_first_context_switch(void);

/**
 *  @brief Request Dispatch on Other Cores
 *
 *  Send message to other cores requesting them to perform
 *  a thread dispatch operation.
 */
void _SMP_Request_other_cores_to_dispatch(void);

/**
 *  @Brief Request Other Cores to Shutdown
 *
 *  Send message to other cores requesting them to shutdown.
 */
void _SMP_Request_other_cores_to_shutdown(void);

#endif

#ifdef __cplusplus
}
#endif

#endif
#endif
/* end of include file */
