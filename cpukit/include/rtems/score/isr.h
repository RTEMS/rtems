/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreISR
 *
 * @brief This header file provides the main interfaces of the
 *   @ref RTEMSScoreISR.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_SCORE_ISR_H
#define _RTEMS_SCORE_ISR_H

#include <rtems/score/isrlevel.h>

/**
 * @defgroup RTEMSScoreISR ISR Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the ISR Handler implementation.
 *
 * This handler encapsulates functionality which provides the foundation
 * ISR services used in all of the APIs supported by RTEMS.  This handler
 * supports interrupt critical sections, vectoring of user interrupt handlers,
 * nesting of interrupts, and manipulating interrupt levels.
 *
 * The ::_ISR_Nest_level variable is maintained by @ref RTEMSScorePerCPU.
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  The following type defines the type used to manage the vectors.
 */
typedef uint32_t   ISR_Vector_number;

/**
 *  Return type for ISR Handler
 */
typedef void ISR_Handler;

#if (CPU_SIMPLE_VECTORED_INTERRUPTS == FALSE)

typedef void * ISR_Handler_entry;

#else
/**
 *  Pointer to an ISR Handler
 */
#if (CPU_ISR_PASSES_FRAME_POINTER == TRUE)
typedef ISR_Handler ( *ISR_Handler_entry )(
                 ISR_Vector_number,
                 CPU_Interrupt_frame *
             );
#else
typedef ISR_Handler ( *ISR_Handler_entry )(
                 ISR_Vector_number
             );
#endif

/**
 *  The following declares the Vector Table.  Application
 *  interrupt service routines are vectored by the ISR Handler via this table.
 */
extern ISR_Handler_entry _ISR_Vector_table[ CPU_INTERRUPT_NUMBER_OF_VECTORS ];
#endif

/**
 * @brief Provides the configured interrupt stack size through an address.
 *
 * The address of this global symbol is equal to the configured interrupt stack
 * size.  The address of this symbol has an arbitrary value an may not be
 * representable in the code model used by the compiler.
 *
 * This global symbol is defined by the application configuration option
 * CONFIGURE_INIT_TASK_STACK_SIZE via <rtems/confdefs.h>.
 */
RTEMS_DECLARE_GLOBAL_SYMBOL( _ISR_Stack_size );

/**
 * @brief Provides the configured interrupt stack size through an object.
 *
 * This object is provided to avoid issues with the _ISR_Stack_size symbol
 * address and the code model used by the compiler.
 */
extern const char * const volatile _ISR_Stack_size_object;

/**
 * @brief The interrupt stack area begin.
 *
 * The interrupt stack area is defined by the application configuration via
 * <rtems/confdefs.h>.  The size of the area depends on
 * CONFIGURE_INIT_TASK_STACK_SIZE and CONFIGURE_MAXIMUM_PROCESSORS.
 */
extern char _ISR_Stack_area_begin[];

/**
 * @brief The interrupt stack area end.
 *
 * The interrupt stack area is defined by the application configuration via
 * <rtems/confdefs.h>.  The size of the area depends on
 * CONFIGURE_INIT_TASK_STACK_SIZE and CONFIGURE_MAXIMUM_PROCESSORS.
 */
extern const char _ISR_Stack_area_end[];

/**
 * @brief Initializes the ISR handler.
 *
 * This routine performs the initialization necessary for the ISR handler.
 */
void _ISR_Handler_initialization ( void );

/**
 *  @brief Install interrupt handler vector.
 *
 *  This routine installs new_handler as the interrupt service routine
 *  for the specified vector.  The previous interrupt service routine is
 *  returned as old_handler.
 *
 *  LM32 Specific Information:
 *  XXX document implementation including references if appropriate
 *
 *  @param[in] _vector is the vector number
 *  @param[in] _new_handler is ISR handler to install
 *  @param[in] _old_handler is a pointer to a variable which will be set
 *             to the old handler
 *
 *  @retval *_old_handler will be set to the old ISR handler
 */
#define _ISR_Install_vector( _vector, _new_handler, _old_handler ) \
  _CPU_ISR_install_vector( _vector, _new_handler, _old_handler )

/**
 * @brief ISR interrupt dispatcher.
 *
 * This routine is the interrupt dispatcher.  ALL interrupts
 * are vectored to this routine so that minimal context can be saved
 * and setup performed before the application's high-level language
 * interrupt service routine is invoked.   After the application's
 * interrupt service routine returns control to this routine, it
 * will determine if a thread dispatch is necessary.  If so, it will
 * ensure that the necessary thread scheduling operations are
 * performed when the outermost interrupt service routine exits.
 *
 * @note  Typically implemented in assembly language.
 */
void _ISR_Handler( void );

#ifdef __cplusplus
}
#endif

/** @} */

#endif
/* end of include file */
