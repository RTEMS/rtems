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
#include <rtems/score/assert.h>

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
 *  @brief Disables interrupts on this processor.
 *
 *  This macro disables all interrupts on this processor so that a critical
 *  section of code is protected from concurrent access by interrupts of this
 *  processor.  Disabling of interrupts disables thread dispatching on the
 *  processor as well.
 *
 *  On SMP configurations other processors can enter such sections if not
 *  protected by other means.
 *
 *  @param[out] _level The argument @a _level will contain the previous
 *  interrupt mask level.
 */
#define _ISR_Disable( _level ) \
  do { \
    _CPU_ISR_Disable( _level ); \
    _Assert_Owner_of_giant(); \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
  } while (0)

/**
 *  @brief Enables interrupts on this processor.
 *
 *  This macro restores the interrupt status on the processor with the
 *  interrupt level value obtained by _ISR_Disable().  It is used at the end of
 *  a critical section of code to enable interrupts so they can be processed
 *  again.
 *
 *  @param[in] _level The interrupt level previously obtained by
 *  _ISR_Disable().
 */
#define _ISR_Enable( _level ) \
  do { \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
    _Assert_Owner_of_giant(); \
    _CPU_ISR_Enable( _level ); \
  } while (0)

/**
 *  @brief Temporarily enables interrupts on this processor.
 *
 *  This macro temporarily enables interrupts to the previous
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
 *  @param[in] _level The interrupt level previously obtained by
 *  _ISR_Disable().
 */
#define _ISR_Flash( _level ) \
  do { \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
    _Assert_Owner_of_giant(); \
    _CPU_ISR_Flash( _level ); \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
  } while (0)

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

#if defined( RTEMS_SMP )

#define _ISR_Disable_without_giant( _level ) \
  do { \
    _CPU_ISR_Disable( _level ); \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
  } while (0)

#define _ISR_Enable_without_giant( _level ) \
  do { \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
    _CPU_ISR_Enable( _level ); \
  } while (0)

#endif /* defined( RTEMS_SMP ) */

/**@}*/

#ifdef __cplusplus
}
#endif
#endif
