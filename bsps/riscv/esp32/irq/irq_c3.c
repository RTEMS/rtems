/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup riscv_interrupt
 *
 * @brief ESP32-C3 interrupt support.
 */

/*
 * Copyright (C) 2026 Kinsey Moore <wkmoore@gmail.com>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <bsp/fatal.h>
#include <bsp/irq-generic.h>

#include <rtems/score/cpu.h>
#include <rtems/score/percpu.h>
#include <rtems/score/riscv-utility.h>
#include <rtems/score/scheduler.h>

typedef struct peripheral_irq_map {
  rtems_vector_number peripheral_int;
  uint8_t             cpu_int;
} peripheral_irq_map_t;

static peripheral_irq_map_t irq_mappings[] = {
  { .peripheral_int = UHCI0_INTR, .cpu_int = 1 },
  /* Group software interrupts on interrupt 1 */
  { .peripheral_int = SW_INTR_0, .cpu_int = 1 },
  { .peripheral_int = SW_INTR_1, .cpu_int = 1 },
  { .peripheral_int = SW_INTR_2, .cpu_int = 1 },
  { .peripheral_int = SW_INTR_3, .cpu_int = 1 },
  { .peripheral_int = GPIO_PROCPU_INTR, .cpu_int = 2 },
  { .peripheral_int = GPSPI2_INTR_2, .cpu_int = 3 },
  { .peripheral_int = I2S_INTR, .cpu_int = 4 },
  { .peripheral_int = UART_INTR, .cpu_int = 5 },
  { .peripheral_int = UART1_INTR, .cpu_int = 6 },
  { .peripheral_int = LEDC_INTR, .cpu_int = 7 },
  /* Group EFUSE interrupt with LED controller */
  { .peripheral_int = EFUSE_INTR, .cpu_int = 7 },
  { .peripheral_int = TWAI_INTR, .cpu_int = 8 },
  { .peripheral_int = USB_SERIAL_JTAG_INTR, .cpu_int = 9 },
  { .peripheral_int = RTC_CNTL_INTR, .cpu_int = 10 },
  { .peripheral_int = RMT_INTR, .cpu_int = 11 },
  { .peripheral_int = I2C_EXT0_INTR, .cpu_int = 12 },
  { .peripheral_int = TG_T0_INTR, .cpu_int = 13 },
  { .peripheral_int = TG_WDT_INTR, .cpu_int = 14 },
  { .peripheral_int = TG1_T0_INTR, .cpu_int = 15 },
  /* Group watchdog interrupts together */
  { .peripheral_int = TG1_WDT_INTR, .cpu_int = 14 },
  { .peripheral_int = SYSTIMER_TARGET0_INTR, .cpu_int = 16 },
  { .peripheral_int = SYSTIMER_TARGET1_INTR, .cpu_int = 17 },
  { .peripheral_int = SYSTIMER_TARGET2_INTR, .cpu_int = 18 },
  { .peripheral_int = VDIGTAL_ADC_INTR, .cpu_int = 19 },
  { .peripheral_int = GDMA_CH0_INTR, .cpu_int = 20 },
  { .peripheral_int = GDMA_CH1_INTR, .cpu_int = 21 },
  { .peripheral_int = GDMA_CH2_INTR, .cpu_int = 22 },
  { .peripheral_int = RSA_INTR, .cpu_int = 23 },
  { .peripheral_int = AES_INTR, .cpu_int = 24 },
  { .peripheral_int = SHA_INTR, .cpu_int = 25 },
  { .peripheral_int = ASSIST_DEBUG_INTR, .cpu_int = 26 },
  { .peripheral_int = PMS_DMA_VIO_INTR, .cpu_int = 27 },
  { .peripheral_int = PMS_IBUS_VIO_INTR, .cpu_int = 28 },
  { .peripheral_int = PMS_DBUS_VIO_INTR, .cpu_int = 29 },
  { .peripheral_int = PMS_PERI_VIO_INTR, .cpu_int = 30 },
  { .peripheral_int = PMS_PERI_VIO_SIZE_INTR, .cpu_int = 31 }
};

/* mapping registers are indexed directly from the base address */

/* Bits here correspond directly to peripheral interrupt numbers */
/* low 32 bits */
#define INTERRUPT_CORE0_INTR_STATUS_0_REG 0xf8
/* remaining high bits */
#define INTERRUPT_CORE0_INTR_STATUS_1_REG 0xfc

