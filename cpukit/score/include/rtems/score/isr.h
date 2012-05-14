/**
 *  @file  rtems/score/isr.h
 *
 *  This include file contains all the constants and structures associated
 *  with the management of processor interrupt levels.  This handler
 *  supports interrupt critical sections, vectoring of user interrupt
 *  handlers, nesting of interrupts, and manipulating interrupt levels.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_ISR_H
#define _RTEMS_SCORE_ISR_H

#include <rtems/score/percpu.h>

/**
 *  @defgroup ScoreISR ISR Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which provides the foundation
 *  ISR services used in all of the APIs supported by RTEMS.
 *
 *  The ISR Nest level counter variable is maintained as part of the
 *  per cpu data structure.
 */
/**@{*/

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
#if (CPU_ISR_PASSES_FRAME_POINTER == 1)
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
 *  This constant promotes out the number of vectors truly supported by
 *  the current CPU being used.  This is usually the number of distinct vectors
 *  the cpu can vector.
 */
#define ISR_NUMBER_OF_VECTORS                CPU_INTERRUPT_NUMBER_OF_VECTORS

/**
 *  This constant promotes out the highest valid interrupt vector number.
 */
#define ISR_INTERRUPT_MAXIMUM_VECTOR_NUMBER  CPU_INTERRUPT_MAXIMUM_VECTOR_NUMBER

/**
 *  The following declares the Vector Table.  Application
 *  interrupt service routines are vectored by the ISR Handler via this table.
 */
SCORE_EXTERN ISR_Handler_entry *_ISR_Vector_table;
#endif

/**
 *  This routine performs the initialization necessary for this handler.
 */
void _ISR_Handler_initialization ( void );

/**
 *  @brief Disable Interrupts on This Core
 *
 *  This routine disables all interrupts so that a critical section
 *  of code can be executing without being interrupted.
 *
 *  @return The argument @a _level will contain the previous interrupt
 *          mask level.
 */
#define _ISR_Disable_on_this_core( _level ) \
  do { \
    _CPU_ISR_Disable( _level ); \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
  } while (0)

/**
 *  @brief Enable Interrupts on This Core
 *
 *  This routine enables interrupts to the previous interrupt mask
 *  LEVEL.  It is used at the end of a critical section of code to
 *  enable interrupts so they can be processed again.
 *
 *  @param[in] level contains the interrupt level mask level
 *             previously returned by @ref _ISR_Disable_on_this_core.
 */
#define _ISR_Enable_on_this_core( _level ) \
  do { \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
    _CPU_ISR_Enable( _level ); \
  } while (0)

/**
 *  @brief Temporarily Enable Interrupts on This Core
 *
 *  This routine temporarily enables interrupts to the previous
 *  interrupt mask level and then disables all interrupts so that
 *  the caller can continue into the second part of a critical
 *  section.
 *
 *  This routine is used to temporarily enable interrupts
 *  during a long critical section.  It is used in long sections of
 *  critical code when a point is reached at which interrupts can
 *  be temporarily enabled.  Deciding where to flash interrupts
 *  in a long critical section is often difficult and the point
 *  must be selected with care to ensure that the critical section
 *  properly protects itself.
 *
 *  @param[in] level contains the interrupt level mask level
 *             previously returned by @ref _ISR_Disable_on_this_core.
 */
#define _ISR_Flash_on_this_core( _level ) \
  do { \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
    _CPU_ISR_Flash( _level ); \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
  } while (0)

#if defined(RTEMS_SMP)

/**
 *  @brief Initialize SMP Interrupt Critical Section Support
 *
 *  This method initializes the variables required by the SMP implementation
 *  of interrupt critical section management.
 */
void _ISR_SMP_Initialize(void);

/**
 *  @brief Enter Interrupt Critical Section on SMP System
 *
 *  This method is used to enter an interrupt critical section that
 *  is honored across all cores in an SMP system.
 *
 *  @return This method returns the previous interrupt mask level.
 */
ISR_Level _ISR_SMP_Disable(void);

/**
 *  @brief Exit Interrupt Critical Section on SMP System
 *
 *  This method is used to exit an interrupt critical section that
 *  is honored across all cores in an SMP system.
 *
 *  @param[in] level contains the interrupt level mask level
 *             previously returned by @ref _ISR_SMP_Disable.
 */
void _ISR_SMP_Enable(ISR_Level level);

/**
 *  @brief Temporarily Exit Interrupt Critical Section on SMP System
 *
 *  This method is used to temporarily exit an interrupt critical section
 *  that is honored across all cores in an SMP system.
 *
 *  @param[in] level contains the interrupt level mask level
 *             previously returned by @ref _ISR_SMP_Disable.
 */
void _ISR_SMP_Flash(ISR_Level level);

/**
 *  @brief Enter SMP interrupt code
 *
 *  This method is used to enter the SMP interrupt section.
 *
 *  @return This method returns the isr level.
 */
int _ISR_SMP_Enter(void);

/**
 *  @brief Exit SMP interrupt code
 *
 *  This method is used to exit the SMP interrupt.
 *
 *  @return This method returns 0 on a simple return and returns 1 on a
 *  dispatching return.
 */
int _ISR_SMP_Exit(void);

#endif

