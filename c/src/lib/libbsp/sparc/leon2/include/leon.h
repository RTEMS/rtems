/*  erc32.h
 *
 *  This include file contains information pertaining to the LEON-1.
 *  The LEON-1 is a custom SPARC V7 implementation.
 *  This CPU has a number of on-board peripherals and
 *  was developed by the European Space Agency to target space applications.
 *
 *  NOTE:  Other than where absolutely required, this version currently
 *         supports only the peripherals and bits used by the basic board
 *         support package. This includes at least significant pieces of
 *         the following items:
 *
 *           + UART Channels A and B
 *           + Real Time Clock
 *           + Memory Control Register
 *           + Interrupt Control
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Ported to LEON implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space
 *  Agency (ESA).
 *
 *  LEON modifications of respective RTEMS file: COPYRIGHT (c) 1995.
 *  European Space Agency.
 */

#ifndef _INCLUDE_LEON_h
#define _INCLUDE_LEON_h

#include <rtems/score/sparc.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Interrupt Sources
 *
 *  The interrupt source numbers directly map to the trap type and to
 *  the bits used in the Interrupt Clear, Interrupt Force, Interrupt Mask,
 *  and the Interrupt Pending Registers.
 */

#define LEON_INTERRUPT_CORRECTABLE_MEMORY_ERROR  1
#define LEON_INTERRUPT_UART_2_RX_TX              2
#define LEON_INTERRUPT_UART_1_RX_TX              3
#define LEON_INTERRUPT_EXTERNAL_0                4
#define LEON_INTERRUPT_EXTERNAL_1                5
#define LEON_INTERRUPT_EXTERNAL_2                6
#define LEON_INTERRUPT_EXTERNAL_3                7
#define LEON_INTERRUPT_TIMER1                    8
#define LEON_INTERRUPT_TIMER2                    9
#define LEON_INTERRUPT_EMPTY1                    10
#define LEON_INTERRUPT_EMPTY2                    11
#define LEON_INTERRUPT_EMPTY3                    12
#define LEON_INTERRUPT_EMPTY4                    13
#define LEON_INTERRUPT_EMPTY5                    14
#define LEON_INTERRUPT_EMPTY6                    15

#ifndef ASM

/*
 *  Trap Types for on-chip peripherals
 *
 *  Source: Table 8 - Interrupt Trap Type and Default Priority Assignments
 *
 *  NOTE: The priority level for each source corresponds to the least
 *        significant nibble of the trap type.
 */

#define LEON_TRAP_TYPE( _source ) SPARC_ASYNCHRONOUS_TRAP((_source) + 0x10)

#define LEON_TRAP_SOURCE( _trap ) ((_trap) - 0x10)

#define LEON_INT_TRAP( _trap ) \
  ( (_trap) >= LEON_TRAP_TYPE( LEON_INTERRUPT_CORRECTABLE_MEMORY_ERROR ) && \
    (_trap) <= LEON_TRAP_TYPE( LEON_INTERRUPT_EMPTY6 ) )

/*
 *  Structure for LEON memory mapped registers.
 *
 *  Source: Section 6.1 - On-chip registers
 *
 *  NOTE:  There is only one of these structures per CPU, its base address
 *         is 0x80000000, and the variable LEON_REG is placed there by the
 *         linkcmds file.
 */

typedef struct {
	volatile unsigned int Memory_Config_1;
	volatile unsigned int Memory_Config_2;
	volatile unsigned int Edac_Control;
	volatile unsigned int Failed_Address;
	volatile unsigned int Memory_Status;
	volatile unsigned int Cache_Control;
	volatile unsigned int Power_Down;
	volatile unsigned int Write_Protection_1;
	volatile unsigned int Write_Protection_2;
	volatile unsigned int Leon_Configuration;
	volatile unsigned int dummy2;
	volatile unsigned int dummy3;
	volatile unsigned int dummy4;
	volatile unsigned int dummy5;
	volatile unsigned int dummy6;
	volatile unsigned int dummy7;
	volatile unsigned int Timer_Counter_1;
	volatile unsigned int Timer_Reload_1;
	volatile unsigned int Timer_Control_1;
	volatile unsigned int Watchdog;
	volatile unsigned int Timer_Counter_2;
	volatile unsigned int Timer_Reload_2;
	volatile unsigned int Timer_Control_2;
	volatile unsigned int dummy8;
	volatile unsigned int Scaler_Counter;
	volatile unsigned int Scaler_Reload;
	volatile unsigned int dummy9;
	volatile unsigned int dummy10;
	volatile unsigned int UART_Channel_1;
	volatile unsigned int UART_Status_1;
	volatile unsigned int UART_Control_1;
	volatile unsigned int UART_Scaler_1;
	volatile unsigned int UART_Channel_2;
	volatile unsigned int UART_Status_2;
	volatile unsigned int UART_Control_2;
	volatile unsigned int UART_Scaler_2;
	volatile unsigned int Interrupt_Mask;
	volatile unsigned int Interrupt_Pending;
	volatile unsigned int Interrupt_Force;
	volatile unsigned int Interrupt_Clear;
	volatile unsigned int PIO_Data;
	volatile unsigned int PIO_Direction;
	volatile unsigned int PIO_Interrupt;
} LEON_Register_Map;