#define INTERRUPT_CORE0_CLOCK_GATE_REG         0x100
#define INTERRUPT_CORE0_CPU_INT_ENABLE_REG     0x104
#define INTERRUPT_CORE0_CPU_INT_TYPE_REG       0x108
#define INTERRUPT_CORE0_CPU_INT_CLEAR_REG      0x10c
#define INTERRUPT_CORE0_CPU_INT_EIP_STATUS_REG 0x110

/* base register for interrupt priorities + N*4 for N in [1,31]*/
#define INTERRUPT_CORE0_CPU_INT_PRI_n_REG 0x118

#define INTERRUPT_CORE0_CPU_INT_THRESH_REG 0x194

#define MATRIX_REG( reg ) *( (volatile uint32_t *) ( INT_MATRIX_BASE + reg ) )

static uint64_t get_int_status( void )
{
  uint64_t total_set = MATRIX_REG( INTERRUPT_CORE0_INTR_STATUS_1_REG );

  total_set <<= 32;
  total_set |= MATRIX_REG( INTERRUPT_CORE0_INTR_STATUS_0_REG );

  return total_set;
}

static uint8_t int_periph_to_cpu( rtems_vector_number vector )
{
  size_t map_count = RTEMS_ARRAY_SIZE( irq_mappings );

  for ( size_t i = 0; i < map_count; i++ ) {
    if ( irq_mappings[ i ].peripheral_int == vector ) {
      return irq_mappings[ i ].cpu_int;
    }
  }

  return 0;
}

static rtems_vector_number get_active_interrupt( uint8_t cpu_vector )
{
  uint64_t total_set = get_int_status();

  for ( rtems_vector_number i = 0; i < sizeof( uint64_t ) * 8; i++ ) {
    if ( ( total_set & 0x1 ) != 0 && int_periph_to_cpu( i ) == cpu_vector ) {
      return i;
    }
    total_set >>= 1;
  }

  return 0;
}

static uintptr_t periph_int_to_map_reg( rtems_vector_number vector )
{
  return vector * sizeof( uint32_t );
}

static uintptr_t cpu_int_to_prio_reg( uint8_t cpu_int )
{
  return INTERRUPT_CORE0_CPU_INT_PRI_n_REG + cpu_int * sizeof( uint32_t );
}

void _RISCV_Interrupt_dispatch( uintptr_t mcause, Per_CPU_Control *cpu_self )
{
  (void) cpu_self;

  rtems_vector_number active;
  uint8_t             cpu_vector = mcause & 0x1f;

  /*
   * All interrupts on the ESP32-C3 are external except for exceptions which
   * are handled elsewhere
   */

  /* Check this CPU vector to see which peripheral has an active interrupt */
  active = get_active_interrupt( cpu_vector );

  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( active ) );

  bsp_interrupt_handler_dispatch_unchecked( active );

  /* mark interrupt serviced in matrix */
  bsp_interrupt_clear( active );
}

void bsp_interrupt_facility_initialize( void )
{
  /*
   * Enable all interrupt matrix output channels, all interrupt control is done
   * via mapping in the interrupt matrix
   */
  uint32_t cookie = riscv_interrupt_disable();

  /* Enable all cpu-side interrupts */
  MATRIX_REG( INTERRUPT_CORE0_CPU_INT_ENABLE_REG ) = 0xffffffffU;
  /* Set everything to level interrupts */
  MATRIX_REG( INTERRUPT_CORE0_CPU_INT_TYPE_REG ) = 0x0U;
  /*
   * Start at interrupt 1, interrupt 0 is invalid but included in the external
   * interrupt count
   */
  for ( uint8_t vec = 1; vec < RISCV_MAXIMUM_EXTERNAL_INTERRUPTS; vec++ ) {
    bsp_interrupt_set_priority( vec, 14 );
  }

  __asm__ volatile( "fence o, i"
                    :
                    :
                    : "memory" );
  riscv_interrupt_enable( cookie );

  /*
   * Clear all mappings. Start at interrupt 1, interrupt 0 is invalid but
   * included in the external interrupt count
   */
  for ( uint8_t vec = 1; vec < RISCV_MAXIMUM_EXTERNAL_INTERRUPTS; vec++ ) {
    bsp_interrupt_vector_disable( vec );
  }
}

bool bsp_interrupt_is_valid_vector( rtems_vector_number vector )
{
  if ( vector == 0 ) {
    return false;
  }

  return vector < (rtems_vector_number) RISCV_MAXIMUM_EXTERNAL_INTERRUPTS;
}

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  (void) vector;

  attributes->is_maskable = true;
  attributes->can_enable = true;
  attributes->maybe_enable = true;
  attributes->can_disable = true;
  attributes->maybe_disable = true;
  attributes->can_raise = true;
  attributes->cleared_by_acknowledge = true;
  attributes->can_get_affinity = false;
  attributes->can_set_affinity = false;
  /* priorities range from 1 to 15, but this is expected to be 0-based */
  attributes->maximum_priority = 14;
  attributes->can_get_priority = true;
  attributes->can_set_priority = true;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  uint64_t total_set;
  uint64_t bit = 1;

  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );
  bsp_interrupt_assert( pending != NULL );

  total_set = get_int_status();
  bit <<= vector;

  *pending = ( total_set & bit ) != 0;

  return RTEMS_SUCCESSFUL;
}

