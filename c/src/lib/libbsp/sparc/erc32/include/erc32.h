/*  erc32.h
 *
 *  This include file contains information pertaining to the ERC32.
 *  The ERC32 is a custom SPARC V7 implementation based on the Cypress
 *  601/602 chipset.  This CPU has a number of on-board peripherals and
 *  was developed by the European Space Agency to target space applications.
 *
 *  NOTE:  Other than where absolutely required, this version currently
 *         supports only the peripherals and bits used by the basic board
 *         support package. This includes at least significant pieces of
 *         the following items:
 *
 *           + UART Channels A and B
 *           + General Purpose Timer
 *           + Real Time Clock
 *           + Watchdog Timer (so it can be disabled)
 *           + Control Register (so powerdown mode can be enabled)
 *           + Memory Control Register
 *           + Interrupt Control
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995.
 *  European Space Agency.
 */

#ifndef _INCLUDE_ERC32_h
#define _INCLUDE_ERC32_h

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

#define ERC32_INTERRUPT_MASKED_ERRORS             1
#define ERC32_INTERRUPT_EXTERNAL_1                2
#define ERC32_INTERRUPT_EXTERNAL_2                3
#define ERC32_INTERRUPT_UART_A_RX_TX              4
#define ERC32_INTERRUPT_UART_B_RX_TX              5
#define ERC32_INTERRUPT_CORRECTABLE_MEMORY_ERROR  6
#define ERC32_INTERRUPT_UART_ERROR                7
#define ERC32_INTERRUPT_DMA_ACCESS_ERROR          8
#define ERC32_INTERRUPT_DMA_TIMEOUT               9
#define ERC32_INTERRUPT_EXTERNAL_3               10
#define ERC32_INTERRUPT_EXTERNAL_4               11
#define ERC32_INTERRUPT_GENERAL_PURPOSE_TIMER    12
#define ERC32_INTERRUPT_REAL_TIME_CLOCK          13
#define ERC32_INTERRUPT_EXTERNAL_5               14
#define ERC32_INTERRUPT_WATCHDOG_TIMEOUT         15

#ifndef ASM

/*
 *  Trap Types for on-chip peripherals
 *
 *  Source: Table 8 - Interrupt Trap Type and Default Priority Assignments
 *
 *  NOTE: The priority level for each source corresponds to the least
 *        significant nibble of the trap type.
 */

#define ERC32_TRAP_TYPE( _source ) SPARC_ASYNCHRONOUS_TRAP((_source) + 0x10)

#define ERC32_TRAP_SOURCE( _trap ) ((_trap) - 0x10)

#define ERC32_Is_MEC_Trap( _trap ) \
  ( (_trap) >= ERC32_TRAP_TYPE( ERC32_INTERRUPT_MASKED_ERRORS ) && \
    (_trap) <= ERC32_TRAP_TYPE( ERC32_INTERRUPT_WATCHDOG_TIMEOUT ) )

/*
 *  Structure for ERC32 memory mapped registers.
 *
 *  Source: Section 3.25.2 - Register Address Map
 *
 *  NOTE:  There is only one of these structures per CPU, its base address
 *         is 0x01f80000, and the variable MEC is placed there by the
 *         linkcmds file.
 */

