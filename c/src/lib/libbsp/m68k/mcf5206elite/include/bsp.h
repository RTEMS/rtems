/*
 * Board Support Package for MCF5206eLITE evaluation board
 * BSP definitions
 *
 * Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __mcf5206elite_BSP_h
#define __mcf5206elite_BSP_h

#include "mcf5206/mcf5206e.h"

#ifndef KB
#define KB (1024)
#endif
#ifndef MB 
#define MB (KB*KB)
#endif

/*** Board resources allocation ***/


/* 
 * To achieve some compatibility with dBUG monitor, we use the same
 * memory resources allocation as it is used in dBUG.
 *
 * If this definitions will be changed, change the linker script also.
 */
 
/* Memory mapping */
/* CS0: Boot Flash */
#define BSP_MEM_ADDR_FLASH    (0xFFE00000)
#define BSP_MEM_SIZE_FLASH    (1*MB)
#define BSP_MEM_MASK_FLASH    (MCF5206E_CSMR_MASK_1M)

/* CS2: External SRAM */
#define BSP_MEM_ADDR_ESRAM    (0x30000000)
#define BSP_MEM_SIZE_ESRAM    (1*MB)
#define BSP_MEM_MASK_ESRAM    (MCF5206E_CSMR_MASK_1M)

/* CS3: General-Purpose I/O register */
#define BSP_MEM_ADDR_GPIO     (0x40000000)
#define BSP_MEM_SIZE_GPIO     (64*KB)
#define BSP_MEM_MASK_GPIO     (MCF5206E_CSMR_MASK_64K)

/* DRAM0: Dynamic RAM */
#define BSP_MEM_ADDR_DRAM     (0x00000000)
#define BSP_MEM_SIZE_DRAM     (16*MB)
#define BSP_MEM_MASK_DRAM     (MCF5206E_DCMR_MASK_16M)

/* On-chip SRAM */
#define BSP_MEM_ADDR_SRAM     (0x20000000)
#define BSP_MEM_SIZE_SRAM     (8*KB)

/* On-chip peripherial registers */
#define BSP_MEM_ADDR_IMM      (0x10000000)
#define BSP_MEM_SIZE_IMM      (1*KB)
#define MBAR BSP_MEM_ADDR_IMM

/* Interrupt vector assignment */
#define BSP_INTVEC_AVEC1    (25)
#define BSP_INTLVL_AVEC1    (1)
#define BSP_INTPRIO_AVEC1   (3)

#define BSP_INTVEC_AVEC2    (26)
#define BSP_INTLVL_AVEC2    (2)
#define BSP_INTPRIO_AVEC2   (3)

#define BSP_INTVEC_AVEC3    (27)
#define BSP_INTLVL_AVEC3    (3)
#define BSP_INTPRIO_AVEC3   (3)

#define BSP_INTVEC_AVEC4    (28)
#define BSP_INTLVL_AVEC4    (4)
#define BSP_INTPRIO_AVEC4   (3)

#define BSP_INTVEC_AVEC5    (29)
#define BSP_INTLVL_AVEC5    (5)
#define BSP_INTPRIO_AVEC5   (3)

#define BSP_INTVEC_AVEC6    (30)
#define BSP_INTLVL_AVEC6    (6)
#define BSP_INTPRIO_AVEC6   (3)

#define BSP_INTVEC_AVEC7    (31)
#define BSP_INTLVL_AVEC7    (7)
#define BSP_INTPRIO_AVEC7   (3)

#define BSP_INTVEC_TIMER1   (BSP_INTVEC_AVEC5)
#define BSP_INTLVL_TIMER1   (BSP_INTLVL_AVEC5)
#define BSP_INTPRIO_TIMER1  (2)

#define BSP_INTVEC_TIMER2    (BSP_INTVEC_AVEC6)
#define BSP_INTLVL_TIMER2    (BSP_INTLVL_AVEC6)
#define BSP_INTPRIO_TIMER2   (2)

