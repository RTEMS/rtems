/*  bsp.h
 *
 *  This include file contains all MVME147 board IO definitions.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  MVME147 port for TNI - Telecom Bretagne
 *  by Dominique LE CAMPION (Dominique.LECAMPION@enst-bretagne.fr)
 *  May 1996
 *
 *  $Id$
 */

#ifndef __MVME147_h
#define __MVME147_h

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <clockdrv.h>
#include <console.h>
#include <iosupp.h>

/*
 *  confdefs.h overrides for this BSP:
 *   - number of termios serial ports (defaults to 1)
 *   - Interrupt stack space is not minimum if defined.
 */

/* #define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 2 */
#define CONFIGURE_INTERRUPT_STACK_MEMORY  (4 * 1024)

/* Constants */

#define RAM_START 0x00005000
#define RAM_END   0x00400000

  /* MVME 147 Peripheral controller chip 
     see MVME147/D1, 3.4 */

struct pcc_map {
  /* 32 bit registers */
  rtems_unsigned32 dma_table_address;            /* 0xfffe1000 */
  rtems_unsigned32 dma_data_address;             /* 0xfffe1004 */
  rtems_unsigned32 dma_bytecount;                /* 0xfffe1008 */
  rtems_unsigned32 dma_data_holding;             /* 0xfffe100c */

  /* 16 bit registers */
  rtems_unsigned16 timer1_preload;               /* 0xfffe1010 */
  rtems_unsigned16 timer1_count;                 /* 0xfffe1012 */
  rtems_unsigned16 timer2_preload;               /* 0xfffe1014 */
  rtems_unsigned16 timer2_count;                 /* 0xfffe1016 */

  /* 8 bit registers */
  rtems_unsigned8 timer1_int_control;            /* 0xfffe1018 */
  rtems_unsigned8 timer1_control;                /* 0xfffe1019 */
  rtems_unsigned8 timer2_int_control;            /* 0xfffe101a */
  rtems_unsigned8 timer2_control;                /* 0xfffe101b */

  rtems_unsigned8 acfail_int_control;            /* 0xfffe101c */
  rtems_unsigned8 watchdog_control;              /* 0xfffe101d */

  rtems_unsigned8 printer_int_control;           /* 0xfffe101e */
  rtems_unsigned8 printer_control;               /* 0xfffe102f */

  rtems_unsigned8 dma_int_control;               /* 0xfffe1020 */
  rtems_unsigned8 dma_control;                   /* 0xfffe1021 */
  rtems_unsigned8 bus_error_int_control;         /* 0xfffe1022 */
  rtems_unsigned8 dma_status;                    /* 0xfffe1023 */
  rtems_unsigned8 abort_int_control;             /* 0xfffe1024 */
  rtems_unsigned8 table_address_function_code;   /* 0xfffe1025 */
  rtems_unsigned8 serial_port_int_control;       /* 0xfffe1026 */
  rtems_unsigned8 general_purpose_control;       /* 0xfffe1027 */
  rtems_unsigned8 lan_int_control;               /* 0xfffe1028 */
  rtems_unsigned8 general_purpose_status;        /* 0xfffe1029 */
  rtems_unsigned8 scsi_port_int_control;         /* 0xfffe102a */
  rtems_unsigned8 slave_base_address;            /* 0xfffe102b */
  rtems_unsigned8 software_int_1_control;        /* 0xfffe102c */
  rtems_unsigned8 int_base_vector;               /* 0xfffe102d */
  rtems_unsigned8 software_int_2_control;        /* 0xfffe102e */
  rtems_unsigned8 revision_level;                /* 0xfffe102f */
};

#define pcc      ((volatile struct pcc_map * const) 0xfffe1000)

#define z8530 0xfffe3001


/* interrupt vectors - see MVME146/D1 4.14 */
#define PCC_BASE_VECTOR        0x40 /* First user int */
#define SCC_VECTOR             PCC_BASE_VECTOR+3
#define TIMER_1_VECTOR         PCC_BASE_VECTOR+8
#define TIMER_2_VECTOR         PCC_BASE_VECTOR+9  
#define SOFT_1_VECTOR          PCC_BASE_VECTOR+10
#define SOFT_2_VECTOR          PCC_BASE_VECTOR+11 

#define USE_CHANNEL_A   1                /* 1 = use channel A for console */
#define USE_CHANNEL_B   0                /* 1 = use channel B for console */

#if (USE_CHANNEL_A == 1)
#define CONSOLE_CONTROL  0xfffe3002
#define CONSOLE_DATA     0xfffe3003
#elif (USE_CHANNEL_B == 1)
#define CONSOLE_CONTROL  0xfffe3000
#define CONSOLE_DATA     0xfffe3001
#endif



#define FOREVER       1                  /* infinite loop */

#ifdef M147_INIT
#undef EXTERN
#define EXTERN
#else
#undef EXTERN
#define EXTERN extern
#endif

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
 *  NOTE: Use the MPCSR vector for the MVME147
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) set_vector( (handler), \
						   SOFT_1_VECTOR, 1 )

#define Cause_tm27_intr()  pcc->software_int_1_control = 0x0c
  /* generate level 4 sotware int. */

#define Clear_tm27_intr()  pcc->software_int_1_control = 0x00

#define Lower_tm27_intr()


/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;

extern m68k_isr_entry M68Kvec[];   /* vector table address */

/*
 *  Device Driver Table Entries
 */

/*
 * NOTE: Use the standard Console driver entry
 */
 
/*
 * NOTE: Use the standard Clock driver entry
 */

/* functions */

void bsp_cleanup( void );

m68k_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