/**
 *  @brief Enter Interrupt Disable Critical Section
 *
 *  This routine enters an interrupt disable critical section.  When
 *  in an SMP configuration, this involves obtaining a spinlock to ensure
 *  that only one core is inside an interrupt disable critical section.
 *  When on a single core system, this only involves disabling local
 *  CPU interrupts.
 *
 *  @return The argument @a _level will contain the previous interrupt
 *          mask level.
 */
#if defined(RTEMS_SMP)
  #define _ISR_Disable( _level ) \
    _level = _ISR_SMP_Disable();
#else
  #define _ISR_Disable( _level ) \
    _ISR_Disable_on_this_core( _level );
#endif
  
/**
 *  @brief Exits Interrupt Disable Critical Section
 *
 *  This routine exits an interrupt disable critical section.  When
 *  in an SMP configuration, this involves releasing a spinlock.
 *  When on a single core system, this only involves disabling local
 *  CPU interrupts.
 *
 *  @return The argument @a _level will contain the previous interrupt
 *          mask level.
 */
#if defined(RTEMS_SMP)
  #define _ISR_Enable( _level ) \
    _ISR_SMP_Enable( _level );
#else
  #define _ISR_Enable( _level ) \
    _ISR_Enable_on_this_core( _level );
#endif

/**
 *  @brief Temporarily Exit Interrupt Disable Critical Section
 *
 *  This routine is used to temporarily enable interrupts
 *  during a long critical section.  It is used in long sections of
 *  critical code when a point is reached at which interrupts can
 *  be temporarily enabled.  Deciding where to flash interrupts
 *  in a long critical section is often difficult and the point
 *  must be selected with care to ensure that the critical section
 *  properly protects itself.
 *
 *  @return The argument @a _level will contain the previous interrupt
 *          mask level.
 */
#if defined(RTEMS_SMP)
  #define _ISR_Flash( _level ) \
    _ISR_SMP_Flash( _level );
#else
  #define _ISR_Flash( _level ) \
    _ISR_Flash_on_this_core( _level );
#endif

/**
 *  @brief Install Interrupt Handler Vector
 *
 *  This routine installs new_handler as the interrupt service routine
 *  for the specified vector.  The previous interrupt service routine is
 *  returned as old_handler.
 *
 *  @param[in] _vector is the vector number
 *  @param[in] _new_handler is ISR handler to install
 *  @param[in] _old_handler is a pointer to a variable which will be set
 *             to the old handler
 *
 *  @return *_old_handler will be set to the old ISR handler
 */
#define _ISR_Install_vector( _vector, _new_handler, _old_handler ) \
  _CPU_ISR_install_vector( _vector, _new_handler, _old_handler )

/**
 *  @brief Return Current Interrupt Level
 *
 *  This routine returns the current interrupt level.
 *
 *  @return This method returns the current level.
 */
#define _ISR_Get_level() \
        _CPU_ISR_Get_level()

/**
 *  @brief Set Current Interrupt Level
 *
 *  This routine sets the current interrupt level to that specified
 *  by @a _new_level.  The new interrupt level is effective when the
 *  routine exits.
 *
 *  @param[in] _new_level contains the desired interrupt level.
 */
#define _ISR_Set_level( _new_level ) \
  do { \
    RTEMS_COMPILER_MEMORY_BARRIER();  \
    _CPU_ISR_Set_level( _new_level ); \
    RTEMS_COMPILER_MEMORY_BARRIER();  \
  } while (0)

/**
 *  @brief ISR Handler or Dispatcher
 *
 *  This routine is the interrupt dispatcher.  ALL interrupts
 *  are vectored to this routine so that minimal context can be saved
 *  and setup performed before the application's high-level language
 *  interrupt service routine is invoked.   After the application's
 *  interrupt service routine returns control to this routine, it
 *  will determine if a thread dispatch is necessary.  If so, it will
 *  ensure that the necessary thread scheduling operations are
 *  performed when the outermost interrupt service routine exits.
 *
 *  @note  Typically implemented in assembly language.
 */
void _ISR_Handler( void );

/**
 *  @brief ISR Wrapper for Thread Dispatcher
 *
 *  This routine provides a wrapper so that the routine
 *  @ref _Thread_Dispatch can be invoked when a reschedule is necessary
 *  at the end of the outermost interrupt service routine.  This
 *  wrapper is necessary to establish the processor context needed
 *  by _Thread_Dispatch and to save the processor context which is
 *  corrupted by _Thread_Dispatch.  This context typically consists
 *  of registers which are not preserved across routine invocations.
 *
 *  @note  Typically mplemented in assembly language.
 */
void _ISR_Dispatch( void );

/**
 *  @brief Is an ISR in Progress
 *
 *  This function returns true if the processor is currently servicing
 *  and interrupt and false otherwise.   A return value of true indicates
 *  that the caller is an interrupt service routine, NOT a thread.
 *
 *  @return This methods returns true when called from an ISR.
 */
#if (CPU_PROVIDES_ISR_IS_IN_PROGRESS == TRUE)
  bool _ISR_Is_in_progress( void );
#else
  #define _ISR_Is_in_progress() \
          (_ISR_Nest_level != 0)
#endif

#include <rtems/score/isr.inl>

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