#define BSP_INTVEC_MBUS     (BSP_INTVEC_AVEC4)
#define BSP_INTLVL_MBUS     (BSP_INTLVL_AVEC4)
#define BSP_INTPRIO_MBUS    (2)

#define BSP_INTVEC_UART1    (64)
#define BSP_INTLVL_UART1    (4)
#define BSP_INTPRIO_UART1   (0)

#define BSP_INTVEC_UART2    (65)
#define BSP_INTLVL_UART2    (4)
#define BSP_INTPRIO_UART2   (1)

#define BSP_INTVEC_DMA0     (66)
#define BSP_INTLVL_DMA0     (3)
#define BSP_INTPRIO_DMA0    (1)

#define BSP_INTVEC_DMA1     (67)
#define BSP_INTLVL_DMA1     (3)
#define BSP_INTPRIO_DMA1    (2)

/* Location of DS1307 Real-Time Clock/NVRAM chip */
#define DS1307_I2C_BUS_NUMBER (0)

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>
#include <rtems.h>
#include <console.h>
#include <iosupp.h>
#include <clockdrv.h>

#include "i2c.h"

/*
 *  confdefs.h overrides for this BSP:
 *   - termios serial ports (defaults to 1)
 *   - Interrupt stack space is not minimum if defined.
 */

#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 2
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (4 * 1024)

/* System frequency */
#define BSP_SYSTEM_FREQUENCY ((unsigned int)&_SYS_CLOCK_FREQUENCY)
extern char _SYS_CLOCK_FREQUENCY; /* Don't use this variable directly!!! */

/* MBUS I2C bus clock default frequency */
#define BSP_MBUS_FREQUENCY (16000)

/* Number of I2C buses supported in this board */
#define I2C_NUMBER_OF_BUSES (1)

/* I2C bus selection */
#define I2C_SELECT_BUS(bus)

/*
 *  Define the time limits for RTEMS Test Suite test durations.
 *  Long test and short test duration limits are provided.  These
 *  values are in seconds and need to be converted to ticks for the
 *  application.
 *
 */

#define MAX_LONG_TEST_DURATION       300 /* 5 minutes = 300 seconds */
#define MAX_SHORT_TEST_DURATION      3   /* 3 seconds */

/*
 *  Stuff for Time Test 27
 *  Don't bother with hardware -- just use a software-interrupt
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) set_vector( (handler), 34, 1 )

#define Cause_tm27_intr()	asm volatile ("trap #2");

#define Clear_tm27_intr()

#define Lower_tm27_intr()

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define delay( microseconds ) \
  { register rtems_unsigned32 _delay=(microseconds); \
    register rtems_unsigned32 _tmp=123; \
    asm volatile( "0: \
                     nbcd      %0 ; \
                     nbcd      %0 ; \
                     dbf       %1,0b" \
                  : "=d" (_tmp), "=d" (_delay) \
                  : "0"  (_tmp), "1"  (_delay) ); \
  }

/* Constants */

/* Structures */

/*
 *  Device Driver Table Entries
 */

/*
 * NOTE: Use the standard Console driver entry
 */
 
/*
 * NOTE: Use the standard Clock driver entry
 */

/*
 * Real-Time Clock Driver Table Entry
 * NOTE: put this entry to the device driver table AFTER I2C bus driver!
 */
#define RTC_DRIVER_TABLE_ENTRY \
    { rtc_initialize, NULL, NULL, NULL, NULL, NULL }
extern rtems_device_driver rtc_initialize(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg
);

/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;

extern m68k_isr_entry M68Kvec[];   /* vector table address */

extern rtems_isr (*rtems_clock_hook)(rtems_vector_number);

/* functions */

void bsp_cleanup( void );

m68k_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

/*
 * Values assigned by link editor
 */
extern void *_RomBase, *_RamBase;

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif
/* end of include file */