typedef struct {
  volatile uint32_t    Control;                              /* offset 0x00 */
  volatile uint32_t    Software_Reset;                       /* offset 0x04 */
  volatile uint32_t    Power_Down;                           /* offset 0x08 */
  volatile uint32_t    Unimplemented_0;                      /* offset 0x0c */
  volatile uint32_t    Memory_Configuration;                 /* offset 0x10 */
  volatile uint32_t    IO_Configuration;                     /* offset 0x14 */
  volatile uint32_t    Wait_State_Configuration;             /* offset 0x18 */
  volatile uint32_t    Unimplemented_1;                      /* offset 0x1c */
  volatile uint32_t    Memory_Access_0;                      /* offset 0x20 */
  volatile uint32_t    Memory_Access_1;                      /* offset 0x24 */
  volatile uint32_t    Unimplemented_2[ 7 ];                 /* offset 0x28 */
  volatile uint32_t    Interrupt_Shape;                      /* offset 0x44 */
  volatile uint32_t    Interrupt_Pending;                    /* offset 0x48 */
  volatile uint32_t    Interrupt_Mask;                       /* offset 0x4c */
  volatile uint32_t    Interrupt_Clear;                      /* offset 0x50 */
  volatile uint32_t    Interrupt_Force;                      /* offset 0x54 */
  volatile uint32_t    Unimplemented_3[ 2 ];                 /* offset 0x58 */
                                                             /* offset 0x60 */
  volatile uint32_t    Watchdog_Program_and_Timeout_Acknowledge;
  volatile uint32_t    Watchdog_Trap_Door_Set;               /* offset 0x64 */
  volatile uint32_t    Unimplemented_4[ 6 ];                 /* offset 0x68 */
  volatile uint32_t    Real_Time_Clock_Counter;              /* offset 0x80 */
  volatile uint32_t    Real_Time_Clock_Scalar;               /* offset 0x84 */
  volatile uint32_t    General_Purpose_Timer_Counter;        /* offset 0x88 */
  volatile uint32_t    General_Purpose_Timer_Scalar;         /* offset 0x8c */
  volatile uint32_t    Unimplemented_5[ 2 ];                 /* offset 0x90 */
  volatile uint32_t    Timer_Control;                        /* offset 0x98 */
  volatile uint32_t    Unimplemented_6;                      /* offset 0x9c */
  volatile uint32_t    System_Fault_Status;                  /* offset 0xa0 */
  volatile uint32_t    First_Failing_Address;                /* offset 0xa4 */
  volatile uint32_t    First_Failing_Data;                   /* offset 0xa8 */
  volatile uint32_t    First_Failing_Syndrome_and_Check_Bits;/* offset 0xac */
  volatile uint32_t    Error_and_Reset_Status;               /* offset 0xb0 */
  volatile uint32_t    Error_Mask;                           /* offset 0xb4 */
  volatile uint32_t    Unimplemented_7[ 2 ];                 /* offset 0xb8 */
  volatile uint32_t    Debug_Control;                        /* offset 0xc0 */
  volatile uint32_t    Breakpoint;                           /* offset 0xc4 */
  volatile uint32_t    Watchpoint;                           /* offset 0xc8 */
  volatile uint32_t    Unimplemented_8;                      /* offset 0xcc */
  volatile uint32_t    Test_Control;                         /* offset 0xd0 */
  volatile uint32_t    Test_Data;                            /* offset 0xd4 */
  volatile uint32_t    Unimplemented_9[ 2 ];                 /* offset 0xd8 */
  volatile uint32_t    UART_Channel_A;                       /* offset 0xe0 */
  volatile uint32_t    UART_Channel_B;                       /* offset 0xe4 */
  volatile uint32_t    UART_Status;                          /* offset 0xe8 */
} ERC32_Register_Map;

#endif

/*
 *  The following constants are intended to be used ONLY in assembly
 *  language files.
 *
 *  NOTE:  The intended style of usage is to load the address of MEC
 *         into a register and then use these as displacements from
 *         that register.
 */

#ifdef ASM

