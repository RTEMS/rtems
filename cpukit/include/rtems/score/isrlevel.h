/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreISR
 *
 * @brief This header file provides the ::ISR_Level related interfaces of the
 *   @ref RTEMSScoreISR.
 *
 * This header file exists to simplify include dependencies.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
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

#ifndef _RTEMS_SCORE_ISR_LEVEL_h
#define _RTEMS_SCORE_ISR_LEVEL_h

#include <rtems/score/cpu.h>
#include <rtems/score/assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreISR ISR Handler
 *
 * @{
 */

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
#define _ISR_Local_disable( _level ) \
  do { \
    _CPU_ISR_Disable( _level ); \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
  } while (0)

/**
 *  @brief Enables interrupts on this processor.
 *
 *  This macro restores the interrupt status on the processor with the
 *  interrupt level value obtained by _ISR_Local_disable().  It is used at the end of
 *  a critical section of code to enable interrupts so they can be processed
 *  again.
 *
 *  @param[in] _level The interrupt level previously obtained by
 *  _ISR_Local_disable().
 */
#define _ISR_Local_enable( _level ) \
  do { \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
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
 *  _ISR_Local_disable().
 */
#define _ISR_Local_flash( _level ) \
  do { \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
    _CPU_ISR_Flash( _level ); \
    RTEMS_COMPILER_MEMORY_BARRIER(); \
  } while (0)

/**
 * @brief Returns true if interrupts are enabled in the specified interrupt
 * level, otherwise returns false.
 *
 * @param[in] _level The ISR level.
 *
 * @retval true Interrupts are enabled in the interrupt level.
 * @retval false Otherwise.
 */
#define _ISR_Is_enabled( _level ) \
  _CPU_ISR_Is_enabled( _level )

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

/**
 * @brief Checks if an ISR in progress.
 *
 * This function returns true, if the processor is currently servicing
 * and interrupt, and false otherwise.   A return value of true indicates
 * that the caller is an interrupt service routine, **not** a thread.
 *
 * @return true Returns true, if called from within an ISR, otherwise false.
 */
bool _ISR_Is_in_progress( void );

/** @} */

#ifdef __cplusplus
}
#endif
#endif
