/**
 *  @file  rtems/bspsmp.h
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

#if defined (RTEMS_SMP)
#include <rtems/score/percpu.h>

/**
 *  @defgroup RTEMS BSP SMP Interface
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
 *  @brief Maximum Number of CPUs in SMP System
 *
 *  This variable is set during the SMP initialization sequence to
 *  indicate the Maximum number of CPUs in this system.
 */
extern uint32_t rtems_configuration_smp_maximum_processors;
 
/**
 *  @brief Initialize Secondary CPUs
 *
 *  This method is invoked by RTEMS during initialization to bring the
 *  secondary CPUs out of reset.
 *
 *  @param [in] maximum is the maximum number of CPU cores that RTEMS
 *              can handle
 *
 *  @return This method returns the number of cores available in the
 *          system.
 */
int bsp_smp_initialize(
  int maximum
);

/**
 *  @brief Obtain Current CPU Index
 *
 *  This method is invoked by RTEMS when it needs to know the index
 *  of the CPU it is executing on.
 *
 *  @return This method returns the current CPU index.
 */
int bsp_smp_processor_id(void) RTEMS_COMPILER_PURE_ATTRIBUTE;

/**
 *  @brief Make Request of Another CPU
 *
 *  This method is invoked by RTEMS when it needs to make a request
 *  of another CPU.  It should be implemented using some type of
 *  interprocessor interrupt.
 *
 *  @param [in] cpu is the target CPU for this request.
 *  @param [in] message is message to send
 */
void rtems_smp_send_message(
  int       cpu,
  uint32_t  message
);

/**
 *  @brief Generate a Interprocessor Broadcast Interrupt
 *
 *  This method is invoked when RTEMS wants to let all of the other
 *  CPUs know that it has sent them message.  CPUs not including 
 *  the originating CPU should receive the interrupt.

 *
 *  @note On CPUs without the capability to generate a broadcast 
 *        to all other CPUs interrupt, this can be implemented by
 *        a loop of sending interrupts to specific CPUs.
 */
void bsp_smp_broadcast_interrupt(void);

/**
 *  @brief Generate a Interprocessor Interrupt
 *
 *  This method is invoked by RTEMS to let @a cpu know that it
 *  has sent it a message.
 *
 *  @param [in] cpu is the recipient CPU
 */
void bsp_smp_interrupt_cpu(
  int cpu
);

/**
 *  @brief Obtain CPU Core Number
 *
 *  This method is invoked by RTEMS when it needs to know which core
 *  number it is executing on.  This is used when it needs to perform
 *  some action or bookkeeping and needs to distinguish itself from
 *  the other cores.  For example, it may need to realize it needs to
 *  preempt a thread on another node.
 *
 *  @return This method returns the Id of the current CPU core.
 */
int   bsp_smp_processor_id( void );

/**
 *  This method is invoked by @ref rtems_smp_secondary_cpu_initialize
 *  to allow the BSP to perform some intialization.  The @a cpu
 *  parameter indicates the secondary CPU that the code is executing on
 *  and is currently being initialized.
 *
 *  @note This is called by @ref rtems_smp_secondary_cpu_initialize.
 */
void bsp_smp_secondary_cpu_initialize(int cpu);

/**
 *  This method is the C entry point which secondary CPUs should
 *  arrange to call.  It performs OS initialization for the secondary
 *  CPU and coordinates bring it to a useful state.
 *
 *  @note This is provided by RTEMS.
 */
void rtems_smp_secondary_cpu_initialize(void);
 
/**
 *  This method is invoked by the BSP to initialize the per CPU structure
 *  for the specified @a cpu while it is bringing the secondary CPUs
 *  out of their reset state and into a useful state.
 *
 *  @param [in] cpu indicates the CPU whose per cpu structure is to
 *              be initialized.
 */
void rtems_smp_initialize_per_cpu(int cpu);

/**
 *  This is the method called by the BSP's interrupt handler
 *  to process the incoming interprocessor request. 
 */
void rtems_smp_process_interrupt(void);

void bsp_smp_wait_for(
  volatile unsigned int *address,
  unsigned int           desired,
  int                    maximum_usecs
);

#endif

#ifdef __cplusplus
}
#endif

#else
  #define bsp_smp_processor_id()  0 
#endif

#endif

/* end of include file */