#define  ERC32_MEC_CONTROL_OFFSET                                  0x00
#define  ERC32_MEC_SOFTWARE_RESET_OFFSET                           0x04
#define  ERC32_MEC_POWER_DOWN_OFFSET                               0x08
#define  ERC32_MEC_UNIMPLEMENTED_0_OFFSET                          0x0C
#define  ERC32_MEC_MEMORY_CONFIGURATION_OFFSET                     0x10
#define  ERC32_MEC_IO_CONFIGURATION_OFFSET                         0x14
#define  ERC32_MEC_WAIT_STATE_CONFIGURATION_OFFSET                 0x18
#define  ERC32_MEC_UNIMPLEMENTED_1_OFFSET                          0x1C
#define  ERC32_MEC_MEMORY_ACCESS_0_OFFSET                          0x20
#define  ERC32_MEC_MEMORY_ACCESS_1_OFFSET                          0x24
#define  ERC32_MEC_UNIMPLEMENTED_2_OFFSET                          0x28
#define  ERC32_MEC_INTERRUPT_SHAPE_OFFSET                          0x44
#define  ERC32_MEC_INTERRUPT_PENDING_OFFSET                        0x48
#define  ERC32_MEC_INTERRUPT_MASK_OFFSET                           0x4C
#define  ERC32_MEC_INTERRUPT_CLEAR_OFFSET                          0x50
#define  ERC32_MEC_INTERRUPT_FORCE_OFFSET                          0x54
#define  ERC32_MEC_UNIMPLEMENTED_3_OFFSET                          0x58
#define  ERC32_MEC_WATCHDOG_PROGRAM_AND_TIMEOUT_ACKNOWLEDGE_OFFSET 0x60
#define  ERC32_MEC_WATCHDOG_TRAP_DOOR_SET_OFFSET                   0x64
#define  ERC32_MEC_UNIMPLEMENTED_4_OFFSET                          0x6C
#define  ERC32_MEC_REAL_TIME_CLOCK_COUNTER_OFFSET                  0x80
#define  ERC32_MEC_REAL_TIME_CLOCK_SCALAR_OFFSET                   0x84
#define  ERC32_MEC_GENERAL_PURPOSE_TIMER_COUNTER_OFFSET            0x88
#define  ERC32_MEC_GENERAL_PURPOSE_TIMER_SCALAR_OFFSET             0x8C
#define  ERC32_MEC_UNIMPLEMENTED_5_OFFSET                          0x90
#define  ERC32_MEC_TIMER_CONTROL_OFFSET                            0x98
#define  ERC32_MEC_UNIMPLEMENTED_6_OFFSET                          0x9C
#define  ERC32_MEC_SYSTEM_FAULT_STATUS_OFFSET                      0xA0
#define  ERC32_MEC_FIRST_FAILING_ADDRESS_OFFSET                    0xA4
#define  ERC32_MEC_FIRST_FAILING_DATA_OFFSET                       0xA8
#define  ERC32_MEC_FIRST_FAILING_SYNDROME_AND_CHECK_BITS_OFFSET    0xAC
#define  ERC32_MEC_ERROR_AND_RESET_STATUS_OFFSET                   0xB0
#define  ERC32_MEC_ERROR_MASK_OFFSET                               0xB4
#define  ERC32_MEC_UNIMPLEMENTED_7_OFFSET                          0xB8
#define  ERC32_MEC_DEBUG_CONTROL_OFFSET                            0xC0
#define  ERC32_MEC_BREAKPOINT_OFFSET                               0xC4
#define  ERC32_MEC_WATCHPOINT_OFFSET                               0xC8
#define  ERC32_MEC_UNIMPLEMENTED_8_OFFSET                          0xCC
#define  ERC32_MEC_TEST_CONTROL_OFFSET                             0xD0
#define  ERC32_MEC_TEST_DATA_OFFSET                                0xD4
#define  ERC32_MEC_UNIMPLEMENTED_9_OFFSET                          0xD8
#define  ERC32_MEC_UART_CHANNEL_A_OFFSET                           0xE0
#define  ERC32_MEC_UART_CHANNEL_B_OFFSET                           0xE4
#define  ERC32_MEC_UART_STATUS_OFFSET                              0xE8

#endif

/*
 *  The following defines the bits in the Configuration Register.
 */

#define ERC32_CONFIGURATION_POWER_DOWN_MASK               0x00000001
#define ERC32_CONFIGURATION_POWER_DOWN_ALLOWED            0x00000001
#define ERC32_CONFIGURATION_POWER_DOWN_DISABLED           0x00000000

#define ERC32_CONFIGURATION_SOFTWARE_RESET_MASK           0x00000002
#define ERC32_CONFIGURATION_SOFTWARE_RESET_ALLOWED        0x00000002
#define ERC32_CONFIGURATION_SOFTWARE_RESET_DISABLED       0x00000000

