/*  bsp.h
 *
 *  This include file contains all board IO definitions.
 *
 *  This file includes definitions for the Intec SS555.
 *
 *
 *  SS555 port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  Derived from c/src/lib/libbsp/powerpc/mbx8xx/include/bsp.h:
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __SS555_h
#define __SS555_h

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <rtems/console.h>
#include <rtems/clockdrv.h>
#include <mpc5xx.h>
#include <mpc5xx/console.h>
#include <libcpu/vectors.h>
#include <bsp/irq.h>

/*
 * Clock definitions
 */

#define BSP_CRYSTAL_HZ 	 4000000	/* crystal frequency, Hz */
#define BSP_CLOCK_HZ	40000000	/* CPU clock frequency, Hz

/*
 * I/O definitions
 *
 * The SS555 board includes a CPLD to control on-board features and
 * off-board devices.
 */
typedef struct cpld_ {
  rtems_unsigned8	cs3a[32];		/* Chip select 3A */
  rtems_unsigned8	pad0[0x200000 - 0x000020];

  rtems_unsigned8	cs3b[32];		/* Chip select 3B */
  rtems_unsigned8	pad2[0x400000 - 0x200020];

  rtems_unsigned8	cs3c[32];		/* Chip select 3C */
  rtems_unsigned8	pad4[0x600000 - 0x400020];

  rtems_unsigned8	cs3d[32];		/* Chip select 3D */
  rtems_unsigned8	pad6[0x800000 - 0x600020];

  rtems_unsigned8	serial_ints;	/* Enable/disable serial interrupts */
  rtems_unsigned8	serial_resets;	/* Enable/disable serial resets */
  rtems_unsigned8	serial_ack;	/* Acknowledge serial transfers */
  rtems_unsigned8	pad8[0xA00000 - 0x800003];

  rtems_unsigned8	iflash_writess;	/* Enable/disable internal-flash writes */
  rtems_unsigned8	nflash_writess;	/* Enable/disable NAND-flash writes */
  rtems_unsigned8	padA[0xC00000 - 0xA00002];
} cpld_t;

extern volatile cpld_t cpld;              /* defined in linkcmds */

/*
 * Stuff for Time Test 27
 *
 * The following require that IRQ7 be jumpered to ground.  On the SS555,
 * this can be done by shorting together CN5 pin 48 and CN5 pin 50.
 */

#define MUST_WAIT_FOR_INTERRUPT 1

#define Declarations_for_tm27						\
void tm27On()								\
{									\
}									\
									\
void tm27Off()								\
{									\
}									\
									\
int tm27IsOn()								\
{									\
  return 1;								\
}									\
									\
rtems_irq_connect_data tm27IrqData = {					\
  CPU_USIU_EXT_IRQ_7,							\
  (rtems_irq_hdl)0,							\
  (rtems_irq_enable)tm27On,						\
  (rtems_irq_disable)tm27Off,						\
  (rtems_irq_is_enabled)tm27IsOn					\
};

#define Install_tm27_vector( handler )					\
{									\
  usiu.siel |= (1 << 17); 						\
  usiu.sipend |= (1 << 17); 						\
									\
  tm27IrqData.hdl = (rtems_irq_hdl)handler;				\
  BSP_install_rtems_irq_handler (&tm27IrqData);				\
}

#define Cause_tm27_intr()						\
{									\
  usiu.siel &= ~(1 << 17); 						\
}

#define Clear_tm27_intr()						\
{									\
  usiu.siel |= (1 << 17); 						\
  usiu.sipend |= (1 << 17); 						\
}

#define Lower_tm27_intr()						\
{									\
  ppc_cached_irq_mask |= (1 << 17);					\
  usiu.simask = ppc_cached_irq_mask;					\
}

/* miscellaneous stuff assumed to exist */

extern rtems_configuration_table BSP_Configuration;

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
 * How many libio files we want
 */

#define BSP_LIBIO_MAX_FDS       20

/* functions */

void bsp_cleanup( void );

rtems_isr_entry set_vector(                    /* returns old vector */
  rtems_isr_entry     handler,                  /* isr routine        */
  rtems_vector_number vector,                   /* vector number      */
  int                 type                      /* RTEMS or RAW intr  */
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
