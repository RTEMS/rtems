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
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SMPIMPL_H
#define _RTEMS_SCORE_SMPIMPL_H

#include <rtems/score/smp.h>
#include <rtems/score/percpu.h>
#include <rtems/score/processormask.h>
#include <rtems/fatal.h>
#include <rtems/rtems/cache.h>

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
#define SMP_MESSAGE_SHUTDOWN 0x1UL

/**
 * @brief SMP message to request a test handler invocation.
 *
 * @see _SMP_Send_message().
 */
#define SMP_MESSAGE_TEST 0x2UL

/**
 * @brief SMP message to request a multicast action.
 *
 * @see _SMP_Send_message().
 */
#define SMP_MESSAGE_MULTICAST_ACTION 0x4UL

/**
 * @brief SMP message to request a clock tick.
 *
 * This message is provided for systems without a proper interrupt affinity
 * support and may be used by the clock driver.
 *
 * @see _SMP_Send_message().
 */
#define SMP_MESSAGE_CLOCK_TICK 0x8UL

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
  SMP_FATAL_SCHEDULER_PIN_OR_UNPIN_NOT_SUPPORTED
} SMP_Fatal_code;

static inline void _SMP_Fatal( SMP_Fatal_code code )
{
  _Terminate( RTEMS_FATAL_SOURCE_SMP, code );
}

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
 * @param[in] cpu_self The current processor control.
 */
void _SMP_Start_multitasking_on_secondary_processor(
  Per_CPU_Control *cpu_self
) RTEMS_NO_RETURN;

typedef void ( *SMP_Test_message_handler )( Per_CPU_Control *cpu_self );

extern SMP_Test_message_handler _SMP_Test_message_handler;

/**
 * @brief Sets the handler for test messages.
 *
 * This handler can be used to test the inter-processor interrupt
 * implementation.
 */
static inline void _SMP_Set_test_message_handler(
  SMP_Test_message_handler handler
)
{
  _SMP_Test_message_handler = handler;
}

/**
 * @brief Processes all pending multicast actions.
 */
void _SMP_Multicast_actions_process( void );

/**
 * @brief Interrupt handler for inter-processor interrupts.
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

    if ( ( message & SMP_MESSAGE_TEST ) != 0 ) {
      ( *_SMP_Test_message_handler )( cpu_self );
    }

    if ( ( message & SMP_MESSAGE_MULTICAST_ACTION ) != 0 ) {
      _SMP_Multicast_actions_process();
    }
  }

  return message;
}

/**
 *  @brief Returns true, if the processor with the specified index should be
 *  started.
 *
 *  @param[in] cpu_index The processor index.
 *
 *  @retval true The processor should be started.
 *  @retval false Otherwise.
 */
bool _SMP_Should_start_processor( uint32_t cpu_index );

/**
 *  @brief Sends an SMP message to a processor.
 *
 *  The target processor may be the sending processor.
 *
 *  @param[in] cpu_index The target processor of the message.
 *  @param[in] message The message.
 */
void _SMP_Send_message( uint32_t cpu_index, unsigned long message );

/**
 *  @brief Sends an SMP message to all other online processors.
 *
 *  @param[in] message The message.
 */
void _SMP_Send_message_broadcast(
  unsigned long message
);

/**
 *  @brief Sends an SMP message to a set of processors.
 *
 *  The sending processor may be part of the set.
 *
 *  @param[in] targets The set of processors to send the message.
 *  @param[in] message The message.
 */
void _SMP_Send_message_multicast(
  const Processor_mask *targets,
  unsigned long         message
);

typedef void ( *SMP_Action_handler )( void *arg );

/**
 *  @brief Initiates an SMP multicast action to a set of processors.
 *
 *  The current processor may be part of the set.
 *
 *  @param[in] setsize The size of the set of target processors of the message.
 *  @param[in] cpus The set of target processors of the message.
 *  @param[in] handler The multicast action handler.
 *  @param[in] arg The multicast action argument.
 */
void _SMP_Multicast_action(
  const size_t setsize,
  const cpu_set_t *cpus,
  SMP_Action_handler handler,
  void *arg
);

/**
 * @brief Executes a handler with argument on the specified processor on behalf
 * of the boot processor.
 *
 * The calling processor must be the boot processor.  In case the specified
 * processor is not online or not in the
 * PER_CPU_STATE_READY_TO_START_MULTITASKING state, then no action is
 * performed.
 *
 * @param cpu The processor to execute the action.
 * @param handler The handler of the action.
 * @param arg The argument of the action.
 *
 * @retval true The handler executed on the specified processor.
 * @retval false Otherwise.
 *
 * @see _SMP_Before_multitasking_action_broadcast().
 */
bool _SMP_Before_multitasking_action(
  Per_CPU_Control    *cpu,
  SMP_Action_handler  handler,
  void               *arg
);

/**
 * @brief Executes a handler with argument on all online processors except the
 * boot processor on behalf of the boot processor.
 *
 * The calling processor must be the boot processor.
 *
 * @param handler The handler of the action.
 * @param arg The argument of the action.
 *
 * @retval true The handler executed on all online processors except the boot
 * processor.
 * @retval false Otherwise.
 *
 * @see _SMP_Before_multitasking_action().
 */
bool _SMP_Before_multitasking_action_broadcast(
  SMP_Action_handler  handler,
  void               *arg
);

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

RTEMS_INLINE_ROUTINE const Processor_mask *_SMP_Get_online_processors( void )
{
#if defined(RTEMS_SMP)
  return &_SMP_Online_processors;
#else
  return &_Processor_mask_The_one_and_only;
#endif
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