#define ERC32_CONFIGURATION_BUS_TIMEOUT_MASK              0x00000004
#define ERC32_CONFIGURATION_BUS_TIMEOUT_ENABLED           0x00000004
#define ERC32_CONFIGURATION_BUS_TIMEOUT_DISABLED          0x00000000

#define ERC32_CONFIGURATION_ACCESS_PROTECTION_MASK        0x00000008
#define ERC32_CONFIGURATION_ACCESS_PROTECTION_ENABLED     0x00000008
#define ERC32_CONFIGURATION_ACCESS_PROTECTION_DISABLED    0x00000000

/*
 *  The following defines the bits in the Memory Configuration Register.
 */

#define ERC32_MEMORY_CONFIGURATION_RAM_SIZE_MASK  0x00001C00
#define ERC32_MEMORY_CONFIGURATION_RAM_SIZE_256K  ( 0 << 10 )
#define ERC32_MEMORY_CONFIGURATION_RAM_SIZE_512K  ( 1 << 10 )
#define ERC32_MEMORY_CONFIGURATION_RAM_SIZE_1MB   ( 2 << 10 )
#define ERC32_MEMORY_CONFIGURATION_RAM_SIZE_2MB   ( 3 << 10 )
#define ERC32_MEMORY_CONFIGURATION_RAM_SIZE_4MB   ( 4 << 10 )
#define ERC32_MEMORY_CONFIGURATION_RAM_SIZE_8MB   ( 5 << 10 )
#define ERC32_MEMORY_CONFIGURATION_RAM_SIZE_16MB  ( 6 << 10 )
#define ERC32_MEMORY_CONFIGURATION_RAM_SIZE_32MB  ( 7 << 10 )

#define ERC32_MEMORY_CONFIGURATION_PROM_SIZE_MASK  0x001C0000
#define ERC32_MEMORY_CONFIGURATION_PROM_SIZE_128K    ( 0 << 18 )
#define ERC32_MEMORY_CONFIGURATION_PROM_SIZE_256K    ( 1 << 18 )
#define ERC32_MEMORY_CONFIGURATION_PROM_SIZE_512K   ( 2 << 18 )
#define ERC32_MEMORY_CONFIGURATION_PROM_SIZE_1M   ( 3 << 18 )
#define ERC32_MEMORY_CONFIGURATION_PROM_SIZE_2M   ( 4 << 18 )
#define ERC32_MEMORY_CONFIGURATION_PROM_SIZE_4M  ( 5 << 18 )
#define ERC32_MEMORY_CONFIGURATION_PROM_SIZE_8M  ( 6 << 18 )
#define ERC32_MEMORY_CONFIGURATION_PROM_SIZE_16M  ( 7 << 18 )

/*
 *  The following defines the bits in the Timer Control Register.
 */

#define ERC32_MEC_TIMER_CONTROL_GCR    0x00000001  /* 1 = reload at 0 */
                                               /* 0 = stop at 0 */
#define ERC32_MEC_TIMER_CONTROL_GCL    0x00000002  /* 1 = load and start */
                                               /* 0 = no function */
#define ERC32_MEC_TIMER_CONTROL_GSE    0x00000004  /* 1 = enable counting */
                                               /* 0 = hold scalar and counter */
#define ERC32_MEC_TIMER_CONTROL_GSL    0x00000008  /* 1 = load scalar and start */
                                               /* 0 = no function */

#define ERC32_MEC_TIMER_CONTROL_RTCCR  0x00000100  /* 1 = reload at 0 */
                                               /* 0 = stop at 0 */
#define ERC32_MEC_TIMER_CONTROL_RTCCL  0x00000200  /* 1 = load and start */
                                               /* 0 = no function */
#define ERC32_MEC_TIMER_CONTROL_RTCSE  0x00000400  /* 1 = enable counting */
                                               /* 0 = hold scalar and counter */
