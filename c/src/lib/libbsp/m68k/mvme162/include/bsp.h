/*  bsp.h
 *
 *  This include file contains all MVME162 board IO definitions.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  Modifications of respective RTEMS file: COPYRIGHT (c) 1994.
 *  EISCAT Scientific Association. M.Savitski
 *
 *  This material is a part of the MVME162 Board Support Package
 *  for the RTEMS executive. Its licensing policies are those of the
 *  RTEMS above.
 *
 *  $Id$
 */

#ifndef __MVME162_h
#define __MVME162_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <iosupp.h>
#include <z8530.h>

/*
// Following defines must reflect the setup of the particular MVME162
//-----------------------------------
*/
#define GROUP_BASE_ADDRESS    0x0000F200
#define BOARD_BASE_ADDRESS    0x00000000
/* Base for local interrupters' vectors (with enable bit set) */
#define VECTOR_BASE           0x67800000
/* RAM limits */
#define RAM_START             0x00100000
#define RAM_END               0x00200000
/*
//-----------------------------------
*/
static volatile struct lcsr {
  unsigned long     slave_adr[2];
  unsigned long     slave_trn[2];
  unsigned long     slave_ctl;
  unsigned long     mastr_adr[4];
  unsigned long     mastr_trn;
  unsigned long     mastr_att;
  unsigned long     mastr_ctl;
  unsigned long     dma_ctl_1;
  unsigned long     dma_ctl_2;
  unsigned long     dma_loc_cnt;
  unsigned long     dma_vme_cnt;
  unsigned long     dma_byte_cnt;
  unsigned long     dma_adr_cnt;
  unsigned long     dma_status;
  unsigned long     to_ctl;
  unsigned long     timer_cmp_1;
  unsigned long     timer_cnt_1;
  unsigned long     timer_cmp_2;
  unsigned long     timer_cnt_2;
  unsigned long     board_ctl;
  unsigned long     prescaler_cnt;
  unsigned long     intr_stat;
  unsigned long     intr_ena;
  unsigned long     intr_soft_set;
  unsigned long     intr_clear;
  unsigned long     intr_level[4];
  unsigned long     vector_base;
} *lcsr = (void *) 0xFFF40000;

#define USE_CHANNEL_A   1                /* 1 = use channel A for console */
#define USE_CHANNEL_B   0                /* 1 = use channel B for console */

/* Constants */

#if   (USE_CHANNEL_A == 1)
  #define CONSOLE_CONTROL  0xFFF45005
  #define CONSOLE_DATA     0xFFF45007
#elif (USE_CHANNEL_B == 1)
  #define CONSOLE_CONTROL  0xFFF45001
  #define CONSOLE_DATA     0xFFF45003
#endif

/*
// The following registers are located in the VMEbus short
// IO space and respond to address modifier codes $29 and $2D.
// On FORCE SPARC CPU use address gcsr_vme and device /dev/vme16d32.
*/
static volatile struct gcsr {
  unsigned char       chip_revision;
  unsigned char       chip_id;
  unsigned char       lmsig;
  unsigned char       board_scr;
  unsigned short      gpr[6];
} *gcsr_vme = (void *) (GROUP_BASE_ADDRESS + BOARD_BASE_ADDRESS),
  *gcsr = (void *) 0xFFF40100;

static volatile unsigned short  *ipio[6] = {  (unsigned short *) 0xFFF58000,
                                              (unsigned short *) 0xFFF58100,
                                              (unsigned short *) 0xFFF58200,
                                              (unsigned short *) 0xFFF58300,
                                              (unsigned short *) 0xFFF58400,
                                              (unsigned short *) 0xFFF58500
                                           };

static volatile unsigned short  *ipid[6] = {  (unsigned short *) 0xFFF58080,
                                              (unsigned short *) 0xFFF58180,
                                              (unsigned short *) 0xFFF58280,
                                              (unsigned short *) 0xFFF58380,
                                              (unsigned short *) 0xFFF58080,
                                              (unsigned short *) 0xFFF58280
                                           };

static volatile struct ipic_space {
  struct sing {
    unsigned short    io_space[64];
    unsigned short    id_space[32];
    unsigned short    id_reptd[32];
  } single[4];
  struct twin {
    unsigned short    io_space[128];
    unsigned short    io_reptd[128];
  } twin[2];
} *ipic_space = (void *) 0xFFF58000;

static volatile struct ipic_csr {
  unsigned char     chip_id;
  unsigned char     chip_rev;
  unsigned char     res[2];
  unsigned short    a_31_16_base;
  unsigned short    b_31_16_base;
  unsigned short    c_31_16_base;
  unsigned short    d_31_16_base;
  unsigned char     a_23_16_size;
  unsigned char     b_23_16_size;
  unsigned char     c_23_16_size;
  unsigned char     d_23_16_size;
  unsigned short    a_intr_cnt;
  unsigned short    b_intr_cnt;
  unsigned short    c_intr_cnt;
  unsigned short    d_intr_cnt;
} *ipic_csr = (void *) 0xFFFBC000;

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
 *  NOTE: Not implemented
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler )

#define Cause_tm27_intr()

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

#ifdef 1626_INIT
#undef EXTERN
#define EXTERN
#else
#undef EXTERN
#define EXTERN extern
#endif

/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;

extern m68k_isr     M68Kvec[];   /* vector table address */

/* functions */

void bsp_cleanup( void );

m68k_isr set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
