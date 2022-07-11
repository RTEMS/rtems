/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsNios2ISS
 *
 * @brief Global BSP definitions.
 */

/*  bsp.h
 *
 *  This include file contains all board IO definitions.
 *
 *  XXX : put yours in here
 *
 *  COPYRIGHT (c) 1989-1999.
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

#ifndef LIBBSP_NIOS2_NIOS2_ISS_BSP_H
#define LIBBSP_NIOS2_NIOS2_ISS_BSP_H

/**
 * @defgroup RTEMSBSPsNios2ISS ISS
 *
 * @ingroup RTEMSBSPsNios2
 *
 * @brief ISS Board Support Package.
 *
 * @{
 */

#include <stdint.h>
#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================ */

/* SOPC-specific Constants */

#define SYSTEM_BUS_WIDTH 32

#define JTAG_UART_BASE 0x08000000
#define JTAG_UART_IRQ 2

#define CLOCK_BASE 0x08001000
#define CLOCK_FREQ 50000000
#define CLOCK_VECTOR 1

#define TIMER_BASE 0x08002000
#define TIMER_FREQ 50000000
#define TIMER_VECTOR 3

/* ============================================ */

#define NIOS2_BYPASS_CACHE ((uint32_t)0x80000000ul)
#define NIOS2_IO_BASE(x) ( (void*) ((uint32_t)x | NIOS2_BYPASS_CACHE ) )
#define NIOS2_IENABLE(x) do{ __builtin_wrctl(3,__builtin_rdctl(3)|x);}while(0)
#define NIOS2_IRQ_ENABLE(x) do {__builtin_wrctl(3,__builtin_rdctl(3)|x);} while(0)

/* ============================================ */
/* TODO: Put this in an external header file */

#ifndef SYSTEM_BUS_WIDTH
#error SYSTEM_BUS_WIDTH is undefined
#endif

#if SYSTEM_BUS_WIDTH != 32
#error Only SYSTEM_BUS_WIDTH 32 is supported
#endif

typedef struct
{
  volatile uint32_t status;
  volatile uint32_t control;
  volatile uint32_t period_lo;
  volatile uint32_t period_hi;
  volatile uint32_t snap_lo;
  volatile uint32_t snap_hi;
}
altera_avalon_timer_regs;

#define ALTERA_AVALON_TIMER_STATUS_TO_MSK             (0x1)
#define ALTERA_AVALON_TIMER_STATUS_TO_OFST            (0)
#define ALTERA_AVALON_TIMER_STATUS_RUN_MSK            (0x2)
#define ALTERA_AVALON_TIMER_STATUS_RUN_OFST           (1)

#define ALTERA_AVALON_TIMER_CONTROL_ITO_MSK           (0x1)
#define ALTERA_AVALON_TIMER_CONTROL_ITO_OFST          (0)
#define ALTERA_AVALON_TIMER_CONTROL_CONT_MSK          (0x2)
#define ALTERA_AVALON_TIMER_CONTROL_CONT_OFST         (1)
#define ALTERA_AVALON_TIMER_CONTROL_START_MSK         (0x4)
#define ALTERA_AVALON_TIMER_CONTROL_START_OFST        (2)
#define ALTERA_AVALON_TIMER_CONTROL_STOP_MSK          (0x8)
#define ALTERA_AVALON_TIMER_CONTROL_STOP_OFST         (3)

typedef struct
{
  volatile uint32_t data;
  volatile uint32_t control;
}
altera_avalon_jtag_uart_regs;

#define ALTERA_AVALON_JTAG_UART_DATA_DATA_MSK             (0x000000FFu)
#define ALTERA_AVALON_JTAG_UART_DATA_DATA_OFST            (0)
#define ALTERA_AVALON_JTAG_UART_DATA_RVALID_MSK           (0x00008000u)
#define ALTERA_AVALON_JTAG_UART_DATA_RVALID_OFST          (15)
#define ALTERA_AVALON_JTAG_UART_DATA_RAVAIL_MSK           (0xFFFF0000u)
#define ALTERA_AVALON_JTAG_UART_DATA_RAVAIL_OFST          (16)

#define ALTERA_AVALON_JTAG_UART_CONTROL_RE_MSK            (0x00000001u)
#define ALTERA_AVALON_JTAG_UART_CONTROL_RE_OFST           (0)
#define ALTERA_AVALON_JTAG_UART_CONTROL_WE_MSK            (0x00000002u)
#define ALTERA_AVALON_JTAG_UART_CONTROL_WE_OFST           (1)
#define ALTERA_AVALON_JTAG_UART_CONTROL_RI_MSK            (0x00000100u)
#define ALTERA_AVALON_JTAG_UART_CONTROL_RI_OFST           (8)
#define ALTERA_AVALON_JTAG_UART_CONTROL_WI_MSK            (0x00000200u)
#define ALTERA_AVALON_JTAG_UART_CONTROL_WI_OFST           (9)
#define ALTERA_AVALON_JTAG_UART_CONTROL_AC_MSK            (0x00000400u)
#define ALTERA_AVALON_JTAG_UART_CONTROL_AC_OFST           (10)
#define ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_MSK        (0xFFFF0000u)
#define ALTERA_AVALON_JTAG_UART_CONTROL_WSPACE_OFST       (16)

/* ============================================ */

/* functions */

rtems_isr_entry set_vector(                     /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
);

#ifdef __cplusplus
}
#endif

/** @} */

#endif
/* end of include file */
