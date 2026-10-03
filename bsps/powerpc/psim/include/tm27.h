/* SPDX-License-Identifier: GPL-2.0+-with-RTEMS-exception */

/*
 * @file
 * @ingroup powerpc_psim
 * @brief Implementations for interrupt mechanisms for Time Test 27
 */

/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_TMTEST27
#error "This is an RTEMS internal file you must not include directly."
static inline rtems_status_code _TM27_Raise_alternative( void )
{
  return rtems_interrupt_raise( TM27_INTERRUPT_VECTOR_ALTERNATIVE );
}

static inline rtems_status_code _TM27_Clear_alternative( void )
{
  return rtems_interrupt_clear( TM27_INTERRUPT_VECTOR_ALTERNATIVE );
}

#endif

#ifndef __tm27_h
#define __tm27_h

#include <bsp/irq.h>

/*
 *  Stuff for Time Test 27
 */

#define MUST_WAIT_FOR_INTERRUPT 1

#define TM27_INTERRUPT_VECTOR_DEFAULT BSP_OPENPIC_IPI_LOWEST_OFFSET
#define TM27_INTERRUPT_VECTOR_ALTERNATIVE ( BSP_OPENPIC_IPI_LOWEST_OFFSET + 1 )

static rtems_interrupt_entry psim_tm27_interrupt_entry;

static inline void Install_tm27_vector( rtems_interrupt_handler handler )
{
  rtems_interrupt_entry_initialize(
    &psim_tm27_interrupt_entry,
    handler,
    NULL,
    "tm27"
  );
  (void) rtems_interrupt_entry_install(
    TM27_INTERRUPT_VECTOR_DEFAULT,
    RTEMS_INTERRUPT_SHARED,
    &psim_tm27_interrupt_entry
  );
}

static inline void Cause_tm27_intr( void )
{
  (void) rtems_interrupt_raise( TM27_INTERRUPT_VECTOR_DEFAULT );
}

/*
 * The acknowledge of an interprocessor interrupt clears it, so there is
 * nothing to clear.
 */
static inline void Clear_tm27_intr( void )
{
}

#define Lower_tm27_intr()              \
  do {                                 \
    uint32_t _msr = 0;                 \
    _ISR_Set_level( 0 );               \
    __asm__ volatile( "mfmsr %0 ;"     \
                      : "=r"( _msr )   \
                      : "r"( _msr ) ); \
    _msr |= 0x8002;                    \
    __asm__ volatile( "mtmsr %0 ;"     \
                      : "=r"( _msr )   \
                      : "r"( _msr ) ); \
  } while ( 0 )

static inline rtems_status_code _TM27_Raise_alternative( void )
{
  return rtems_interrupt_raise( TM27_INTERRUPT_VECTOR_ALTERNATIVE );
}

static inline rtems_status_code _TM27_Clear_alternative( void )
{
  return rtems_interrupt_clear( TM27_INTERRUPT_VECTOR_ALTERNATIVE );
}

#endif
