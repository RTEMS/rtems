/*  bsp_start()
 *
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  INPUT:  NONE
 *
 *  OUTPUT: NONE
 *
 *  Author:	Thomas Doerfler <td@imd.m.isar.de>
 *              IMD Ingenieurbuero fuer Microcomputertechnik
 *
 *  COPYRIGHT (c) 1998 by IMD
 *
 *  Changes from IMD are covered by the original distributions terms.
 *  This file has been derived from the papyrus BSP:
 *
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
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
 *
 *  $Id$
 */
#include <string.h>
#include <fcntl.h>

#include <bsp.h>
#include <bsp/uart.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <bsp/irq.h>
#include <rtems/bspIo.h>
#include <libcpu/cpuIdent.h>
#include <rtems/powerpc/powerpc.h>
#include <bsp/ppc_exc_bspsupp.h>
#include <ppc4xx/ppc405gp.h>
#include <ppc4xx/ppc405ex.h>

#include <stdio.h>

/*
 *  Driver configuration parameters
 */

/* Expected by clock.c */
uint32_t	bsp_clicks_per_usec;
bool		bsp_timer_internal_clock;   /* true, when timer runs with CPU clk */
uint32_t	bsp_timer_least_valid;
uint32_t	bsp_timer_average_overhead;


/*      Initialize whatever libc we are using
 *      called from postdriver hook
 */

void bsp_postdriver_hook(void);
void bsp_libc_init( void *, uint32_t, int );

/*
 *
 *  bsp_predriver_hook
 *
 *  Before drivers are setup.
 */

void bsp_predriver_hook(void)
{
  
}

/*
 *  Function:   bsp_pretasking_hook
 *  Created:    95/03/10
 *
 *  Description:
 *      BSP pretasking hook.  Called just before drivers are initialized.
 *      Used to setup libc and install any BSP extensions.
 *
 *  NOTES:
 *      Must not use libc (to do io) from here, since drivers are
 *      not yet initialized.
 *
 */


extern uint8_t	_RAMEnd;		/* Defined in linkcmds */

void bsp_pretasking_hook(void)
/* Initialise libc with the address and size of the heap, which runs
   from the end of the RTEMS workspace to the top of RAM */
{
    uint32_t heap_start;
 
    heap_start = ( (uint32_t)Configuration.work_space_start +
					rtems_configuration_get_work_space_size() );

    bsp_libc_init((void *)heap_start, (uint32_t)(&_RAMEnd) - heap_start, 0);

	#ifdef RTEMS_DEBUG
		rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
	#endif
}

/*-------------------- Haleakala-specific UART setup -------------------------*/

static void
EarlyUARTInit(int baudRate)
{
	volatile uint8_t* up = (uint8_t*)(BSP_UART_IOBASE_COM1);
	int divider = BSP_UART_BAUD_BASE / baudRate;
	up[LCR] = DLAB;		/* Access DLM/DLL */
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
	reg |= 0x00800001;		/* Ext clock, div 1 */
	mtsdr(SDR0_UART0,reg);
}

void GPIO_AlternateSelect(int bitnum, int source)
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

void Init_FPGA(void)
{
	/* Have to write to the FPGA to enable the UART drivers */
	/* Have to enable CS2 as an output in GPIO to get the FPGA working */
	mtebc(EBC0_B2CR,0xF0018000);	/* Set up CS2 at 0xF0000000 */
	mtebc(EBC0_B2AP,0x9400C800);
	GPIO_AlternateSelect(9,1);		/* GPIO9 = PerCS2 */
	{
		unsigned long *fpgaPtr = (unsigned long*)(0xF0000000);
		unsigned long n;
		n = *(fpgaPtr);
		n &= ~0x00100;		/* User LEDs on */
		n |=  0x30000;		/* UART 0 and 1 transcievers on! */
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
	if (c=='\n')
		DirectUARTWrite('\r');
}

/* We will provide our own printk output function as it may get used early */
BSP_output_char_function_type BSP_output_char = DirectUARTWrite;


/*===================================================================*/


/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */


void bsp_start( void )
{
	LINKER_SYMBOL(intrStack_start);
	LINKER_SYMBOL(intrStack_size);
	ppc_cpu_id_t myCpu;
	ppc_cpu_revision_t myCpuRevision;

	/* Get the UART clock initialized first in case we call printk */

	InitUARTClock();
	Init_FPGA();
	EarlyUARTInit(115200);

	/*
	* Get CPU identification dynamically. Note that the get_ppc_cpu_type() 
	* function store the result in global variables 
	* so that it can be used later...
	*/
	myCpu 	    = get_ppc_cpu_type();
	myCpuRevision = get_ppc_cpu_revision();

	/*
	*  initialize the device driver parameters
	*/

	/* Set globals visible to clock.c */
	bsp_clicks_per_usec = 400;	/* timebase register ticks/microsecond = CPU Clk in MHz */
	bsp_timer_internal_clock  = TRUE;
	bsp_timer_average_overhead = 2;
	bsp_timer_least_valid = 3;
	
	/*
	* Initialize default raw exception handlers. 
	*/
	ppc_exc_initialize(
		PPC_INTERRUPT_DISABLE_MASK_DEFAULT,
			   (uint32_t) intrStack_start,
			   (uint32_t) intrStack_size);

	/*
	* Install our own set of exception vectors
	*/
	BSP_rtems_irq_mng_init(0);

	/*
	*  Allocate the memory for the RTEMS Work Space.  This can come from
	*  a variety of places: hard coded address, malloc'ed from outside
	*  RTEMS world (e.g. simulator or primitive memory manager), or (as
	*  typically done by stock BSPs) by subtracting the required amount
	*  of work space from the last physical address on the CPU board.
	*/
	/* In this case we allocate space at an address defined in linkcmds
	   which points to a block above the stack and below the heap */
	{
		extern uint8_t	_WorkspaceStart;	
		Configuration.work_space_start = &_WorkspaceStart;
	}

}

void BSP_ask_for_reset(void)
{
  printk("system stopped, press RESET");
  while(1) {};
}

void BSP_panic(char *s)
{
  printk("%s PANIC %s\n",_RTEMS_version, s);
  BSP_ask_for_reset();
}

void _BSP_Fatal_error(unsigned int v)
{
  printk("%s PANIC ERROR %x\n",_RTEMS_version, v);
  BSP_ask_for_reset();
}


