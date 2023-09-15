/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup RTEMSBSPsSPARCLEON3
 * @brief LEON3 BSP data types and macros
 */

/*  leon.h
 *
 *  LEON3 BSP data types and macros.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modified for LEON3 BSP.
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research.
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

#ifndef _INCLUDE_LEON_h
#define _INCLUDE_LEON_h

#include <rtems.h>
#include <amba.h>
#include <grlib/io.h>
#include <bsp/leon3.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LEON_INTERRUPT_EXTERNAL_1 5

#ifndef ASM
/*
 *  Trap Types for on-chip peripherals
 *
 *  Source: Table 8 - Interrupt Trap Type and Default Priority Assignments
 *
 *  NOTE: The priority level for each source corresponds to the least
 *        significant nibble of the trap type.
 */

#define LEON_TRAP_TYPE( _source ) SPARC_INTERRUPT_SOURCE_TO_TRAP( _source )

#define LEON_TRAP_SOURCE( _trap ) SPARC_INTERRUPT_TRAP_TO_SOURCE( _trap )

#define LEON_INT_TRAP( _trap ) SPARC_IS_INTERRUPT_TRAP( _trap )

/* /\* */
/*  *  This is used to manipulate the on-chip registers. */
/*  * */
/*  *  The following symbol must be defined in the linkcmds file and point */
/*  *  to the correct location. */
/*  *\/ */
/* Leon uses dynamic register mapping using amba configuration records */
/* LEON_Register_Map is obsolete */
/* extern LEON_Register_Map LEON_REG; */

#endif

/*
 *  The following defines the bits in Memory Configuration Register 1.
 */

#define LEON_MEMORY_CONFIGURATION_PROM_SIZE_MASK  0x0003C000

/*
 *  The following defines the bits in Memory Configuration Register 1.
 */

#define LEON_MEMORY_CONFIGURATION_RAM_SIZE_MASK  0x00001E00


/*
 *  The following defines the bits in the Timer Control Register.
 */

#define LEON_REG_TIMER_CONTROL_EN    0x00000001  /* 1 = enable counting */
                                              /* 0 = hold scalar and counter */
#define LEON_REG_TIMER_CONTROL_RL    0x00000002  /* 1 = reload at 0 */
                                              /* 0 = stop at 0 */
#define LEON_REG_TIMER_CONTROL_LD    0x00000004  /* 1 = load counter */
                                              /* 0 = no function */

/*
 *  The following defines the bits in the UART Control Registers.
 */

#define LEON_REG_UART_CONTROL_RTD  0x000000FF /* RX/TX data */

/*
 *  The following defines the bits in the LEON UART Status Register.
 */

#define LEON_REG_UART_STATUS_DR   0x00000001 /* Data Ready */
#define LEON_REG_UART_STATUS_TSE  0x00000002 /* TX Send Register Empty */
#define LEON_REG_UART_STATUS_THE  0x00000004 /* TX Hold Register Empty */
#define LEON_REG_UART_STATUS_BR   0x00000008 /* Break Error */
#define LEON_REG_UART_STATUS_OE   0x00000010 /* RX Overrun Error */
#define LEON_REG_UART_STATUS_PE   0x00000020 /* RX Parity Error */
#define LEON_REG_UART_STATUS_FE   0x00000040 /* RX Framing Error */
#define LEON_REG_UART_STATUS_TF   0x00000200 /* FIFO Full */
#define LEON_REG_UART_STATUS_ERR  0x00000078 /* Error Mask */

/*
 *  The following defines the bits in the LEON UART Control Register.
 */

#define LEON_REG_UART_CTRL_RE     0x00000001 /* Receiver enable */
#define LEON_REG_UART_CTRL_TE     0x00000002 /* Transmitter enable */
#define LEON_REG_UART_CTRL_RI     0x00000004 /* Receiver interrupt enable */
#define LEON_REG_UART_CTRL_TI     0x00000008 /* Transmitter interrupt enable */
#define LEON_REG_UART_CTRL_PS     0x00000010 /* Parity select */
#define LEON_REG_UART_CTRL_PE     0x00000020 /* Parity enable */
#define LEON_REG_UART_CTRL_FL     0x00000040 /* Flow control enable */
#define LEON_REG_UART_CTRL_LB     0x00000080 /* Loop Back enable */
#define LEON_REG_UART_CTRL_DB     0x00000800 /* Debug FIFO enable */
#define LEON_REG_UART_CTRL_SI     0x00004000 /* TX shift register empty IRQ enable */
#define LEON_REG_UART_CTRL_FA     0x80000000 /* FIFO Available */
#define LEON_REG_UART_CTRL_FA_BIT 31

/* Macros used for manipulating bits in LEON3 GP Timer Control Register */

#define LEON3_IRQMPSTATUS_CPUNR     28
#define LEON3_IRQMPSTATUS_BROADCAST 27


#ifndef ASM

