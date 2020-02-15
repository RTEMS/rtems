/**
 * @file
 *
 * @ingroup RTEMSScoreSMPImpl
 *
 * @brief SuperCore SMP Implementation
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SMPIMPL_H
#define _RTEMS_SCORE_SMPIMPL_H

#include <rtems/score/smp.h>
#include <rtems/score/percpu.h>
#include <rtems/score/processormask.h>
#include <rtems/fatal.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreSMP
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
#define SMP_MESSAGE_SHUTDOWN 0x1UL

/**
 * @brief SMP message to perform per-processor jobs.
 *
 * @see _SMP_Send_message().
 */
#define SMP_MESSAGE_PERFORM_JOBS 0x2UL

/**
 * @brief SMP fatal codes.
 */
typedef enum {
  SMP_FATAL_BOOT_PROCESSOR_NOT_ASSIGNED_TO_SCHEDULER,
  SMP_FATAL_MANDATORY_PROCESSOR_NOT_PRESENT,
  SMP_FATAL_MULTITASKING_START_ON_INVALID_PROCESSOR,
  SMP_FATAL_MULTITASKING_START_ON_UNASSIGNED_PROCESSOR,
  SMP_FATAL_SHUTDOWN,
  SMP_FATAL_SHUTDOWN_RESPONSE,
  SMP_FATAL_START_OF_MANDATORY_PROCESSOR_FAILED,
  SMP_FATAL_SCHEDULER_PIN_OR_UNPIN_NOT_SUPPORTED,
  SMP_FATAL_WRONG_CPU_STATE_TO_PERFORM_JOBS
} SMP_Fatal_code;

/**
 * @brief Terminates with the given code.
 *
 * @param code The code for the termination.
 */
static inline void _SMP_Fatal( SMP_Fatal_code code )
{
  _Terminate( RTEMS_FATAL_SOURCE_SMP, code );
}

/**
 * @brief Initializes SMP Handler
 *
 * This method initialize the SMP Handler.
 */
#if defined( RTEMS_SMP )
  void _SMP_Handler_initialize( void );
#else
  #define _SMP_Handler_initialize() \
    do { } while ( 0 )
#endif

#if defined( RTEMS_SMP )

/**
 * @brief Set of online processors.
 *
 * A processor is online if was started during system initialization.  In this
 * case its corresponding bit in the mask is set.
 *
 * @see _SMP_Handler_initialize().
 */
extern Processor_mask _SMP_Online_processors;

/**
 * @brief Performs high-level initialization of a secondary processor and runs
 * the application threads.
 *
 * The low-level initialization code must call this function to hand over the
 * control of this processor to RTEMS.  Interrupts must be disabled.  It must
 * be possible to send inter-processor interrupts to this processor.  Since
 * interrupts are disabled the inter-processor interrupt delivery is postponed
 * until interrupts are enabled the first time.  Interrupts are enabled during
 * the execution begin of threads in case they have interrupt level zero (this
 * is the default).
 *
 * The pre-requisites for the call to this function are
 * - disabled interrupts,
 * - delivery of inter-processor interrupts is possible,
 * - a valid stack pointer and enough stack space,
 * - a valid code memory, and
 * - a valid BSS section.
 *
 * This function must not be called by the main processor.  The main processor
 * uses _Thread_Start_multitasking() instead.
 *
 * This function does not return to the caller.
 *
 * @param cpu_self The current processor control.
 */
void _SMP_Start_multitasking_on_secondary_processor(
  Per_CPU_Control *cpu_self
) RTEMS_NO_RETURN;

/**
 * @brief Interrupts handler for inter-processor interrupts.
 *
 * @param[in, out] cpu_self The cpu control for the operation.
 *
 * @return The received message.
 */
static inline long unsigned _SMP_Inter_processor_interrupt_handler(
  Per_CPU_Control *cpu_self
)
{
  unsigned long message;

  /*
   * In the common case the inter-processor interrupt is issued to carry out a
   * thread dispatch.
   */
  cpu_self->dispatch_necessary = true;

  message = _Atomic_Exchange_ulong(
    &cpu_self->message,
    0,
    ATOMIC_ORDER_ACQUIRE
  );

  if ( RTEMS_PREDICT_FALSE( message != 0 ) ) {
    if ( ( message & SMP_MESSAGE_SHUTDOWN ) != 0 ) {
      _SMP_Fatal( SMP_FATAL_SHUTDOWN_RESPONSE );
      /* does not continue past here */
    }

    if ( ( message & SMP_MESSAGE_PERFORM_JOBS ) != 0 ) {
      _Per_CPU_Perform_jobs( cpu_self );
    }
  }

  return message;
}

