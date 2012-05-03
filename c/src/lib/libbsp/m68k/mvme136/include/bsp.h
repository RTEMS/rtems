/*  bsp.h
 *
 *  This include file contains all MVME136 board IO definitions.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <rtems/clockdrv.h>
#include <rtems/console.h>
#include <rtems/iosupp.h>

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define rtems_bsp_delay( microseconds ) \
  { register uint32_t         _delay=(microseconds); \
    register uint32_t         _tmp=123; \
    __asm__ volatile( "0: \
                     nbcd      %0 ; \
                     nbcd      %0 ; \
                     dbf       %1,0b" \
                  : "=d" (_tmp), "=d" (_delay) \
                  : "0"  (_tmp), "1"  (_delay) ); \
  }

/* Constants */

#define RAM_START 0
#define RAM_END   0x100000

#define M681ADDR      0xfffb0040         /* address of the M68681 chip */
#define RXRDYB        0x01               /* status reg recv ready mask */
#define TXRDYB        0x04               /* status reg trans ready mask */
#define PARITYERR     0x20               /* status reg parity error mask */
#define FRAMEERR      0x40               /* status reg frame error mask */

#define FOREVER       1                  /* infinite loop */

/* Structures */

struct r_m681_info {
  char fill1[ 5 ];                       /* channel A regs ( not used ) */
  char isr;                              /* interrupt status reg */
  char fill2[ 2 ];                       /* counter regs (not used) */
  char mr1mr2b;                          /* MR1B and MR2B regs */
  char srb;                              /* status reg channel B */
  char fill3;                            /* do not access */
  char rbb;                              /* receive buffer channel B */
  char ivr;                              /* interrupt vector register */
};

struct w_m681_info {
  char fill1[ 4 ];                       /* channel A regs (not used) */
  char acr;                              /* auxillary control reg */
  char imr;                              /* interrupt mask reg */
  char fill2[ 2 ];                       /* counter regs (not used) */
  char mr1mr2b;                          /* MR1B and MR2B regs */
  char csrb;                             /* clock select reg */
  char crb;                              /* command reg */
  char tbb;                              /* transmit buffer channel B */
  char ivr;                              /* interrupt vector register */
};

extern rtems_isr_entry M68Kvec[];   /* vector table address */

/* functions */

rtems_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

#ifdef __cplusplus
}
#endif

#endif
