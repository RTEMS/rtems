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
 * @brief SMP fatal codes.
 */
typedef enum {
  SMP_FATAL_BOOT_PROCESSOR_NOT_ASSIGNED_TO_SCHEDULER,
  SMP_FATAL_MANDATORY_PROCESSOR_NOT_PRESENT,
  SMP_FATAL_MULTITASKING_START_ON_INVALID_PROCESSOR,
  SMP_FATAL_MULTITASKING_START_ON_UNASSIGNED_PROCESSOR,
  SMP_FATAL_SHUTDOWN,
  SMP_FATAL_SHUTDOWN_RESPONSE,
  SMP_FATAL_START_OF_MANDATORY_PROCESSOR_FAILED
} SMP_Fatal_code;

static inline void _SMP_Fatal( SMP_Fatal_code code )
{
  _Terminate( RTEMS_FATAL_SOURCE_SMP, false, code );
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
 */
void _SMP_Start_multitasking_on_secondary_processor( void )
  RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

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
 */
static inline void _SMP_Inter_processor_interrupt_handler( void )
{
  Per_CPU_Control *cpu_self = _Per_CPU_Get();

  if ( _Atomic_Load_ulong( &cpu_self->message, ATOMIC_ORDER_RELAXED ) != 0 ) {
    unsigned long message = _Atomic_Exchange_ulong(
      &cpu_self->message,
      0UL,
      ATOMIC_ORDER_RELAXED
    );

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
 *  @brief Sends a SMP message to a processor.
 *
 *  The target processor may be the sending processor.
 *
 *  @param[in] cpu_index The target processor of the message.
 *  @param[in] message The message.
 */
void _SMP_Send_message( uint32_t cpu_index, unsigned long message );

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
void _SMP_Send_message_broadcast(
  unsigned long message
);

/**
 *  @brief Sends a SMP message to a set of processors.
 *
 *  The sending processor may be part of the set.
 *
 *  @param[in] setsize The size of the set of target processors of the message.
 *  @param[in] cpus The set of target processors of the message.
 *  @param[in] message The message.
 */
void _SMP_Send_message_multicast(
  const size_t setsize,
  const cpu_set_t *cpus,
  unsigned long message
);

typedef void ( *SMP_Multicast_action_handler )( void *arg );

/**
 *  @brief Initiates a SMP multicast action to a set of processors.
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
  SMP_Multicast_action_handler handler,
  void *arg
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

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
