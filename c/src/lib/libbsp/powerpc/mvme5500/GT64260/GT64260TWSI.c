/* GT64260TWSI.c : Two-Wire Serial Interface (TWSI) support for the GT64260
 *
 * Copyright (c) 2004, Brookhaven National Laboratory and
 *                 Shuchen Kate Feng <feng1@bnl.gov>
 * All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution.
 *
 * See section 24:TWSI interface of "the GT-64260B System Controller
 * for powerPc Processors Data Sheet".
 *
 * For full TWSI protocol description look in Philips Semiconductor
 * TWSI spec.
 *
 * We need it to read out I2C devices used for the MVME5500
 * (eg. the memory SPD and VPD).
 *
 */
#include <libcpu/spr.h>  /*registers.h included here for rtems_bsp_delay()*/
#include <libcpu/io.h>
#include <rtems/bspIo.h>	    /* printk */

#include "bsp/gtreg.h"
#include "bsp/GT64260TWSI.h"

#define MAX_LOOP 100

#define TWSI_DEBUG 0

static int TWSI_initFlg = 0;	/* TWSI Initialization Flag */

void GT64260TWSIinit(void)
{

  if ( !TWSI_initFlg) {
#if TWSI_DEBUG
     printk("GT64260TWSIinit(");
#endif
     outl( 0, TWSI_SFT_RST); /* soft reset */
     rtems_bsp_delay(1000);

     /* See 24.2.5 : Assume bus speed is 133MHZ
      * Try to be close to the default frequency : 62.5KHZ
      * value 0x2c: 69.27 KHz TWSI bus clock
      */
     outl(0x2c, TWSI_BAUDE_RATE);
     rtems_bsp_delay(1000);

     /* Set Acknowledge and enable TWSI in the Control register */
     outl(0x44, TWSI_CTRL);
     rtems_bsp_delay(4000);
     TWSI_initFlg = 1;
#if TWSI_DEBUG
     printk(")\n");
#endif
  }
}

/* return the interrupt flag */
int GT64260TWSIintFlag(void)
{
  unsigned int loop;

  for (loop = 0; loop < MAX_LOOP; loop++ ) {
    /* Return 1 if the interrupt flag is set */
      if (inl(TWSI_CTRL) & TWSI_INTFLG) return(1);
      rtems_bsp_delay(1000);
  }
  return(0);
}

int GT64260TWSIstop(void)
{

#if TWSI_DEBUG
  printk("GT64260TWSIstop(");
#endif

  outl((inl(TWSI_CTRL) | TWSI_STOP), TWSI_CTRL);
  rtems_bsp_delay(1000);

  /* Check if interrupt flag bit is set*/
  if (GT64260TWSIintFlag()) {
     outl((inl( TWSI_CTRL) & ~TWSI_INTFLG), TWSI_CTRL);
     rtems_bsp_delay(1000);
#if TWSI_DEBUG
     printk(")\n");
#endif
     return(0);
  }
#if TWSI_DEBUG
     printk("NoIntFlag\n");
#endif
  return(-1);
}

int GT64260TWSIstart(void)
{
  unsigned int loop;
  unsigned int status;

#if TWSI_DEBUG
  printk("GT64260TWSIstart(");
#endif
  /* Initialize the TWSI interface */
  GT64260TWSIinit();

  /* set the start bit */
  outl((TWSI_START | TWSI_TWSIEN), TWSI_CTRL);
  rtems_bsp_delay(1000);

  if (GT64260TWSIintFlag()) {
     /* Check for completion of START sequence */
     for (loop = 0; loop<MAX_LOOP; loop++ ) {
         /* if (start condition transmitted) ||
          *    (repeated start condition transmitted )
          */
         if (((status= inl( TWSI_STATUS)) == 8) || (status == 0x10)) {
#if TWSI_DEBUG
             printk(")");
#endif
             return(0);
         }
         rtems_bsp_delay(1000);
     }
  }
  /* if loop ends or intFlag ==0 */
  GT64260TWSIstop();
  return(-1);
}

int GT64260TWSIread(unsigned char * pData, int lastByte)
{
  unsigned int loop;

#if TWSI_DEBUG
  printk("GT64260TWSIread(");
#endif
  /* Clear INTFLG and set ACK and ENABLE bits */
  outl((TWSI_ACK | TWSI_TWSIEN), TWSI_CTRL);
  rtems_bsp_delay(1000);

  if (GT64260TWSIintFlag()) {
     for (loop = 0; loop< MAX_LOOP; loop++) {
       /* if Master received read data, acknowledge transmitted */
       if ( (inl( TWSI_STATUS) == 0x50)) {
	   *pData = (unsigned char) inl( TWSI_DATA);
   	   rtems_bsp_delay(1500);

           /* Clear INTFLAG and set Enable bit only */
	   if (lastByte) outl(TWSI_TWSIEN, TWSI_CTRL);
	   rtems_bsp_delay(1500);
#if TWSI_DEBUG
  printk(")\n");
#endif
	   return(0);
	}
	rtems_bsp_delay(1000);
     } /* end for */
  }
  /* if loop ends or intFlag ==0 */
  GT64260TWSIstop();
  return(-1);
}

/* do a TWSI write cycle on the TWSI bus*/
int GT64260TWSIwrite(unsigned char Data)
{
  unsigned int loop;
  unsigned int status;

#if TWSI_DEBUG
  printk("GT64260TWSIwrite(");
#endif
  /* Write data into the TWSI data register */
  outl(((unsigned int) Data), TWSI_DATA);
  rtems_bsp_delay(1000);

  /* Clear INTFLG in the control register to drive data onto TWSI bus */
  outl(0, TWSI_CTRL);
  rtems_bsp_delay(1000);

  if (GT64260TWSIintFlag() ) {
     for (loop = 0; loop< MAX_LOOP; loop++) {
	 rtems_bsp_delay(1000);
	 /* if address + write bit transmitted, acknowledge not received */
	 if ( (status = inl( TWSI_STATUS)) == 0x20) {
            /* No device responding, generate STOP and return -1 */
	   printk("no device responding\n");
	    GT64260TWSIstop();
	    return(-1);
	 }
         /* if (address + write bit transmitted, acknowledge received)
	  *    (Master transmmitted data byte, acknowledge received)
          *    (address + read bit transmitted, acknowledge received)
          */
	 if ((status == 0x18)||(status == 0x28)||(status == 0x40)) {
#if TWSI_DEBUG
	      printk(")\n");
#endif
	      return(0);
         }
	 rtems_bsp_delay(1000);
     } /* end for */
  }
  printk("No correct status, timeout\n");
  GT64260TWSIstop();
  return(-1);
}
