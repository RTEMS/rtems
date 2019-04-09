/**
 * @file
 *
 * @ingroup RTEMSScoreISR
 *
 * @brief Data Related to the Management of Processor Interrupt Levels
 *
 * This include file contains all the constants and structures associated
 * with the management of processor interrupt levels.  This handler
 * supports interrupt critical sections, vectoring of user interrupt
 * handlers, nesting of interrupts, and manipulating interrupt levels.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_ISR_H
#define _RTEMS_SCORE_ISR_H

#include <rtems/score/isrlevel.h>

/**
 * @defgroup RTEMSScoreISR ISR Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief ISR Handler
 *
 * This handler encapsulates functionality which provides the foundation
 * ISR services used in all of the APIs supported by RTEMS.
 *
 * The ISR Nest level counter variable is maintained as part of the
 * per cpu data structure.
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
 * @brief Global symbol with a value equal to the configure interrupt stack size.
 *
 * This global symbol is defined by the application configuration option
 * CONFIGURE_INIT_TASK_STACK_SIZE via <rtems/confdefs.h>.
 */
RTEMS_DECLARE_GLOBAL_SYMBOL( _ISR_Stack_size );

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

/**
 * @brief Checks if an ISR in progress.
 *
 * This function returns true if the processor is currently servicing
 * and interrupt and false otherwise.   A return value of true indicates
 * that the caller is an interrupt service routine, NOT a thread.
 *
 * @retval true Returns true when called from an ISR.
 * @retval false Returns false when not called from an ISR.
 */
bool _ISR_Is_in_progress( void );

#ifdef __cplusplus
}
#endif

/** @} */

#endif
/* end of include file */