#define ERC32_MEC_TIMER_CONTROL_RTCSL  0x00000800  /* 1 = load scalar and start */
                                               /* 0 = no function */

/*
 *  The following defines the bits in the UART Control Registers.
 *
 */

#define ERC32_MEC_UART_CONTROL_RTD  0x000000FF /* RX/TX data */

/*
 *  The following defines the bits in the MEC UART Control Registers.
 */

#define ERC32_MEC_UART_STATUS_DR   0x00000001 /* Data Ready */
#define ERC32_MEC_UART_STATUS_TSE  0x00000002 /* TX Send Register Empty */
#define ERC32_MEC_UART_STATUS_THE  0x00000004 /* TX Hold Register Empty */
#define ERC32_MEC_UART_STATUS_FE   0x00000010 /* RX Framing Error */
#define ERC32_MEC_UART_STATUS_PE   0x00000020 /* RX Parity Error */
#define ERC32_MEC_UART_STATUS_OE   0x00000040 /* RX Overrun Error */
#define ERC32_MEC_UART_STATUS_CU   0x00000080 /* Clear Errors */
#define ERC32_MEC_UART_STATUS_TXE  0x00000006 /* TX Empty */
#define ERC32_MEC_UART_STATUS_CLRA 0x00000080 /* Clear UART A */
#define ERC32_MEC_UART_STATUS_CLRB 0x00800000 /* Clear UART B */
#define ERC32_MEC_UART_STATUS_ERRA 0x00000070 /* Error in UART A */
#define ERC32_MEC_UART_STATUS_ERRB 0x00700000 /* Error in UART B */

#define ERC32_MEC_UART_STATUS_DRA   (ERC32_MEC_UART_STATUS_DR  << 0)
#define ERC32_MEC_UART_STATUS_TSEA  (ERC32_MEC_UART_STATUS_TSE << 0)
#define ERC32_MEC_UART_STATUS_THEA  (ERC32_MEC_UART_STATUS_THE << 0)
#define ERC32_MEC_UART_STATUS_FEA   (ERC32_MEC_UART_STATUS_FE  << 0)
#define ERC32_MEC_UART_STATUS_PEA   (ERC32_MEC_UART_STATUS_PE  << 0)
#define ERC32_MEC_UART_STATUS_OEA   (ERC32_MEC_UART_STATUS_OE  << 0)
#define ERC32_MEC_UART_STATUS_CUA   (ERC32_MEC_UART_STATUS_CU  << 0)
#define ERC32_MEC_UART_STATUS_TXEA  (ERC32_MEC_UART_STATUS_TXE << 0)

#define ERC32_MEC_UART_STATUS_DRB   (ERC32_MEC_UART_STATUS_DR  << 16)
#define ERC32_MEC_UART_STATUS_TSEB  (ERC32_MEC_UART_STATUS_TSE << 16)
#define ERC32_MEC_UART_STATUS_THEB  (ERC32_MEC_UART_STATUS_THE << 16)
#define ERC32_MEC_UART_STATUS_FEB   (ERC32_MEC_UART_STATUS_FE  << 16)
#define ERC32_MEC_UART_STATUS_PEB   (ERC32_MEC_UART_STATUS_PE  << 16)
#define ERC32_MEC_UART_STATUS_OEB   (ERC32_MEC_UART_STATUS_OE  << 16)
#define ERC32_MEC_UART_STATUS_CUB   (ERC32_MEC_UART_STATUS_CU  << 16)
#define ERC32_MEC_UART_STATUS_TXEB  (ERC32_MEC_UART_STATUS_TXE << 16)

#ifndef ASM

/*
 *  This is used to manipulate the on-chip registers.
 *
 *  The following symbol must be defined in the linkcmds file and point
 *  to the correct location.
 */

extern ERC32_Register_Map ERC32_MEC;

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

#define ERC32_Clear_interrupt( _source ) \
  do { \
    ERC32_MEC.Interrupt_Clear = (1 << (_source)); \
  } while (0)