/*
 *  Macros to manipulate the Interrupt Clear, Interrupt Force, Interrupt Mask,
 *  and the Interrupt Pending Registers.
 *
 *  NOTE: For operations which are not atomic, this code disables interrupts
 *        to guarantee there are no intervening accesses to the same register.
 *        The operations which read the register, modify the value and then
 *        store the result back are vulnerable.
 */

#define LEON_Clear_interrupt( _source ) \
  grlib_store_32(&LEON3_IrqCtrl_Regs->iclear, 1U << (_source))

#define LEON_Force_interrupt( _source ) \
  grlib_store_32(&LEON3_IrqCtrl_Regs->iforce0, 1U << (_source))

#define LEON_Enable_interrupt_broadcast( _source ) \
  do { \
    rtems_interrupt_lock_context _lock_context; \
    uint32_t _mask = 1U << ( _source ); \
    uint32_t _brdcst; \
    LEON3_IRQCTRL_ACQUIRE( &_lock_context ); \
    _brdcst = grlib_load_32(&LEON3_IrqCtrl_Regs->brdcst); \
    _brdcst |= _mask; \
    grlib_store_32(&LEON3_IrqCtrl_Regs->brdcst, _brdcst); \
    LEON3_IRQCTRL_RELEASE( &_lock_context ); \
  } while (0)

#define LEON_Disable_interrupt_broadcast( _source ) \
  do { \
    rtems_interrupt_lock_context _lock_context; \
    uint32_t _mask = 1U << ( _source ); \
    uint32_t _brdcst; \
    LEON3_IRQCTRL_ACQUIRE( &_lock_context ); \
    _brdcst = grlib_load_32(&LEON3_IrqCtrl_Regs->brdcst); \
    _brdcst &= ~_mask; \
    grlib_store_32(&LEON3_IrqCtrl_Regs->brdcst, _brdcst); \
    LEON3_IRQCTRL_RELEASE( &_lock_context ); \
  } while (0)

#define LEON_Is_interrupt_pending( _source ) \
  (grlib_load_32(&LEON3_IrqCtrl_Regs->ipend) & (1U << (_source)))

#define LEON_Cpu_Is_interrupt_masked( _source, _cpu ) \
     (!(grlib_load_32(&LEON3_IrqCtrl_Regs->pimask[_cpu]) & (1U << (_source))))

#define LEON_Cpu_Mask_interrupt( _source, _cpu ) \
  do { \
    rtems_interrupt_lock_context _lock_context; \
    uint32_t _pimask; \
    LEON3_IRQCTRL_ACQUIRE( &_lock_context ); \
    _pimask = grlib_load_32(&LEON3_IrqCtrl_Regs->pimask[_cpu ]); \
    _pimask &= ~(1U << (_source)); \
    grlib_store_32(&LEON3_IrqCtrl_Regs->pimask[_cpu ], _pimask); \
    LEON3_IRQCTRL_RELEASE( &_lock_context ); \
  } while (0)

#define LEON_Cpu_Unmask_interrupt( _source, _cpu ) \
  do { \
    rtems_interrupt_lock_context _lock_context; \
    uint32_t _pimask; \
    LEON3_IRQCTRL_ACQUIRE( &_lock_context ); \
    _pimask = grlib_load_32(&LEON3_IrqCtrl_Regs->pimask[_cpu ]); \
    _pimask |= 1U << (_source); \
    grlib_store_32(&LEON3_IrqCtrl_Regs->pimask[_cpu ], _pimask); \
    LEON3_IRQCTRL_RELEASE( &_lock_context ); \
  } while (0)

#define LEON_Cpu_Disable_interrupt( _source, _previous, _cpu ) \
  do { \
    rtems_interrupt_lock_context _lock_context; \
    uint32_t _mask = 1U << (_source); \
    LEON3_IRQCTRL_ACQUIRE( &_lock_context ); \
    (_previous) = grlib_load_32(&LEON3_IrqCtrl_Regs->pimask[_cpu ]); \
    grlib_store_32(&LEON3_IrqCtrl_Regs->pimask[_cpu ], (_previous) & ~_mask); \
    LEON3_IRQCTRL_RELEASE( &_lock_context ); \
    (_previous) &= _mask; \
  } while (0)

#define LEON_Cpu_Restore_interrupt( _source, _previous, _cpu ) \
  do { \
    rtems_interrupt_lock_context _lock_context; \
    uint32_t _pimask; \
    LEON3_IRQCTRL_ACQUIRE( &_lock_context ); \
    _pimask = grlib_load_32(&LEON3_IrqCtrl_Regs->pimask[_cpu ]); \
    _pimask &= ~(1U << (_source)); \
    _pimask |= _previous; \
    grlib_store_32(&LEON3_IrqCtrl_Regs->pimask[_cpu ], _pimask); \
    LEON3_IRQCTRL_RELEASE( &_lock_context ); \
  } while (0)

