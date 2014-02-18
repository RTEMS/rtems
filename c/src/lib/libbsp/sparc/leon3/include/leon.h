/**
 * @file
 * @ingroup sparc_leon3
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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _INCLUDE_LEON_h
#define _INCLUDE_LEON_h

#include <rtems.h>
#include <amba.h>

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

#define LEON_TRAP_TYPE( _source ) SPARC_ASYNCHRONOUS_TRAP((_source) + 0x10)

#define LEON_TRAP_SOURCE( _trap ) ((_trap) - 0x10)

#define LEON_INT_TRAP( _trap ) \
  ( (_trap) >= 0x11 && \
    (_trap) <= 0x1F )

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
 *
 */

#define LEON_REG_UART_CONTROL_RTD  0x000000FF /* RX/TX data */

/*
 *  The following defines the bits in the LEON UART Status Registers.
 */

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

extern volatile struct irqmp_regs *LEON3_IrqCtrl_Regs;  /* LEON3 Interrupt Controller */
extern volatile struct gptimer_regs *LEON3_Timer_Regs; /* LEON3 GP Timer */

/* LEON3 CPU Index of boot CPU */
extern uint32_t LEON3_Cpu_Index;

/* The external IRQ number, -1 if not external interrupts */
extern int LEON3_IrqCtrl_EIrq;

static __inline__ int bsp_irq_fixup(int irq)
{
       int eirq;

       if (LEON3_IrqCtrl_EIrq != 0 && irq == LEON3_IrqCtrl_EIrq) {
               /* Get interrupt number from IRQ controller */
               eirq = LEON3_IrqCtrl_Regs->intid[LEON3_Cpu_Index] & 0x1f;
               if (eirq & 0x10)
                       irq = eirq;
       }

       return irq;
}

/* Macros used for manipulating bits in LEON3 GP Timer Control Register */

#define LEON3_GPTIMER_EN 1
#define LEON3_GPTIMER_RL 2
#define LEON3_GPTIMER_LD 4
#define LEON3_GPTIMER_IRQEN 8

#define LEON3_MP_IRQ    14        /* Irq used by shared memory driver */

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

extern rtems_interrupt_lock LEON3_IrqCtrl_Lock;

#define LEON3_IRQCTRL_ACQUIRE(_level ) \
  rtems_interrupt_lock_acquire( &LEON3_IrqCtrl_Lock, _level )

#define LEON3_IRQCTRL_RELEASE(_level ) \
  rtems_interrupt_lock_release( &LEON3_IrqCtrl_Lock, _level )

#define LEON_Clear_interrupt( _source ) \
  do { \
    LEON3_IrqCtrl_Regs->iclear = (1 << (_source)); \
  } while (0)

#define LEON_Force_interrupt( _source ) \
  do { \
    LEON3_IrqCtrl_Regs->iforce = (1 << (_source)); \
  } while (0)

#define LEON_Is_interrupt_pending( _source ) \
  (LEON3_IrqCtrl_Regs->ipend & (1 << (_source)))

#define LEON_Is_interrupt_masked( _source ) \
  do {\
     (LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index] & (1 << (_source))); \
   } while (0)

#define LEON_Mask_interrupt( _source ) \
  do { \
    rtems_interrupt_level _level; \
    LEON3_IRQCTRL_ACQUIRE( _level ); \
     LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index]  &= ~(1 << (_source)); \
    LEON3_IRQCTRL_RELEASE( _level ); \
  } while (0)

#define LEON_Unmask_interrupt( _source ) \
  do { \
    rtems_interrupt_level _level; \
    LEON3_IRQCTRL_ACQUIRE( _level ); \
    LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index]  |= (1 << (_source)); \
    LEON3_IRQCTRL_RELEASE( _level ); \
  } while (0)

#define LEON_Disable_interrupt( _source, _previous ) \
  do { \
    rtems_interrupt_level _level; \
    uint32_t _mask = 1 << (_source); \
    LEON3_IRQCTRL_ACQUIRE( _level ); \
     (_previous) = LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index]; \
     LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index] = _previous & ~_mask; \
    LEON3_IRQCTRL_RELEASE( _level ); \
    (_previous) &= _mask; \
  } while (0)

#define LEON_Restore_interrupt( _source, _previous ) \
  do { \
    rtems_interrupt_level _level; \
    uint32_t _mask = 1 << (_source); \
    LEON3_IRQCTRL_ACQUIRE( _level ); \
      LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index] = \
        (LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index] & ~_mask) | (_previous); \
    LEON3_IRQCTRL_RELEASE( _level ); \
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

/* Let user override which on-chip APBUART will be debug UART
 * 0 = Default APBUART. On MP system CPU0=APBUART0, CPU1=APBUART1...
 * 1 = APBUART[0]
 * 2 = APBUART[1]
 * 3 = APBUART[2]
 * ...
 */
extern int syscon_uart_index;

/* Let user override which on-chip APBUART will be debug UART
 * 0 = Default APBUART. On MP system CPU0=APBUART0, CPU1=APBUART1...
 * 1 = APBUART[0]
 * 2 = APBUART[1]
 * 3 = APBUART[2]
 * ...
 */
extern int debug_uart_index;

/*
 *  apbuart_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */
void apbuart_outbyte_polled(
  struct apbuart_regs *regs,
  unsigned char ch,
  int do_cr_on_newline,
  int wait_sent
);

/*
 *  apbuart_inbyte_nonblocking
 *
 *  This routine polls for a character.
 */
int apbuart_inbyte_nonblocking(struct apbuart_regs *regs);

/**
 * @brief Initializes a secondary processor.
 *
 * @param[in] cpu The processor executing this function.
 */
void leon3_secondary_cpu_initialize(uint32_t cpu);

void leon3_cpu_counter_initialize(void);

/* GRLIB extended IRQ controller register */
void leon3_ext_irq_init(void);

void bsp_debug_uart_init(void);

#endif /* !ASM */

#ifdef __cplusplus
}
#endif

#endif /* !_INCLUDE_LEON_h */
/* end of include file */