#define ERC32_Force_interrupt( _source ) \
  do { \
    uint32_t   _level; \
    \
    _level = sparc_disable_interrupts(); \
    ERC32_MEC.Test_Control = ERC32_MEC.Test_Control | 0x80000; \
    ERC32_MEC.Interrupt_Force = (1 << (_source)); \
    sparc_enable_interrupts( _level ); \
  } while (0)

#define ERC32_Is_interrupt_pending( _source ) \
  (ERC32_MEC.Interrupt_Pending & (1 << (_source)))

#define ERC32_Is_interrupt_masked( _source ) \
  (ERC32_MEC.Interrupt_Masked & (1 << (_source)))

#define ERC32_Mask_interrupt( _source ) \
  do { \
    uint32_t   _level; \
    \
    _level = sparc_disable_interrupts(); \
      ERC32_MEC.Interrupt_Mask |= (1 << (_source)); \
    sparc_enable_interrupts( _level ); \
  } while (0)

#define ERC32_Unmask_interrupt( _source ) \
  do { \
    uint32_t   _level; \
    \
    _level = sparc_disable_interrupts(); \
      ERC32_MEC.Interrupt_Mask &= ~(1 << (_source)); \
    sparc_enable_interrupts( _level ); \
  } while (0)

#define ERC32_Disable_interrupt( _source, _previous ) \
  do { \
    uint32_t   _level; \
    uint32_t   _mask = 1 << (_source); \
    \
    _level = sparc_disable_interrupts(); \
      (_previous) = ERC32_MEC.Interrupt_Mask; \
      ERC32_MEC.Interrupt_Mask = _previous | _mask; \
    sparc_enable_interrupts( _level ); \
    (_previous) &= _mask; \
  } while (0)

#define ERC32_Restore_interrupt( _source, _previous ) \
  do { \
    uint32_t   _level; \
    uint32_t   _mask = 1 << (_source); \
    \
    _level = sparc_disable_interrupts(); \
      ERC32_MEC.Interrupt_Mask = \
        (ERC32_MEC.Interrupt_Mask & ~_mask) | (_previous); \
    sparc_enable_interrupts( _level ); \
  } while (0)

/* Make all SPARC BSPs have common macros for interrupt handling */
#define BSP_Clear_interrupt(_source) ERC32_Clear_interrupt(_source)
#define BSP_Force_interrupt(_source) ERC32_Force_interrupt(_source)
#define BSP_Is_interrupt_pending(_source) ERC32_Is_interrupt_pending(_source)
#define BSP_Is_interrupt_masked(_source) ERC32_Is_interrupt_masked(_source)
#define BSP_Unmask_interrupt(_source) ERC32_Unmask_interrupt(_source)
#define BSP_Mask_interrupt(_source) ERC32_Mask_interrupt(_source)
#define BSP_Disable_interrupt(_source, _previous) \
        ERC32_Disable_interrupt(_source, _prev)
#define BSP_Restore_interrupt(_source, _previous) \
        ERC32_Restore_interrupt(_source, _previous)

/*
 *  The following macros attempt to hide the fact that the General Purpose
 *  Timer and Real Time Clock Timer share the Timer Control Register.  Because
 *  the Timer Control Register is write only, we must mirror it in software
 *  and insure that writes to one timer do not alter the current settings
 *  and status of the other timer.
 *
 *  This code promotes the view that the two timers are completely independent.
 *  By exclusively using the routines below to access the Timer Control
 *  Register, the application can view the system as having a General Purpose
 *  Timer Control Register and a Real Time Clock Timer Control Register
 *  rather than the single shared value.
 *
 *  Each logical timer control register is organized as follows:
 *
 *    D0 - Counter Reload
 *          1 = reload counter at zero and restart
 *          0 = stop counter at zero
 *
 *    D1 - Counter Load
 *          1 = load counter with preset value and restart
 *          0 = no function
 *
 *    D2 - Enable
 *          1 = enable counting
 *          0 = hold scaler and counter
 *
 *    D3 - Scaler Load
 *          1 = load scalar with preset value and restart
 *          0 = no function
 *
 *  To insure the management of the mirror is atomic, we disable interrupts
 *  around updates.
 */