/* Map single-cpu operations to local CPU */
#define LEON_Is_interrupt_masked( _source ) \
  LEON_Cpu_Is_interrupt_masked(_source, _LEON3_Get_current_processor())

#define LEON_Mask_interrupt(_source) \
  LEON_Cpu_Mask_interrupt(_source, _LEON3_Get_current_processor())

#define LEON_Unmask_interrupt(_source) \
  LEON_Cpu_Unmask_interrupt(_source, _LEON3_Get_current_processor())

#define LEON_Disable_interrupt(_source, _previous) \
  LEON_Cpu_Disable_interrupt(_source, _previous, _LEON3_Get_current_processor())

#define LEON_Restore_interrupt(_source, _previous) \
  LEON_Cpu_Restore_interrupt(_source, _previous, _LEON3_Get_current_processor())

/* Make all SPARC BSPs have common macros for interrupt handling */
#define BSP_Clear_interrupt(_source) LEON_Clear_interrupt(_source)
#define BSP_Force_interrupt(_source) LEON_Force_interrupt(_source)
#define BSP_Is_interrupt_pending(_source) LEON_Is_interrupt_pending(_source)
#define BSP_Is_interrupt_masked(_source) LEON_Is_interrupt_masked(_source)
#define BSP_Unmask_interrupt(_source) LEON_Unmask_interrupt(_source)
#define BSP_Mask_interrupt(_source) LEON_Mask_interrupt(_source)
#define BSP_Disable_interrupt(_source, _previous) \
        LEON_Disable_interrupt(_source, _prev)
#define BSP_Restore_interrupt(_source, _previous) \
        LEON_Restore_interrupt(_source, _previous)

/* Make all SPARC BSPs have common macros for interrupt handling on any CPU */
#define BSP_Cpu_Is_interrupt_masked(_source, _cpu) \
        LEON_Cpu_Is_interrupt_masked(_source, _cpu)
#define BSP_Cpu_Unmask_interrupt(_source, _cpu) \
        LEON_Cpu_Unmask_interrupt(_source, _cpu)
#define BSP_Cpu_Mask_interrupt(_source, _cpu) \
        LEON_Cpu_Mask_interrupt(_source, _cpu)
#define BSP_Cpu_Disable_interrupt(_source, _previous, _cpu) \
        LEON_Cpu_Disable_interrupt(_source, _prev, _cpu)
#define BSP_Cpu_Restore_interrupt(_source, _previous, _cpu) \
        LEON_Cpu_Restore_interrupt(_source, _previous, _cpu)

/*
 *  Each timer control register is organized as follows:
 *
 *    D0 - Enable
 *          1 = enable counting
 *          0 = hold scaler and counter
 *
 *    D1 - Counter Reload
 *          1 = reload counter at zero and restart
 *          0 = stop counter at zero
 *
 *    D2 - Counter Load
 *          1 = load counter with preset value
 *          0 = no function
 *
 */

#define LEON_REG_TIMER_COUNTER_RELOAD_AT_ZERO     0x00000002
#define LEON_REG_TIMER_COUNTER_STOP_AT_ZERO       0x00000000

#define LEON_REG_TIMER_COUNTER_LOAD_COUNTER       0x00000004

#define LEON_REG_TIMER_COUNTER_ENABLE_COUNTING    0x00000001
#define LEON_REG_TIMER_COUNTER_DISABLE_COUNTING   0x00000000

#define LEON_REG_TIMER_COUNTER_RELOAD_MASK        0x00000002
#define LEON_REG_TIMER_COUNTER_ENABLE_MASK        0x00000001

#define LEON_REG_TIMER_COUNTER_DEFINED_MASK       0x00000003
#define LEON_REG_TIMER_COUNTER_CURRENT_MODE_MASK  0x00000003

/* Load 32-bit word by forcing a cache-miss */
static inline unsigned int leon_r32_no_cache(uintptr_t addr)
{
  unsigned int tmp;
  __asm__ volatile (" lda [%1] 1, %0\n" : "=r"(tmp) : "r"(addr));
  return tmp;
}

/* Let user override which on-chip APBUART will be debug UART
 * 0 = Default APBUART. On MP system CPU0=APBUART0, CPU1=APBUART1...
 * 1 = APBUART[0]
 * 2 = APBUART[1]
 * 3 = APBUART[2]
 * ...
 */
extern int syscon_uart_index;

#if !defined(LEON3_APBUART_BASE)
/* Let user override which on-chip APBUART will be debug UART
 * 0 = Default APBUART. On MP system CPU0=APBUART0, CPU1=APBUART1...
 * 1 = APBUART[0]
 * 2 = APBUART[1]
 * 3 = APBUART[2]
 * ...
 */
extern int leon3_debug_uart_index;
#endif

#endif /* !ASM */

#ifdef __cplusplus
}
#endif

#endif /* !_INCLUDE_LEON_h */
/* end of include file */