/**
 * @brief Checks if the processor with the specified index should be started.
 *
 * @param cpu_index The processor index.
 *
 * @retval true The processor should be started.
 * @retval false The processor should not be started.
 */
bool _SMP_Should_start_processor( uint32_t cpu_index );

/**
 * @brief Sends an SMP message to a processor.
 *
 * The target processor may be the sending processor.
 *
 * @param cpu_index The target processor of the message.
 * @param message The message to send.
 */
void _SMP_Send_message( uint32_t cpu_index, unsigned long message );

/**
 * @brief Sends an SMP message to all other online processors.
 *
 * @param message The message to send.
 */
void _SMP_Send_message_broadcast(
  unsigned long message
);

/**
 * @brief Sends an SMP message to a set of processors.
 *
 * The sending processor may be part of the set.
 *
 * @param targets The set of processors to send the message.
 * @param message The message to send.
 */
void _SMP_Send_message_multicast(
  const Processor_mask *targets,
  unsigned long         message
);

typedef void ( *SMP_Action_handler )( void *arg );

/**
 * @brief Initiates an SMP multicast action to the set of target processors.
 *
 * The current processor may be part of the set.  The caller must ensure that
 * no thread dispatch can happen during the call of this function, otherwise
 * the behaviour is undefined.  In case a target processor is in a wrong state
 * to process per-processor jobs, then this function results in an
 * SMP_FATAL_WRONG_CPU_STATE_TO_PERFORM_JOBS fatal SMP error.
 *
 * @param targets The set of target processors for the action.
 * @param handler The multicast action handler.
 * @param arg The multicast action argument.
 */
void _SMP_Multicast_action(
  const Processor_mask *targets,
  SMP_Action_handler    handler,
  void                 *arg
);

/**
 * @brief Initiates an SMP multicast action to the set of all online
 * processors.
 *
 * Simply calls _SMP_Multicast_action() with _SMP_Get_online_processors() as
 * the target processor set.
 *
 * @param handler The multicast action handler.
 * @param arg The multicast action argument.
 */
void _SMP_Broadcast_action(
  SMP_Action_handler  handler,
  void               *arg
);

/**
 * @brief Initiates an SMP multicast action to the set of all online
 * processors excluding the current processor.
 *
 * Simply calls _SMP_Multicast_action() with _SMP_Get_online_processors() as
 * the target processor set excluding the current processor.
 *
 * @param handler The multicast action handler.
 * @param arg The multicast action argument.
 */
void _SMP_Othercast_action(
  SMP_Action_handler  handler,
  void               *arg
);

/**
 * @brief Initiates an SMP action on the specified target processor.
 *
 * This is an optimized variant of _SMP_Multicast_action().
 *
 * @param cpu_index The index of the target processor.
 * @param handler The action handler.
 * @param arg The action argument.
 */
void _SMP_Unicast_action(
  uint32_t            cpu_index,
  SMP_Action_handler  handler,
  void               *arg
);

/**
 * @brief Ensures that all store operations issued by the current processor
 * before the call this function are visible to all other online processors.
 *
 * Simply calls _SMP_Othercast_action() with an empty multicast action.
 */
void _SMP_Synchronize( void );

#endif /* defined( RTEMS_SMP ) */

/**
 * @brief Requests a multitasking start on all configured and available
 * processors.
 */
#if defined( RTEMS_SMP )
  void _SMP_Request_start_multitasking( void );
#else
  #define _SMP_Request_start_multitasking() \
    do { } while ( 0 )
#endif

/**
 * @brief Requests a shutdown of all processors.
 *
 * This function is a part of the system termination procedure.
 *
 * @see _Terminate().
 */
#if defined( RTEMS_SMP )
  void _SMP_Request_shutdown( void );
#else
  #define _SMP_Request_shutdown() \
    do { } while ( 0 )
#endif

/**
 * @brief Gets all online processors
 *
 * @return The processor mask with all online processors.
 */
RTEMS_INLINE_ROUTINE const Processor_mask *_SMP_Get_online_processors( void )
{
#if defined(RTEMS_SMP)
  return &_SMP_Online_processors;
#else
  return &_Processor_mask_The_one_and_only;
#endif
}

/**
 * @brief Indicate if inter-processor interrupts are needed.
 *
 * @return True if inter-processor interrupts are needed for the correct system
 * operation, otherwise false.
 */
RTEMS_INLINE_ROUTINE const bool _SMP_Need_inter_processor_interrupts( void )
{
  /*
   * Use the configured processor maximum instead of the actual to allow
   * testing on uni-processor systems.
   */
  return _SMP_Processor_configured_maximum > 1;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
