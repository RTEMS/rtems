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
 *
 *  $Id$
 */
 
#ifndef _INCLUDE_LEON_h
#define _INCLUDE_LEON_h

#include <rtems/score/sparc.h>
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
    
/*
 *  Structure for LEON memory mapped registers.  
 *
 *  Source: Section 6.1 - On-chip registers
 *
 *  NOTE:  There is only one of these structures per CPU, its base address 
 *         is 0x80000000, and the variable LEON_REG is placed there by the 
 *         linkcmds file.
 */

/* Leon uses dynamic register mapping using amba configuration records,
 * LEON_Register_Map is obsolete
 */
/*
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
*/

typedef struct {
  volatile unsigned int data;
  volatile unsigned int status;
  volatile unsigned int ctrl;
} LEON3_UART_Regs_Map;

typedef struct {
  volatile unsigned int value;
  volatile unsigned int reload;
  volatile unsigned int conf;
  volatile unsigned int notused;
} LEON3_Timer_SubType;

typedef struct {
  volatile unsigned int scaler_value;   /* common timer registers */
  volatile unsigned int scaler_reload;
  volatile unsigned int status;
  volatile unsigned int notused;
  LEON3_Timer_SubType timer[8];
} LEON3_Timer_Regs_Map;

typedef struct {
  volatile unsigned int iodata;
  volatile unsigned int ioout;
  volatile unsigned int iodir;
  volatile unsigned int irqmask;
  volatile unsigned int irqpol;
  volatile unsigned int irqedge;
} LEON3_IOPORT_Regs_Map;

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

extern volatile LEON3_IrqCtrl_Regs_Map *LEON3_IrqCtrl_Regs;  /* LEON3 Interrupt Controller */
extern volatile LEON3_Timer_Regs_Map *LEON3_Timer_Regs; /* LEON3 GP Timer */
extern volatile LEON3_UART_Regs_Map *LEON3_Console_Uart[LEON3_APBUARTS];

extern int LEON3_Cpu_Index;

/* Macros used for manipulating bits in LEON3 GP Timer Control Register */

#define LEON3_GPTIMER_EN 1
#define LEON3_GPTIMER_RL 2
#define LEON3_GPTIMER_LD 4
#define LEON3_GPTIMER_IRQEN 8

#define LEON3_MP_IRQ    14        /* Irq used by shared memory driver */

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
  do { \
    LEON3_IrqCtrl_Regs->iclear = (1 << (_source)); \
  } while (0)

#define LEON_Force_interrupt( _source ) \
  do { \
    LEON3_IrqCtrl_Regs->iforce = (1 << (_source)); \
  } while (0)
 
#define LEON_Is_interrupt_pending( _source ) \
  (LEON3_IrqCtrl_Regs.ipend & (1 << (_source)))
 
#define LEON_Is_interrupt_masked( _source ) \
  do {\
     (LEON3_IrqCtrl_Regs.mask[LEON3_Cpu_Index] & (1 << (_source))); \
   } while (0)

 
#define LEON_Mask_interrupt( _source ) \
  do { \
    uint32_t _level; \
    _level = sparc_disable_interrupts(); \
     LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index]  &= ~(1 << (_source)); \
    sparc_enable_interrupts( _level ); \
  } while (0)
 
#define LEON_Unmask_interrupt( _source ) \
  do { \
    uint32_t _level; \
    _level = sparc_disable_interrupts(); \
    LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index]  |= (1 << (_source)); \
    sparc_enable_interrupts( _level ); \
  } while (0)

#define LEON_Disable_interrupt( _source, _previous ) \
  do { \
    uint32_t _level; \
    uint32_t _mask = 1 << (_source); \
    _level = sparc_disable_interrupts(); \
     (_previous) = LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index]; \
     LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index] = _previous & ~_mask; \
    sparc_enable_interrupts( _level ); \
    (_previous) &= _mask; \
  } while (0)
 
#define LEON_Restore_interrupt( _source, _previous ) \
  do { \
    uint32_t _level; \
    uint32_t _mask = 1 << (_source); \
    _level = sparc_disable_interrupts(); \
      LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index] = \
        (LEON3_IrqCtrl_Regs->mask[LEON3_Cpu_Index] & ~_mask) | (_previous); \
    sparc_enable_interrupts( _level ); \
  } while (0)


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

#endif /* !ASM */

#ifdef __cplusplus
}
#endif

#endif /* !_INCLUDE_LEON_h */
/* end of include file */

