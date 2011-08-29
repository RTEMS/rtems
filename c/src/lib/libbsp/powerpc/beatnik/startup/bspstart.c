/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  Modified to support the MCP750.
 *  Modifications Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  Modified to support the Synergy VGM & Motorola PowerPC boards.
 *  (C) by Till Straumann, <strauman@slac.stanford.edu>, 2002, 2004, 2005
 * 
 *  Modified to support the mvme5500 BSP
 *  (C) by Kate Feng <feng1@bnl.gov>, 2003, 2004
 *      under the contract DE-AC02-98CH10886 with the Deaprtment of Energy
 *
 *  T. Straumann: 2005-2007; stolen again for 'beatnik'...
 */
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <rtems/system.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <rtems/bspIo.h>
#include <rtems/powerpc/powerpc.h>
/*#include <bsp/consoleIo.h>*/
#include <libcpu/spr.h>   /* registers.h is included here */
#include <bsp.h>
#include <bsp/uart.h>
#include <bsp/pci.h>
#include <bsp/gtreg.h>
#include <bsp/gt_timer.h>
#include <libcpu/bat.h>
#include <libcpu/pte121.h>
#include <libcpu/cpuIdent.h>
#include <bsp/vectors.h>
#include <bsp/vpd.h>

/* for RTEMS_VERSION :-( I dont like the preassembled string */
#include <rtems/sptables.h>

#ifdef __RTEMS_APPLICATION__
#undef __RTEMS_APPLICATION__
#endif

#define SHOW_MORE_INIT_SETTINGS

BSP_output_char_function_type     BSP_output_char = BSP_output_char_via_serial;
BSP_polling_getchar_function_type BSP_poll_char = NULL;

extern Triv121PgTbl BSP_pgtbl_setup(unsigned int *);
extern void BSP_pgtbl_activate(Triv121PgTbl);
extern void BSP_motload_pci_fixup(void);

extern unsigned long __rtems_end[];

/* We really shouldn't use these since MMUoff also sets IP;
 * nevertheless, during early init I don't care for now
 */
extern void MMUoff(void);
extern void MMUon(void);

extern uint32_t probeMemoryEnd(void);

SPR_RW(SPRG0)
SPR_RW(SPRG1)
SPR_RO(HID1)

/* Table of PLL multipliers for 7455/7457:
01000   2     00010   7.5       00000   11.5      00001   17
10000   3     11000   8         10111   12        00101   18
10100   4     01100   8.5       11111   12.5      00111   20
10110   5     01111   9         01011   13        01001   21
10010   5.5   01110   9.5       11100   13.5      01101   24
11010   6     10101   10        11001   14        11101   28
01010   6.5   10001   10.5      00011   15        00110   bypass
00100   7     10011   11        11011   16        11110   off
*/

/* Sorted according to CFG bits and multiplied by 2 it looks
 * like this (note that this is in sequential order, not
 * tabulated as above)
 */
signed char mpc7450PllMultByTwo[32] = {
23,       34,           15,           30,
14,       36,           2/*bypass*/,  40,
4,        42,           13,           26,
17,       48,           19,           18,
6,        21,           11,           22,
8,        20,           10,           24,
16,       28,           12,           32,
27,       56,           0/*off*/,     25,
};

uint32_t bsp_clicks_per_usec         = 0;

/*
 * Total memory using probing.
 */
unsigned int BSP_mem_size;

/*
 * PCI Bus Frequency
 */
unsigned int BSP_bus_frequency       = 0xdeadbeef;
/*
 * processor clock frequency
 */
unsigned int BSP_processor_frequency = 0xdeadbeef;

/*
 * Time base divisior (bus freq / TB clock)
 */
unsigned int BSP_time_base_divisor   = 4000; /* most 604+ CPUs seem to use this */

/* Board identification string */
char BSP_productIdent[20] = {0};
char BSP_serialNumber[20] = {0};

/* VPD appends an extra char -- what for ? */
char BSP_enetAddr0[7] = {0};
char BSP_enetAddr1[7] = {0};

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */

extern rtems_configuration_table Configuration;

char *rtems_progname;

/*
 *  Use the shared implementations of the following routines
 */
 
extern void bsp_pretasking_hook(void); 

#define CMDLINE_BUF_SIZE	2048