#endif

/*
 *  The following constants are intended to be used ONLY in assembly
 *  language files.
 *
 *  NOTE:  The intended style of usage is to load the address of LEON REGS
 *         into a register and then use these as displacements from
 *         that register.
 */

#ifdef ASM

#define  LEON_REG_MEMCFG1_OFFSET                                  0x00
#define  LEON_REG_MEMCFG2_OFFSET                                  0x04
#define  LEON_REG_EDACCTRL_OFFSET                                 0x08
#define  LEON_REG_FAILADDR_OFFSET                                 0x0C
#define  LEON_REG_MEMSTATUS_OFFSET                                0x10
#define  LEON_REG_CACHECTRL_OFFSET                                0x14
#define  LEON_REG_POWERDOWN_OFFSET                                0x18
#define  LEON_REG_WRITEPROT1_OFFSET                               0x1C
#define  LEON_REG_WRITEPROT2_OFFSET                               0x20
#define  LEON_REG_LEONCONF_OFFSET                                 0x24
#define  LEON_REG_UNIMPLEMENTED_2_OFFSET                          0x28
#define  LEON_REG_UNIMPLEMENTED_3_OFFSET                          0x2C
#define  LEON_REG_UNIMPLEMENTED_4_OFFSET                          0x30
#define  LEON_REG_UNIMPLEMENTED_5_OFFSET                          0x34
#define  LEON_REG_UNIMPLEMENTED_6_OFFSET                          0x38
#define  LEON_REG_UNIMPLEMENTED_7_OFFSET                          0x3C
#define  LEON_REG_TIMERCNT1_OFFSET                                0x40
#define  LEON_REG_TIMERLOAD1_OFFSET                               0x44
#define  LEON_REG_TIMERCTRL1_OFFSET                               0x48
#define  LEON_REG_WDOG_OFFSET                                     0x4C
#define  LEON_REG_TIMERCNT2_OFFSET                                0x50
#define  LEON_REG_TIMERLOAD2_OFFSET                               0x54
#define  LEON_REG_TIMERCTRL2_OFFSET                               0x58
#define  LEON_REG_UNIMPLEMENTED_8_OFFSET                          0x5C
#define  LEON_REG_SCALERCNT_OFFSET                                0x60
#define  LEON_REG_SCALER_LOAD_OFFSET                              0x64
#define  LEON_REG_UNIMPLEMENTED_9_OFFSET                          0x68
#define  LEON_REG_UNIMPLEMENTED_10_OFFSET                         0x6C
#define  LEON_REG_UARTDATA1_OFFSET                                0x70
#define  LEON_REG_UARTSTATUS1_OFFSET                              0x74
#define  LEON_REG_UARTCTRL1_OFFSET                                0x78
#define  LEON_REG_UARTSCALER1_OFFSET                              0x7C
#define  LEON_REG_UARTDATA2_OFFSET                                0x80
#define  LEON_REG_UARTSTATUS2_OFFSET                              0x84
#define  LEON_REG_UARTCTRL2_OFFSET                                0x88
#define  LEON_REG_UARTSCALER2_OFFSET                              0x8C
#define  LEON_REG_IRQMASK_OFFSET                                  0x90
#define  LEON_REG_IRQPEND_OFFSET                                  0x94
#define  LEON_REG_IRQFORCE_OFFSET                                 0x98
#define  LEON_REG_IRQCLEAR_OFFSET                                 0x9C
#define  LEON_REG_PIODATA_OFFSET                                  0xA0
#define  LEON_REG_PIODIR_OFFSET                                   0xA4
#define  LEON_REG_PIOIRQ_OFFSET                                   0xA8
#define  LEON_REG_SIM_RAM_SIZE_OFFSET                             0xF4
#define  LEON_REG_SIM_ROM_SIZE_OFFSET                             0xF8

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
 *
 */

