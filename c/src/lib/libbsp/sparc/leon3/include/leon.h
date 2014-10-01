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
 *  http://www.rtems.org/license/LICENSE.
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
 *  The following defines the bits in the LEON Cache Control Register.
 */
#define LEON3_REG_CACHE_CTRL_FI      0x00200000 /* Flush instruction cache */

/* LEON3 Interrupt Controller */
extern volatile struct irqmp_regs *LEON3_IrqCtrl_Regs;
/* LEON3 GP Timer */
extern volatile struct gptimer_regs *LEON3_Timer_Regs;

/* LEON3 CPU Index of boot CPU */
extern uint32_t LEON3_Cpu_Index;

/* The external IRQ number, -1 if not external interrupts */
extern int LEON3_IrqCtrl_EIrq;

static __inline__ int bsp_irq_fixup(int irq)
{
  int eirq, cpu;

  if (LEON3_IrqCtrl_EIrq != 0 && irq == LEON3_IrqCtrl_EIrq) {
    /* Get interrupt number from IRQ controller */
    cpu = _LEON3_Get_current_processor();
    eirq = LEON3_IrqCtrl_Regs->intid[cpu] & 0x1f;
    if (eirq & 0x10)
      irq = eirq;
  }

  return irq;
}

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

extern rtems_interrupt_lock LEON3_IrqCtrl_Lock;

#define LEON3_IRQCTRL_ACQUIRE( _lock_context ) \
  rtems_interrupt_lock_acquire( &LEON3_IrqCtrl_Lock, _lock_context )

#define LEON3_IRQCTRL_RELEASE( _lock_context ) \
  rtems_interrupt_lock_release( &LEON3_IrqCtrl_Lock, _lock_context )

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
    rtems_interrupt_lock_context _lock_context; \
    LEON3_IRQCTRL_ACQUIRE( &_lock_context ); \
     LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index]  &= ~(1 << (_source)); \
    LEON3_IRQCTRL_RELEASE( &_lock_context ); \
  } while (0)

#define LEON_Unmask_interrupt( _source ) \
  do { \
    rtems_interrupt_lock_context _lock_context; \
    LEON3_IRQCTRL_ACQUIRE( &_lock_context ); \
    LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index]  |= (1 << (_source)); \
    LEON3_IRQCTRL_RELEASE( &_lock_context ); \
  } while (0)

#define LEON_Disable_interrupt( _source, _previous ) \
  do { \
    rtems_interrupt_lock_context _lock_context; \
    uint32_t _mask = 1 << (_source); \
    LEON3_IRQCTRL_ACQUIRE( &_lock_context ); \
     (_previous) = LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index]; \
     LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index] = _previous & ~_mask; \
    LEON3_IRQCTRL_RELEASE( &_lock_context ); \
    (_previous) &= _mask; \
  } while (0)

#define LEON_Restore_interrupt( _source, _previous ) \
  do { \
    rtems_interrupt_lock_context _lock_context; \
    uint32_t _mask = 1 << (_source); \
    LEON3_IRQCTRL_ACQUIRE( &_lock_context ); \
      LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index] = \
        (LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index] & ~_mask) | (_previous); \
    LEON3_IRQCTRL_RELEASE( &_lock_context ); \
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

#if defined(RTEMS_MULTIPROCESSING)
  #define LEON3_CLOCK_INDEX \
   (rtems_configuration_get_user_multiprocessing_table() ? LEON3_Cpu_Index : 0)
#else
  #define LEON3_CLOCK_INDEX 0
#endif

/*
 * We assume that a boot loader (usually GRMON) initialized the GPTIMER 0 to
 * run with 1MHz.  This is used to determine all clock frequencies of the PnP
 * devices.  See also ambapp_freq_init() and ambapp_freq_get().
 */
#define LEON3_GPTIMER_0_FREQUENCY_SET_BY_BOOT_LOADER 1000000

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

/* Let user override which on-chip APBUART will be debug UART
 * 0 = Default APBUART. On MP system CPU0=APBUART0, CPU1=APBUART1...
 * 1 = APBUART[0]
 * 2 = APBUART[1]
 * 3 = APBUART[2]
 * ...
 */
extern int debug_uart_index;

void leon3_cpu_counter_initialize(void);

/* GRLIB extended IRQ controller register */
void leon3_ext_irq_init(void);

void bsp_debug_uart_init(void);

void leon3_power_down_loop(void) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

static inline uint32_t leon3_get_cpu_count(
  volatile struct irqmp_regs *irqmp
)
{
  uint32_t mpstat = irqmp->mpstat;

  return ((mpstat >> LEON3_IRQMPSTATUS_CPUNR) & 0xf)  + 1;
}

static inline void leon3_set_system_register(uint32_t addr, uint32_t val)
{
  __asm__ volatile(
    "sta %1, [%0] 2"
    :
    : "r" (addr), "r" (val)
  );
}

static inline uint32_t leon3_get_system_register(uint32_t addr)
{
  uint32_t val;

  __asm__ volatile(
    "lda [%1] 2, %0"
    : "=r" (val)
    : "r" (addr)
  );

  return val;
}

static inline void leon3_set_cache_control_register(uint32_t val)
{
  leon3_set_system_register(0x0, val);
}

static inline uint32_t leon3_get_cache_control_register(void)
{
  return leon3_get_system_register(0x0);
}

static inline uint32_t leon3_get_inst_cache_config_register(void)
{
  return leon3_get_system_register(0x8);
}

static inline uint32_t leon3_get_data_cache_config_register(void)
{
  return leon3_get_system_register(0xc);
}

static inline bool leon3_irqmp_has_timestamp(
  volatile struct irqmp_timestamp_regs *irqmp_ts
)
{
  return (irqmp_ts->control >> 27) > 0;
}

#endif /* !ASM */

#ifdef __cplusplus
}
#endif

#endif /* !_INCLUDE_LEON_h */
/* end of include file */

