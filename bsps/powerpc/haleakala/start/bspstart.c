/*
 *  This routine does the bulk of the system initialization.
 */

/*
 *  Author:     Thomas Doerfler <td@imd.m.isar.de>
 *              IMD Ingenieurbuero fuer Microcomputertechnik
 *
 *  COPYRIGHT (c) 1998 by IMD
 *
 *  Changes from IMD are covered by the original distributions terms.
 *  This file has been derived from the papyrus BSP:
 *
 *  Author:     Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Modifications for spooling console driver and control of memory layout
 *  with linker command file by
 *              Thomas Doerfler <td@imd.m.isar.de>
 *  for these modifications:
 *  COPYRIGHT (c) 1997 by IMD, Puchheim, Germany.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies. IMD makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/lib/libbsp/no_cpu/no_bsp/startup/bspstart.c:
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications for PPC405GP by Dennis Ehlin
 *
 *  Further modified for the PPC405EX Haleakala board by
 *  Michael Hamel ADInstruments Ltd May 2008
 */
#include <string.h>
#include <fcntl.h>

#include <rtems/bspIo.h>
#include <rtems/counter.h>

#include <bsp.h>
#include <bsp/bootcard.h>
#include <bsp/uart.h>
#include <bsp/irq.h>
#include <libcpu/powerpc-utility.h>
#include <bsp/vectors.h>
#include <ppc4xx/ppc405gp.h>
#include <ppc4xx/ppc405ex.h>

#include <stdio.h>
/*
 *  Driver configuration parameters
 */

/* Expected by clock.c */
uint32_t    bsp_clicks_per_usec;

/*-------------------- Haleakala-specific UART setup -------------------------*/

static void
EarlyUARTInit(int baudRate)
{
  volatile uint8_t* up = (uint8_t*)(BSP_UART_IOBASE_COM1);
  int divider = BSP_UART_BAUD_BASE / baudRate;
  up[LCR] = DLAB;    /* Access DLM/DLL */
  up[DLL] = divider & 0x0FF;
  up[DLM] = divider >> 8;
  up[LCR] = CHR_8_BITS;
  up[MCR] = DTR | RTS;
  up[FCR] = FIFO_EN | XMIT_RESET | RCV_RESET;
  up[THR] = '+';
}


static void
InitUARTClock(void)
{
  uint32_t reg;
  mfsdr(SDR0_UART0,reg);
  reg &= ~0x008000FF;
  reg |= 0x00800001;    /* Ext clock, div 1 */
  mtsdr(SDR0_UART0,reg);
}

static void GPIO_AlternateSelect(int bitnum, int source)
/* PPC405EX: select a GPIO function for the specified pin */
{
  int shift;
  unsigned long value, mask;
  GPIORegisters* gpioPtr = (GPIORegisters*)(GPIOAddress);

  shift = (31 - bitnum) & 0xF;
  value = (source & 3) << (shift*2);
  mask  = 3 << (shift*2);
  if (bitnum <= 15) {
    gpioPtr->OSRL = (gpioPtr->OSRL & ~mask) | value;
    gpioPtr->TSRL = (gpioPtr->TSRL & ~mask) | value;
  } else {
    gpioPtr->OSRH = (gpioPtr->OSRH & ~mask) | value;
    gpioPtr->TSRH = (gpioPtr->TSRH & ~mask) | value;
  }
}

static void Init_FPGA(void)
{
  /* Have to write to the FPGA to enable the UART drivers */
  /* Have to enable CS2 as an output in GPIO to get the FPGA working */
  mtebc(EBC0_B2CR,0xF0018000);  /* Set up CS2 at 0xF0000000 */
  mtebc(EBC0_B2AP,0x9400C800);
  GPIO_AlternateSelect(9,1);    /* GPIO9 = PerCS2 */
  {
    unsigned long *fpgaPtr = (unsigned long*)(0xF0000000);
    unsigned long n;
    n = *(fpgaPtr);
    n &= ~0x00100;    /* User LEDs on */
    n |=  0x30000;    /* UART 0 and 1 transcievers on! */
    *fpgaPtr = n;
  }
}

/*===================================================================*/

static void
DirectUARTWrite(const char c)
{
  volatile uint8_t* up = (uint8_t*)(BSP_UART_IOBASE_COM1);
  while ((up[LSR] & THRE) == 0) { ; }
  up[THR] = c;
}

/* We will provide our own printk output function as it may get used early */
BSP_output_char_function_type     BSP_output_char = DirectUARTWrite;
BSP_polling_getchar_function_type BSP_poll_char = NULL;

uint32_t _CPU_Counter_frequency(void)
{
  return bsp_clicks_per_usec * 1000000;
}

/*===================================================================*/

void bsp_start( void )
{
  /* Get the UART clock initialized first in case we call printk */

  InitUARTClock();
  Init_FPGA();
  EarlyUARTInit(115200);

  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type()
   * function store the result in global variables
   * so that it can be used later...
   */
  get_ppc_cpu_type();
  get_ppc_cpu_revision();

  /*
   *  initialize the device driver parameters
   */

  /* Set globals visible to clock.c */
  /* timebase register ticks/microsecond = CPU Clk in MHz */
  bsp_clicks_per_usec = 400;

  ppc_exc_initialize();

  /*
   * Install our own set of exception vectors
   */
  BSP_rtems_irq_mng_init(0);
}