static char cmdline_buf[CMDLINE_BUF_SIZE];
char *BSP_commandline_string = cmdline_buf;

/* this routine is called early and must be safe with a not properly
 * aligned stack
 */
char *
save_boot_params(void *r3, void *r4, void* r5, char *cmdline_start, char *cmdline_end)
{
int             i=cmdline_end-cmdline_start;
	if ( i >= CMDLINE_BUF_SIZE )
		i = CMDLINE_BUF_SIZE-1;
	else if ( i < 0 )
		i = 0;
        memmove(cmdline_buf, cmdline_start, i);
	cmdline_buf[i]=0;
    return cmdline_buf;
}

static BSP_BoardType	board_type = Unknown;

BSP_BoardType
BSP_getBoardType( void )
{
	return board_type;
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
  unsigned char  *stack;
  char           *chpt;
  uint32_t       intrStackStart;
  uint32_t       intrStackSize;

  Triv121PgTbl	pt=0;

  VpdBufRec vpdData [] = {
	{ key: ProductIdent, instance: 0, buf: BSP_productIdent, buflen: sizeof(BSP_productIdent) - 1 },
	{ key: SerialNumber, instance: 0, buf: BSP_serialNumber, buflen: sizeof(BSP_serialNumber) - 1 },
	{ key: CpuClockHz,   instance: 0, buf: &BSP_processor_frequency, buflen: sizeof(BSP_processor_frequency)  },
	{ key: BusClockHz,   instance: 0, buf: &BSP_bus_frequency, buflen: sizeof(BSP_bus_frequency)  },
	{ key: EthernetAddr, instance: 0, buf: BSP_enetAddr0, buflen: sizeof(BSP_enetAddr0) },
	{ key: EthernetAddr, instance: 1, buf: BSP_enetAddr1, buflen: sizeof(BSP_enetAddr1) },
	VPD_END
  };

  /* T. Straumann: 4/2005
   *
   * Need to map the system registers early, so we can printk...
   * (otherwise we silently die)
   */
  /* map the PCI 0, 1 Domain I/O space, GT64260B registers
   * and the reserved area so that the size is the power of 2.
   */
  setdbat(7, BSP_DEV_AND_PCI_IO_BASE, BSP_DEV_AND_PCI_IO_BASE, BSP_DEV_AND_PCI_IO_SIZE, IO_PAGE);

  /* Intersperse messages with actions to help locate problems */
  printk("-----------------------------------------\n");

  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type() & friends functions
   * store the result in global variables so that it can be used latter...
   * This also verifies that we run on a known CPU.
   */
  get_ppc_cpu_type();
  get_ppc_cpu_revision();

  /* Make sure we detect a known host bridge */
  BSP_getDiscoveryVersion(/* assert detection */ 1);

  printk("Welcome to RTEMS %s\n", _RTEMS_version );

  /* Leave all caches as MotLoad left them. Seems to be fine */

  /*
   * the initial stack  has aready been set to this value in start.S
   * so there is no need to set it in r1 again... It is just for info
   * so that it can be printed without accessing R1.
   */
  __asm__ volatile("mr %0, 1":"=r"(stack));

  /* tag the bottom (T. Straumann 6/36/2001 <strauman@slac.stanford.edu>) */

  *((uint32_t *)stack) = 0;

  /*
   * Initialize the interrupt related settings
   * SPRG0 = interrupt nesting level count
   * SPRG1 = software managed IRQ stack
   *
   * This could be done latter (e.g in IRQ_INIT) but it helps to understand
   * some settings below...
   */
  intrStackStart = (uint32_t)__rtems_end;
  intrStackSize  = rtems_configuration_get_interrupt_stack_size();

  /*
   * Initialize default raw exception handlers. See vectors/vectors_init.c
   */
  ppc_exc_initialize(
		  PPC_INTERRUPT_DISABLE_MASK_DEFAULT,
		  intrStackStart,
		  intrStackSize
		  );

  printk("CPU: %s\n", get_ppc_cpu_type_name(current_ppc_cpu));

  /*
   * Initialize RTEMS IRQ system
   */
   BSP_rtems_irq_mng_init(0);

  BSP_vpdRetrieveFields(vpdData);

  if ( !strncmp(BSP_productIdent,"MVME5500",8) )
	board_type = MVME5500;
  else if ( !strncmp(BSP_productIdent,"MVME6100",8) )
	board_type = MVME6100;

  printk("Board Type: %s (S/N %s)\n",
		BSP_productIdent[0] ? BSP_productIdent : "n/a",
		BSP_serialNumber[0] ? BSP_serialNumber : "n/a");

  if ( 0xdeadbeef == BSP_bus_frequency ) {
	BSP_bus_frequency	= 133333333;
  	printk("Bus Clock Freq NOT FOUND in VPD; using %10u Hz\n",
		BSP_bus_frequency);
  } else {
  	printk("Bus Clock Freq:  %10u Hz\n",
		BSP_bus_frequency);
  }

  if ( 0xdeadbeef == BSP_processor_frequency ) {
	BSP_processor_frequency	 = BSP_bus_frequency/2;
	BSP_processor_frequency *= mpc7450PllMultByTwo[ (_read_HID1() >> (31-19)) & 31 ];
  }
  printk("CPU Clock Freq:  %10u Hz\n", BSP_processor_frequency);
	
  /* probe real memory size; if it's more than 256M we can't currently access it
   * since at this point only BAT-0 maps 0..256M
   */
  BSP_mem_size 				=  probeMemoryEnd();

  if ( (chpt = strstr(BSP_commandline_string,"MEMSZ=")) ) {
		char		*endp;
		uint32_t 	sz;
		chpt+=6 /* strlen("MEMSZ=") */;
		sz = strtoul(chpt, &endp, 0);
		if ( endp != chpt )
			BSP_mem_size = sz;
  }

  printk("Memory:          %10u bytes\n", BSP_mem_size);

  if ( BSP_mem_size > 0x10000000 ) {
	uint32_t s;
	if ( BSP_mem_size > 0x80000000 ) {
		BSP_mem_size = 0x80000000;
		printk("Memory clipped to 0x%08x for now, sorry\n", BSP_mem_size);
	}
	for ( s = 0x20000000; s < BSP_mem_size ; s<<=1)
		;
	MMUoff();
	/* since it's currently in use we must first surrender it */
	setdbat(0, 0, 0, 0, 0);
	setdbat(0, 0, 0, s, _PAGE_RW);
	MMUon();
  }

  printk("-----------------------------------------\n");

  /* Maybe not setup yet because of the warning message */

  /* Allocate and set up the page table mappings
   * This is only available on >604 CPUs.
   *
   * NOTE: This setup routine may modify the available memory
   *       size. It is essential to call it before
   *       calculating the workspace etc.
   */
  pt = BSP_pgtbl_setup(&BSP_mem_size);
  if (!pt)
     printk("WARNING: unable to setup page tables.\n");

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Now BSP_mem_size = 0x%x\n",BSP_mem_size); 
#endif

  /*
   * Set up our hooks
   * Make sure libc_init is done before drivers initialized so that
   * they can use atexit()
   */

  bsp_clicks_per_usec = BSP_bus_frequency/(BSP_time_base_divisor * 1000);

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Configuration.work_space_size = %x\n", Configuration.work_space_size); 
#endif

  /* Activate the page table mappings only after
   * initializing interrupts because the irq_mng_init()
   * routine needs to modify the text
   */           
  if ( pt ) {
#ifdef SHOW_MORE_INIT_SETTINGS
    printk("Page table setup finished; will activate it NOW...\n");
#endif
    BSP_pgtbl_activate(pt);
  }

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Going to start PCI buses scanning and initialization\n");
#endif 
  BSP_pci_initialize();

  /* need to tweak the motload setup */
  BSP_motload_pci_fixup();

  /* map 512M, 256 for PCI 256 for VME */
  setdbat(5,BSP_PCI_HOSE0_MEM_BASE, BSP_PCI_HOSE0_MEM_BASE, BSP_PCI_HOSE0_MEM_SIZE, IO_PAGE);
  setdbat(6,BSP_PCI_HOSE1_MEM_BASE, BSP_PCI_HOSE1_MEM_BASE, 0x10000000, IO_PAGE);

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Number of PCI buses found is : %d\n", pci_bus_count());
#endif

  /*
   * Initialize hardware timer facility (not used by BSP itself)
   * Needs PCI to identify discovery version...
   */
  BSP_timers_initialize();

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("MSR %x \n", _read_MSR());
  printk("Exit from bspstart\n");
#endif
}
