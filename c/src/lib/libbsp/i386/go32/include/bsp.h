/*  bsp.h
 * 
 *  This include file definitions related to the ibm386 (go32) board.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __IBMPC_h
#define __IBMPC_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <iosupp.h>

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
 *  Define the interrupt mechanism for Time Test 27
 *
 *  NOTE: Use a software interrupt for the i386.
 */
#define MUST_WAIT_FOR_INTERRUTPT 0
#define Install_tm27_vector( handler ) set_vector( (handler), 0x90, 1 )
#define Cause_tm27_intr()              asm volatile( "int $0x90" : : );
#define Clear_tm27_intr()
#define Lower_tm27_intr()


/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define delay( _microseconds )  { \
    rtems_unsigned32 _cnt = (_microseconds); \
    asm volatile ("0: nop; mov %0,%0; loop 0b" : "=c"(_cnt) : "0"(_cnt) ); \
}


/* Constants */

/* Assume color console */
#define	COLOR		1
#if COLOR
# define	GDC_REG_PORT	0x3d4
# define	GDC_VAL_PORT	0x3d5
# define	TVRAM		((unsigned short *)0xb8000L)
#else
# define	GDC_REG_PORT	0x3b4
# define	GDC_VAL_PORT	0x3b5
# define	TVRAM		((unsigned short *)0xb0000L)
#endif

/* White character attribute--works for MGA and CGA */
#define	WHITE	0x07

/* Ports for PC keyboard */
#define KBD_CTL 0x61
#define KBD_DATA 0x60
#define KBD_STATUS 0x64

/* Constants relating to the 8254 (or 8253) programmable interval timers */
 /*	Port address of the control port and timer channels */
/*
 * Macros for specifying values to be written into a mode register.
 */
#define IO_TIMER1	0x40
#define TIMER_CNTR0     (IO_TIMER1 + 0) /* timer 0 counter port */
#define TIMER_CNTR1     (IO_TIMER1 + 1) /* timer 1 counter port */
#define TIMER_CNTR2     (IO_TIMER1 + 2) /* timer 2 counter port */
#define TIMER_MODE      (IO_TIMER1 + 3) /* timer mode port */
#define         TIMER_SEL0      0x00    /* select counter 0 */
#define         TIMER_SEL1      0x40    /* select counter 1 */
#define         TIMER_SEL2      0x80    /* select counter 2 */
#define         TIMER_INTTC     0x00    /* mode 0, intr on terminal cnt */
#define         TIMER_ONESHOT   0x02    /* mode 1, one shot */
#define         TIMER_RATEGEN   0x04    /* mode 2, rate generator */
#define         TIMER_SQWAVE    0x06    /* mode 3, square wave */
#define         TIMER_SWSTROBE  0x08    /* mode 4, s/w triggered strobe */
#define         TIMER_HWSTROBE  0x0a    /* mode 5, h/w triggered strobe */
#define         TIMER_LATCH     0x00    /* latch counter for reading */
#define         TIMER_LSB       0x10    /* r/w counter LSB */
#define         TIMER_MSB       0x20    /* r/w counter MSB */
#define         TIMER_16BIT     0x30    /* r/w counter 16 bits, LSB first */
#define         TIMER_BCD       0x01    /* count in BCD */

/*	The internal tick rate in ticks per second */
#define		TIMER_TICK	1193182
#define		US_TO_TICK(us)	(((us)*105+44)/88)
#define		TICK_TO_US(tk)	(((tk)*88+52)/105)

/* Structures */

#ifdef IBMPC_INIT
#undef BSP_EXTERN
#define BSP_EXTERN
#else
#undef BSP_EXTERN
#define BSP_EXTERN extern
#endif

/* functions */

int _IBMPC_chrdy( char * ch );
int _IBMPC_inch( void );
void _IBMPC_outch( unsigned char );

/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;

#if 0
extern i386_IDT_slot Interrupt_descriptor_table[ 256 ];
extern i386_GDT_slot Global_descriptor_table[ 8192 ];
BSP_EXTERN unsigned short Idt[3];  /* Interrupt Descriptor Table Address */
BSP_EXTERN unsigned short Gdt[3];  /* Global Descriptor Table Address */
BSP_EXTERN unsigned int   Idt_base;
BSP_EXTERN unsigned int   Gdt_base;
#endif

/* routines */

i386_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