#define LEON_REG_UART_CONTROL_RTD  0x000000FF /* RX/TX data */

/*
 *  The following defines the bits in the LEON UART Status Registers.
 */

#define LEON_REG_UART_STATUS_CLR  0x00000000 /* Clear all status bits */
#define LEON_REG_UART_STATUS_DR   0x00000001 /* Data Ready */
#define LEON_REG_UART_STATUS_TSE  0x00000002 /* TX Send Register Empty */
#define LEON_REG_UART_STATUS_THE  0x00000004 /* TX Hold Register Empty */
#define LEON_REG_UART_STATUS_BR   0x00000008 /* Break Error */
#define LEON_REG_UART_STATUS_OE   0x00000010 /* RX Overrun Error */
#define LEON_REG_UART_STATUS_PE   0x00000020 /* RX Parity Error */
#define LEON_REG_UART_STATUS_FE   0x00000040 /* RX Framing Error */
#define LEON_REG_UART_STATUS_ERR  0x00000078 /* Error Mask */


/*
 *  The following defines the bits in the LEON UART Status Registers.
 */

#define LEON_REG_UART_CTRL_RE     0x00000001 /* Receiver enable */
#define LEON_REG_UART_CTRL_TE     0x00000002 /* Transmitter enable */
#define LEON_REG_UART_CTRL_RI     0x00000004 /* Receiver interrupt enable */
#define LEON_REG_UART_CTRL_TI     0x00000008 /* Transmitter interrupt enable */
#define LEON_REG_UART_CTRL_PS     0x00000010 /* Parity select */
#define LEON_REG_UART_CTRL_PE     0x00000020 /* Parity enable */
#define LEON_REG_UART_CTRL_FL     0x00000040 /* Flow control enable */
#define LEON_REG_UART_CTRL_LB     0x00000080 /* Loop Back enable */

#ifndef ASM

/*
 *  This is used to manipulate the on-chip registers.
 *
 *  The following symbol must be defined in the linkcmds file and point
 *  to the correct location.
 */

extern LEON_Register_Map LEON_REG;

static __inline__ int bsp_irq_fixup(int irq)
{
       return irq;
}

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
  do { \
    LEON_REG.Interrupt_Clear = (1 << (_source)); \
  } while (0)

#define LEON_Force_interrupt( _source ) \
  do { \
    LEON_REG.Interrupt_Force = (1 << (_source)); \
  } while (0)

#define LEON_Is_interrupt_pending( _source ) \
  (LEON_REG.Interrupt_Pending & (1 << (_source)))

#define LEON_Is_interrupt_masked( _source ) \
  (LEON_REG.Interrupt_Masked & (1 << (_source)))

#define LEON_Mask_interrupt( _source ) \
  do { \
    uint32_t _level; \
    \
    _level = sparc_disable_interrupts(); \
      LEON_REG.Interrupt_Mask &= ~(1 << (_source)); \
    sparc_enable_interrupts( _level ); \
  } while (0)

#define LEON_Unmask_interrupt( _source ) \
  do { \
    uint32_t _level; \
    \
    _level = sparc_disable_interrupts(); \
      LEON_REG.Interrupt_Mask |= (1 << (_source)); \
    sparc_enable_interrupts( _level ); \
  } while (0)

#define LEON_Disable_interrupt( _source, _previous ) \
  do { \
    uint32_t _level; \
    uint32_t _mask = 1 << (_source); \
    \
    _level = sparc_disable_interrupts(); \
      (_previous) = LEON_REG.Interrupt_Mask; \
      LEON_REG.Interrupt_Mask = _previous & ~_mask; \
    sparc_enable_interrupts( _level ); \
    (_previous) &= _mask; \
  } while (0)

#define LEON_Restore_interrupt( _source, _previous ) \
  do { \
    uint32_t _level; \
    uint32_t _mask = 1 << (_source); \
    \
    _level = sparc_disable_interrupts(); \
      LEON_REG.Interrupt_Mask = \
        (LEON_REG.Interrupt_Mask & ~_mask) | (_previous); \
    sparc_enable_interrupts( _level ); \
  } while (0)

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
	asm volatile (" lda [%1] 1, %0\n" : "=r"(tmp) : "r"(addr));
	return tmp;
}

#endif /* !ASM */

#ifdef __cplusplus
}
#endif

#endif /* !_INCLUDE_LEON_h */

