/*
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
 *
 */
/*  bspstart.c
 *
 *  This set of routines starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before any of these are invoked.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: 
 */

#include <string.h>

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <pci.h>

unsigned char ucSystemType;
unsigned char ucBoardRevMaj;
unsigned char ucBoardRevMin;
unsigned long ulMemorySize;
unsigned long ulCpuBusClock;

/*
 * The bus speed is expressed in MHz
 */
static unsigned long ulBusSpeed[] = {
	56250000,
	60000000,
	64300000,
	66666667,
	75000000,
	83333333,
	100000000,
	66666667
};

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */
 
extern rtems_configuration_table  Configuration;
rtems_configuration_table         BSP_Configuration;

rtems_cpu_table   Cpu_table;
rtems_unsigned32  bsp_isr_level;

static int stdin_fd, stdout_fd, stderr_fd;

/*
 * End of RTEMs image imported from linker
 */
extern int end;

/*
 *  Use the shared implementations of the following routines
 */

void bsp_postdriver_hook(void);
void bsp_libc_init( void *, unsigned32, int );

/*
 *  bsp_pretasking_hook
 *
 *  BSP pretasking hook.  Called just before drivers are initialized.
 *  Used to setup libc and install any BSP extensions.
 */

void bsp_pretasking_hook(void)
{
        rtems_unsigned32 heap_start;
        rtems_unsigned32 heap_size;

        heap_start = (rtems_unsigned32) &end;
        if (heap_start & (CPU_ALIGNMENT-1))
          heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

        heap_size = BSP_Configuration.work_space_start - (void *)&end;
        heap_size &= 0xfffffff0;  /* keep it as a multiple of 16 bytes */

        bsp_libc_init((void *) heap_start, heap_size, 0);

	/*
	 * Initialise RTC hooks based on system type
	 */
	InitializeRTC();

	/*
	 * Initialise NvRAM hooks based on system type
	 */
	InitializeNvRAM();

	/*
	 * Initialise the PCI bus(ses)
	 */
	InitializePCI();

	/*
	 * Initialize the Universe PCI-VME bridge
	 */
	InitializeUniverse();


#ifdef RTEMS_DEBUG
  rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif

}

/*
 *  bsp_std_close
 *
 *  Simple routine to close all standard IO streams.
 */

void bsp_std_close( void )
{
	close(stdin_fd);
	close(stdout_fd);
	close(stderr_fd);
}
 

/*
 *  bsp_predriver_hook
 *
 *  Before drivers are setup.
 */ 
void bsp_predriver_hook(void)
{
  /* bsp_spurious_initialize; ??*/
  initialize_external_exception_vector();
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
	unsigned char *work_space_start;
	unsigned char ucBoardRev, ucMothMemType, ucEquipPres1, ucEquipPres2;
	unsigned16	usPVR=0;
	unsigned8	ucTempl, ucTemph;
	unsigned8	ucBanksPresent;
	unsigned8	ucSimmPresent;
	unsigned32	ulCurBank, ulTopBank;

	/*
	 * Determine system type
	 */
	inport_byte(&((PPLANARREGISTERS)0)->MotherboardMemoryType, ucMothMemType);
	inport_byte(&((PPLANARREGISTERS)0)->SimmPresent, ucSimmPresent);

	inport_byte(&((PPLANARREGISTERS)0)->EquipmentPresent1, ucEquipPres1);
	inport_byte(&((PPLANARREGISTERS)0)->EquipmentPresent2, ucEquipPres2);
	ucSystemType=((ucMothMemType&0x03)<<1) | ((ucEquipPres1&0x80)>>7);
	ucSystemType^=7;

	/*
	 * Determine board revision for use by rev. specific code
	 */
	inport_byte(&((PPLANARREGISTERS)0)->BoardRevision, ucBoardRev);
	ucBoardRevMaj=ucBoardRev>>5;
	ucBoardRevMin=ucBoardRev&0x1f;

	/*
	 * Determine the memory size by reading the end address for top
	 * assigned bank in the memory controller
	 */
	(void)PCIConfigRead8(0,0,0,0xa0, &ucBanksPresent);
	for(ulCurBank=0;ulCurBank<8;ulCurBank++)
	{
		if((ucBanksPresent>>ulCurBank)&0x01)
		{
			ulTopBank=ulCurBank;
		}
	}

	(void)PCIConfigRead8(0,0,0,0x90+ulTopBank, &ucTempl);
	(void)PCIConfigRead8(0,0,0,0x98+ulTopBank, &ucTemph);
	ulMemorySize=(ucTempl+(ucTemph<<8)+1)<<20;
#if PPCN_60X_USE_DINK
	ulMemorySize=0x01fe0000;
#endif

	/*
	 * Determine processor bus clock
	 */
	asm volatile ("mfpvr %0" : "=r" ((usPVR)) : "0" ((usPVR)));

	/*
	 * Determine processor internal clock
	 */
	if(ucSystemType==SYS_TYPE_PPC4)
	{
		if(((ucBoardRevMaj==1) && (ucBoardRevMin==0)) ||
		   ((ucSimmPresent&0x40)==0))
		{
			/*
			 * Rev. 1A is always 66MHz
			 */
			ulCpuBusClock=66666667;
		}
		else
		{
			ulCpuBusClock=83333333;
		}
	}
	else if((((usPVR>>16)==MPC603e) && (ucSystemType!=SYS_TYPE_PPC1)) ||
	        ((usPVR>>16)==MPC603ev) ||
	        ((usPVR>>16)==MPC604e))
	{
		ulCpuBusClock=ulBusSpeed[(ucEquipPres2&0x1c)>>2];
	}
	else
	{
		if(((ucSystemType>SYS_TYPE_PPC1) || (ucBoardRevMaj>=5)) &&
		   (ucEquipPres1&0x08))
		{
			/*
			 * 66 MHz bus clock for 005 if indicated
			 */
			ulCpuBusClock=66666667;
		}
		else
		{
			/*
			 * 33 MHz bus clock for 004 always
			 */
			ulCpuBusClock=33333333;
		}
	}


  /*
   *  Allocate the memory for the RTEMS Work Space.  This can come from
   *  a variety of places: hard coded address, malloc'ed from outside
   *  RTEMS world (e.g. simulator or primitive memory manager), or (as
   *  typically done by stock BSPs) by subtracting the required amount
   *  of work space from the last physical address on the CPU board.
   */

  work_space_start = 
    (unsigned char *)ulMemorySize - BSP_Configuration.work_space_size;

  if ( work_space_start <= (unsigned char *)&end ) {
    DEBUG_puts( "bspstart: Not enough RAM!!!\n" );
    bsp_cleanup();
  }

  BSP_Configuration.work_space_start = work_space_start;

  /*
   *  initialize the CPU table for this BSP
   */

  Cpu_table.exceptions_in_RAM = TRUE;
  Cpu_table.pretasking_hook = bsp_pretasking_hook;    /* init libc, etc. */
  Cpu_table.predriver_hook = bsp_predriver_hook; 
  Cpu_table.postdriver_hook = bsp_postdriver_hook;
  Cpu_table.do_zero_of_workspace = TRUE;
  Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;
  Cpu_table.clicks_per_usec = ulCpuBusClock/4000000;


}