#define SYSTEM_CPU_INTR_FROM_CPU_0_REG 0x28
#define SYSTEM_CPU_INTR_FROM_CPU_1_REG 0x2C
#define SYSTEM_CPU_INTR_FROM_CPU_2_REG 0x30
#define SYSTEM_CPU_INTR_FROM_CPU_3_REG 0x34

#define SYSREG_REG( reg ) *( (volatile uint32_t *) ( SYSREG_BASE + reg ) )

rtems_status_code bsp_interrupt_raise( rtems_vector_number vector )
{
  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );

  switch ( vector ) {
    case SW_INTR_0:
      SYSREG_REG( SYSTEM_CPU_INTR_FROM_CPU_0_REG ) = 1;
      break;
    case SW_INTR_1:
      SYSREG_REG( SYSTEM_CPU_INTR_FROM_CPU_1_REG ) = 1;
      break;
    case SW_INTR_2:
      SYSREG_REG( SYSTEM_CPU_INTR_FROM_CPU_2_REG ) = 1;
      break;
    case SW_INTR_3:
      SYSREG_REG( SYSTEM_CPU_INTR_FROM_CPU_3_REG ) = 1;
      break;
    default:
      return RTEMS_UNSATISFIED;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_clear( rtems_vector_number vector )
{
  uint8_t cpu_int;

  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );

  cpu_int = int_periph_to_cpu( vector );

  MATRIX_REG( INTERRUPT_CORE0_CPU_INT_CLEAR_REG ) = 1U << cpu_int;
  MATRIX_REG( INTERRUPT_CORE0_CPU_INT_CLEAR_REG ) = 0U;

  /* If this is a software interrupt, clear it. */
  switch ( vector ) {
    case SW_INTR_0:
      SYSREG_REG( SYSTEM_CPU_INTR_FROM_CPU_0_REG ) = 0;
      break;
    case SW_INTR_1:
      SYSREG_REG( SYSTEM_CPU_INTR_FROM_CPU_1_REG ) = 0;
      break;
    case SW_INTR_2:
      SYSREG_REG( SYSTEM_CPU_INTR_FROM_CPU_2_REG ) = 0;
      break;
    case SW_INTR_3:
      SYSREG_REG( SYSTEM_CPU_INTR_FROM_CPU_3_REG ) = 0;
      break;
    default:
      break;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );
  bsp_interrupt_assert( enabled != NULL );

  /* any mapping in the register is enabled */
  *enabled = MATRIX_REG( periph_int_to_map_reg( vector ) ) != 0;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_enable( rtems_vector_number vector )
{
  size_t map_count = RTEMS_ARRAY_SIZE( irq_mappings );

  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );

  for ( size_t i = 0; i < map_count; i++ ) {
    if ( irq_mappings[ i ].peripheral_int == vector ) {
      /*
       * apply mapping, normal interrupt enable/modify/fence/disable doesn't
       * apply here because all interrupts are configured during init
       */
      MATRIX_REG(
        periph_int_to_map_reg( vector )
      ) = irq_mappings[ i ].cpu_int;
      return RTEMS_SUCCESSFUL;
    }
  }

  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_disable( rtems_vector_number vector )
{
  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );
  /* clear mapping to disable */
  MATRIX_REG( periph_int_to_map_reg( vector ) ) = 0;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_set_priority(
  rtems_vector_number vector,
  uint32_t            priority
)
{
  uint8_t cpu_int;

  /* adjust priority to the range the matrix uses */
  priority += 1;

  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );

  cpu_int = int_periph_to_cpu( vector );

  MATRIX_REG( cpu_int_to_prio_reg( cpu_int ) ) = priority;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_get_priority(
  rtems_vector_number vector,
  uint32_t           *priority
)
{
  uint8_t cpu_int;

  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );
  bsp_interrupt_assert( priority != NULL );

  cpu_int = int_periph_to_cpu( vector );
  *priority = MATRIX_REG( cpu_int_to_prio_reg( cpu_int ) );

  /* adjust priority to the range RTEMS uses */
  *priority -= 1;

  return RTEMS_SUCCESSFUL;
}
