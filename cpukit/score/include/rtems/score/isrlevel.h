/**
 *  @file  rtems/score/isrlevel.h
 *
 *  @brief ISR Level Type
 *
 *  This include file defines the ISR Level type.  It exists to
 *  simplify include dependencies.  It is part of the ISR Handler.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_ISR_LEVEL_h
#define _RTEMS_SCORE_ISR_LEVEL_h

#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreISR ISR Handler
 *
 *  @ingroup Score
 *
 *  @addtogroup ScoreISR ISR Handler
 */
/**@{*/

/**
 *  The following type defines the control block used to manage
 *  the interrupt level portion of the status register.
 */
typedef uint32_t   ISR_Level;

/**
 *  @brief Disable interrupts on this core.
 *
 *  This routine disables all interrupts so that a critical section
 *  of code can be executing without being interrupted.
 *
 *  @retval The argument @a _level will contain the previous interrupt
 *          mask level.
 */
#define _ISR_Disable_on_this_core( _level ) \
  do { \
    _CPU_ISR_Disable( _level ); \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
  } while (0)

/**
 *  @brief Enable interrupts on this core.
 *
 *  This routine enables interrupts to the previous interrupt mask
 *  LEVEL.  It is used at the end of a critical section of code to
 *  enable interrupts so they can be processed again.
 *
 *  @param[in] _level contains the interrupt level mask level
 *             previously returned by @ref _ISR_Disable_on_this_core.
 */
#define _ISR_Enable_on_this_core( _level ) \
  do { \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
    _CPU_ISR_Enable( _level ); \
  } while (0)

/**
 *  @brief Temporarily enable interrupts on this core.
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
 *  @param[in] _level contains the interrupt level mask level
 *             previously returned by @ref _ISR_Disable_on_this_core.
 */
#define _ISR_Flash_on_this_core( _level ) \
  do { \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
    _CPU_ISR_Flash( _level ); \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
  } while (0)

#if defined( RTEMS_SMP )

/**
 *  @brief Enter interrupt critical section on SMP system.
 *
 *  This method is used to enter an interrupt critical section that
 *  is honored across all cores in an SMP system.
 *
 *  @retval This method returns the previous interrupt mask level.
 */
ISR_Level _ISR_SMP_Disable(void);

/**
 *  @brief Exit interrupt critical section on SMP system.
 *
 *  This method is used to exit an interrupt critical section that
 *  is honored across all cores in an SMP system.
 *
 *  @param[in] level contains the interrupt level mask level
 *             previously returned by @ref _ISR_SMP_Disable.
 */
void _ISR_SMP_Enable(ISR_Level level);

/**
 *  @brief Temporarily exit interrupt critical section on SMP system.
 *
 *  This method is used to temporarily exit an interrupt critical section
 *  that is honored across all cores in an SMP system.
 *
 *  @param[in] level contains the interrupt level mask level
 *             previously returned by @ref _ISR_SMP_Disable.
 */
void _ISR_SMP_Flash(ISR_Level level);

#endif /* defined( RTEMS_SMP ) */

/**
 *  @brief Enter interrupt disable critical section.
 *
 *  This routine enters an interrupt disable critical section.  When
 *  in an SMP configuration, this involves obtaining a spinlock to ensure
 *  that only one core is inside an interrupt disable critical section.
 *  When on a single core system, this only involves disabling local
 *  CPU interrupts.
 *
 *  @retval The argument @a _level will contain the previous interrupt
 *          mask level.
 */
#if defined( RTEMS_SMP )
  #define _ISR_Disable( _level ) \
    _level = _ISR_SMP_Disable();
#else
  #define _ISR_Disable( _level ) \
    _ISR_Disable_on_this_core( _level );
#endif

/**
 *  @brief Exits interrupt disable critical section.
 *
 *  This routine exits an interrupt disable critical section.  When
 *  in an SMP configuration, this involves releasing a spinlock.
 *  When on a single core system, this only involves disabling local
 *  CPU interrupts.
 *
 *  @retval The argument @a _level will contain the previous interrupt
 *          mask level.
 */
#if defined( RTEMS_SMP )
  #define _ISR_Enable( _level ) \
    _ISR_SMP_Enable( _level );
#else
  #define _ISR_Enable( _level ) \
    _ISR_Enable_on_this_core( _level );
#endif

/**
 *  @brief Temporarily exit interrupt disable critical section.
 *
 *  This routine is used to temporarily enable interrupts
 *  during a long critical section.  It is used in long sections of
 *  critical code when a point is reached at which interrupts can
 *  be temporarily enabled.  Deciding where to flash interrupts
 *  in a long critical section is often difficult and the point
 *  must be selected with care to ensure that the critical section
 *  properly protects itself.
 *
 *  @retval The argument @a _level will contain the previous interrupt
 *          mask level.
 */
#if defined( RTEMS_SMP )
  #define _ISR_Flash( _level ) \
    _ISR_SMP_Flash( _level );
#else
  #define _ISR_Flash( _level ) \
    _ISR_Flash_on_this_core( _level );
#endif

/**
 *  @brief Return current interrupt level.
 *
 *  This routine returns the current interrupt level.
 *
 *  LM32 Specific Information:
 *  XXX document implementation including references if appropriate
 *
 *  @retval This method returns the current level.
 */
#define _ISR_Get_level() \
        _CPU_ISR_Get_level()

/**
 *  @brief Set current interrupt level.
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

/**@}*/

#ifdef __cplusplus
}
#endif
#endif
