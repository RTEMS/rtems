/*  bsp.h
 *
 *  This include file contains all board IO definitions.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __BSP_h
#define __BSP_h

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

/*
 *  confdefs.h overrides for this BSP:
 *   - termios serial ports (defaults to 1)
 *   - Interrupt stack space is not minimum if defined.
 */

#if (HAS_PMC_PSC8)
#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS (4 + 4)
#else
#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS (4)
#endif
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (12 * 1024)
  
#ifdef ASM
/* Definition of where to store registers in alignment handler */
#define ALIGN_REGS 0x0140

#else
#include <rtems.h>
#include <console.h>
#include <clockdrv.h>
#include <iosupp.h>

/*
 *  We no longer support the first generation board.
 */

#include <gen2.h>

/*
 * The following macro calculates the Baud constant. For the Z8530 chip.
 *
 * Note: baud constant = ((clock frequency / Clock_X) / (2 * Baud Rate)) - 2
 *       for the Score603e ((10,000,000 / 16) / (2 * Baud Rate)) - 2
 */
#define _Score603e_Z8530_Baud( _frequency, _clock_by, _baud_rate  )   \
  ( (_frequency /( _clock_by * 2 * _baud_rate))  - 2)

#define Score603e_Z8530_Chip1_Baud( _value ) \
  _Score603e_Z8530_Baud( SCORE603E_85C30_1_CLOCK, \
     SCORE603E_85C30_1_CLOCK_X, _value ) 

#define Score603e_Z8530_Chip0_Baud( _value ) \
  _Score603e_Z8530_Baud( SCORE603E_85C30_0_CLOCK, \
     SCORE603E_85C30_0_CLOCK_X, _value ) 

#define Initialize_Board_ctrl_register()                         \
  *SCORE603E_BOARD_CTRL_REG = (*SCORE603E_BOARD_CTRL_REG |       \
                               SCORE603E_BRD_FLASH_DISABLE_MASK) \

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
 */

#define MUST_WAIT_FOR_INTERRUPT 1

#define Install_tm27_vector( _handler ) \
  set_vector( (_handler), PPC_IRQ_DECREMENTER, 1 )

#define Cause_tm27_intr()  \
  do { \
    unsigned32 _clicks = 8; \
    asm volatile( "mtdec %0" : "=r" ((_clicks)) : "r" ((_clicks)) ); \
  } while (0)


#define Clear_tm27_intr() \
  do { \
    unsigned32 _clicks = 0xffffffff; \
    asm volatile( "mtdec %0" : "=r" ((_clicks)) : "r" ((_clicks)) ); \
  } while (0)

#define Lower_tm27_intr() \
  do { \
    unsigned32 _msr = 0; \
    _ISR_Set_level( 0 ); \
    asm volatile( "mfmsr %0 ;" : "=r" (_msr) : "r" (_msr) ); \
    _msr |=  0x8002; \
    asm volatile( "mtmsr %0 ;" : "=r" (_msr) : "r" (_msr) ); \
  } while (0)


/* Constants */

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
 *  Information placed in the linkcmds file.
 */

extern int   RAM_START;
extern int   RAM_END;
extern int   RAM_SIZE;

extern int   PROM_START;
extern int   PROM_END;
extern int   PROM_SIZE;

extern int   CLOCK_SPEED;
extern int   CPU_PPC_CLICKS_PER_MS;

extern int   end;        /* last address in the program */

/*
 * How many libio files we want
 */
 
#define BSP_LIBIO_MAX_FDS       20

/* functions */

void bsp_start( void );

void bsp_cleanup( void );

rtems_isr_entry set_vector(                    /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
);

/*
 * spurious.c
 */
rtems_isr bsp_stub_handler(
   rtems_vector_number trap
); 
rtems_isr bsp_spurious_handler(
   rtems_vector_number trap
);
void bsp_spurious_initialize();

/* 
 * genvec.c
 */
rtems_isr_entry  set_EE_vector(
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector                    /* vector number      */
);
void initialize_external_exception_vector ();

/*
 * console.c
 */
void DEBUG_puts( char *string );

void BSP_fatal_return( void );

/*
 * Hwr_init.c
 */
void init_PCI();
void instruction_cache_enable ();
void data_cache_enable ();


void initialize_PCI_bridge ();
rtems_unsigned16 read_and_clear_irq ();
void set_irq_mask(
  rtems_unsigned16 value
);
rtems_unsigned16 get_irq_mask();

/*
 * universe.c
 */
void initialize_universe();

void set_irq_mask(
  rtems_unsigned16 value
);

rtems_unsigned16 get_irq_mask();

void unmask_irq( 
  rtems_unsigned16 irq_idx
);

void init_irq_data_register();

rtems_unsigned16 read_and_clear_PMC_irq(
  rtems_unsigned16    irq
);

rtems_boolean Is_PMC_IRQ(
  rtems_unsigned32   pmc_irq,
  rtems_unsigned16   status_word
);

rtems_unsigned16 read_and_clear_irq();

/*
 * FPGA.c
 */
void initialize_PCI_bridge ();

/* flash.c */

unsigned int SCORE603e_FLASH_Disable(
  rtems_unsigned32               unused
);
unsigned int SCORE603e_FLASH_verify_enable();
unsigned int SCORE603e_FLASH_Enable_writes(
  rtems_unsigned32               area        /* Unused  */
);

#define Convert_Endian_32( _data ) \
  ( ((_data&0x000000ff)<<24) | ((_data&0x0000ff00)<<8) |  \
    ((_data&0x00ff0000)>>8)  | ((_data&0xff000000)>>24) )

#define Convert_Endian_16( _data ) \
  ( ((_data&0x00ff)<<8) | ((_data&0xff00)>>8) )

extern rtems_configuration_table BSP_Configuration;     /* owned by BSP */
extern rtems_cpu_table           Cpu_table;             /* owned by BSP */
extern rtems_unsigned32          bsp_isr_level;

#endif /* ASM */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