#define ERC32_MEC_TIMER_COUNTER_RELOAD_AT_ZERO     0x00000001
#define ERC32_MEC_TIMER_COUNTER_STOP_AT_ZERO       0x00000000

#define ERC32_MEC_TIMER_COUNTER_LOAD_COUNTER       0x00000002

#define ERC32_MEC_TIMER_COUNTER_ENABLE_COUNTING    0x00000004
#define ERC32_MEC_TIMER_COUNTER_DISABLE_COUNTING   0x00000000

#define ERC32_MEC_TIMER_COUNTER_LOAD_SCALER        0x00000008

#define ERC32_MEC_TIMER_COUNTER_RELOAD_MASK        0x00000001
#define ERC32_MEC_TIMER_COUNTER_ENABLE_MASK        0x00000004

#define ERC32_MEC_TIMER_COUNTER_DEFINED_MASK       0x0000000F
#define ERC32_MEC_TIMER_COUNTER_CURRENT_MODE_MASK  0x00000005

extern uint32_t   _ERC32_MEC_Timer_Control_Mirror;

/*
 *  This macros manipulate the General Purpose Timer portion of the
 *  Timer Control register and promote the view that there are actually
 *  two independent Timer Control Registers.
 */

#define ERC32_MEC_Set_General_Purpose_Timer_Control( _value ) \
  do { \
    uint32_t   _level; \
    uint32_t   _control; \
    uint32_t   __value; \
    \
    __value = ((_value) & 0x0f); \
    _level = sparc_disable_interrupts(); \
      _control = _ERC32_MEC_Timer_Control_Mirror; \
      _control &= ERC32_MEC_TIMER_COUNTER_DEFINED_MASK << 8; \
      _ERC32_MEC_Timer_Control_Mirror = _control | _value; \
      _control &= (ERC32_MEC_TIMER_COUNTER_CURRENT_MODE_MASK << 8); \
      _control |= __value; \
      /* printf( "GPT 0x%x 0x%x 0x%x\n", _value, __value, _control );  */ \
      ERC32_MEC.Timer_Control = _control; \
    sparc_enable_interrupts( _level ); \
  } while ( 0 )

#define ERC32_MEC_Get_General_Purpose_Timer_Control( _value ) \
  do { \
    (_value) = _ERC32_MEC_Timer_Control_Mirror & 0xf; \
  } while ( 0 )

/*
 *  This macros manipulate the Real Timer Clock Timer portion of the
 *  Timer Control register and promote the view that there are actually
 *  two independent Timer Control Registers.
 */

#define ERC32_MEC_Set_Real_Time_Clock_Timer_Control( _value ) \
  do { \
    uint32_t   _level; \
    uint32_t   _control; \
    uint32_t   __value; \
    \
    __value = ((_value) & 0x0f) << 8; \
    _level = sparc_disable_interrupts(); \
      _control = _ERC32_MEC_Timer_Control_Mirror; \
      _control &= ERC32_MEC_TIMER_COUNTER_DEFINED_MASK; \
      _ERC32_MEC_Timer_Control_Mirror = _control | __value; \
      _control &= ERC32_MEC_TIMER_COUNTER_CURRENT_MODE_MASK; \
      _control |= __value; \
      /* printf( "RTC 0x%x 0x%x 0x%x\n", _value, __value, _control ); */ \
      ERC32_MEC.Timer_Control = _control; \
    sparc_enable_interrupts( _level ); \
  } while ( 0 )

#define ERC32_MEC_Get_Real_Time_Clock_Timer_Control( _value ) \
  do { \
    (_value) = (_ERC32_MEC_Timer_Control_Mirror >> 8) & 0xf; \
  } while ( 0 )

#endif /* !ASM */

#ifdef __cplusplus
}
#endif

#endif /* !_INCLUDE_ERC32_h */
