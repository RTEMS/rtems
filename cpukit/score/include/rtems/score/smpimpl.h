/**
 * @file
 *
 * @ingroup ScoreSMPImpl
 *
 * @brief SuperCore SMP Implementation
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SMPIMPL_H
#define _RTEMS_SCORE_SMPIMPL_H

#include <rtems/score/smp.h>

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
 * @brief SMP message to request a processor shutdown.
 *
 * @see _SMP_Send_message().
 */
#define SMP_MESSAGE_SHUTDOWN UINT32_C(0x1)

/**
 * @brief SMP fatal codes.
 */
typedef enum {
  SMP_FATAL_SHUTDOWN
} SMP_Fatal_code;

/**
 *  @brief Initialize SMP Handler
 *
 *  This method initialize the SMP Handler.
 */
#if defined( RTEMS_SMP )
  void _SMP_Handler_initialize( void );
#else
  #define _SMP_Handler_initialize() \
    do { } while ( 0 )
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
void _SMP_Send_message( uint32_t cpu, uint32_t message );

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

#endif /* defined( RTEMS_SMP ) */

/**
 *  @brief Request other cores to perform first context switch.
 *
 *  Send message to other cores requesting them to perform
 *  their first context switch operation.
 */
#if defined( RTEMS_SMP )
  void _SMP_Request_other_cores_to_perform_first_context_switch( void );
#else
  #define _SMP_Request_other_cores_to_perform_first_context_switch() \
    do { } while ( 0 )
#endif

/**
 *  @brief Request other cores to shutdown.
 *
 *  Send message to other cores requesting them to shutdown.
 */
#if defined( RTEMS_SMP )
  void _SMP_Request_other_cores_to_shutdown( void );
#else
  #define _SMP_Request_other_cores_to_shutdown() \
    do { } while ( 0 )
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
