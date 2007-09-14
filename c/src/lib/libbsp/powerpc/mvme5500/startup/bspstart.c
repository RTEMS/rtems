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
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modified to support the MCP750.
 *  Modifications Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  Modified to support the Synergy VGM & Motorola PowerPC boards
 *  (C) by Till Straumann, <strauman@slac.stanford.edu>, 2002, 2004, 2005
 *
 *  Modified to support the MVME5500 board.
 *  Also, the settings of L1, L2, and L3 caches is not necessary here.
 *  (C) by Brookhaven National Lab., S. Kate Feng <feng1@bnl.gov>, 2003, 2004, 2005
 *  
 *  $Id$
 */

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <rtems/system.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <rtems/powerpc/powerpc.h>

#include <libcpu/spr.h>   /* registers.h is included here */
#include <bsp.h>
#include <bsp/uart.h>
#include <bsp/pci.h>
#include <libcpu/bat.h>
#include <libcpu/pte121.h>
#include <libcpu/cpuIdent.h>
#include <bsp/vectors.h>
#include <bsp/bspException.h>

#include <rtems/bspIo.h>
#include <rtems/sptables.h>

#ifdef __RTEMS_APPLICATION__
#undef __RTEMS_APPLICATION__
#endif

/*
#define SHOW_MORE_INIT_SETTINGS
#define SHOW_LCR1_REGISTER
#define SHOW_LCR2_REGISTER
#define SHOW_LCR3_REGISTER
#define CONF_VPD
*/

/* there is no public Workspace_Free() variant :-( */
#include <rtems/score/wkspace.h>

BSP_output_char_function_type BSP_output_char = BSP_output_char_via_serial;

extern void _return_to_ppcbug();
extern unsigned long __rtems_end[];
extern unsigned get_L1CR(), get_L2CR(), get_L3CR();
extern void bsp_cleanup(void);
extern Triv121PgTbl BSP_pgtbl_setup();
extern void BSP_pgtbl_activate();
extern int I2Cread_eeprom();
extern void BSP_vme_config(void);

SPR_RW(SPRG0)
SPR_RW(SPRG1)

typedef struct CmdLineRec_ {
		unsigned long	size;
		char		buf[0];
} CmdLineRec, *CmdLine;


#define	mtspr(reg, val)	\
	__asm __volatile("mtspr %0,%1" : : "K"(reg), "r"(val))


#define	mfspr(reg) \
	( { unsigned val; \
	  __asm __volatile("mfspr %0,%1" : "=r"(val) : "K"(reg)); \
	  val; } )

/*
 * Copy Additional boot param passed by boot loader
 */
#define MAX_LOADER_ADD_PARM 80
char loaderParam[MAX_LOADER_ADD_PARM];

/*
 * Total memory using RESIDUAL DATA
 */
unsigned int BSP_mem_size;
/*
 * PCI Bus Frequency
 */
/*
 * Start of the heap
 */
unsigned int BSP_heap_start;

unsigned int BSP_bus_frequency;
/*
 * processor clock frequency
 */
unsigned int BSP_processor_frequency;
/*
 * Time base divisior (how many tick for 1 second).
 */
unsigned int BSP_time_base_divisor;
unsigned char ConfVPD_buff[200];

#define CMDLINE_BUF_SIZE	2048

static char cmdline_buf[CMDLINE_BUF_SIZE];
char *BSP_commandline_string = cmdline_buf;

/*
 * system init stack and soft ir stack size
 */
#define INIT_STACK_SIZE 0x1000
#define INTR_STACK_SIZE CONFIGURE_INTERRUPT_STACK_MEMORY

void BSP_panic(char *s)
{
  printk("%s PANIC %s\n",_RTEMS_version, s);
  __asm__ __volatile ("sc"); 
}

void _BSP_Fatal_error(unsigned int v)
{
  printk("%s PANIC ERROR %x\n",_RTEMS_version, v);
  __asm__ __volatile ("sc"); 
}
 
/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */

extern rtems_configuration_table Configuration;

rtems_configuration_table  BSP_Configuration;

rtems_cpu_table Cpu_table;

char *rtems_progname;

/*
 *  Use the shared implementations of the following routines
 */
 
extern void bsp_postdriver_hook(void); /* see c/src/lib/libbsp/shared/bsppost.c */

extern void bsp_libc_init( void *, uint32_t, int );

extern void bsp_pretasking_hook(void);

void zero_bss()
{
  /* prevent these from being accessed in the short data areas */
  extern unsigned long __bss_start[], __SBSS_START__[], __SBSS_END__[];
  extern unsigned long __SBSS2_START__[], __SBSS2_END__[];
  memset(__SBSS_START__, 0, ((unsigned) __SBSS_END__) - ((unsigned)__SBSS_START__));
  memset(__SBSS2_START__, 0, ((unsigned) __SBSS2_END__) - ((unsigned)__SBSS2_START__));
  memset(__bss_start, 0, ((unsigned) __rtems_end) - ((unsigned)__bss_start));
}

/* NOTE: we cannot simply malloc the commandline string;
 * save_boot_params() is called during a very early stage when
 * libc/malloc etc. are not yet initialized!
 *
 * Here's what we do:
 *
 * initial layout setup by the loader (preload.S):
 *
 * 0..RTEMS...__rtems_end | cmdline ....... TOP
 *
 * After the save_boot_params() routine returns, the stack area will be
 * set up (start.S):
 *
 * 0..RTEMS..__rtems_end | INIT_STACK | IRQ_STACK | ..... TOP
 *
 * initialize_executive_early() [called from boot_card()]
 * will initialize the workspace:
 *
 * 0..RTEMS..__rtems_end | INIT_STACK | IRQ_STACK | ...... | workspace | TOP
 *
 * and later calls our pretasking_hook() which ends up initializing
 * libc which in turn initializes the heap
 *
 * 0..RTEMS..__rtems_end | INIT_STACK | IRQ_STACK | heap | workspace | TOP
 *
 * The idea here is to first move the commandline to the future 'heap' area
 * from where it will be picked up by our pretasking_hook().
 * pretasking_hook() then moves it either to INIT_STACK or the workspace
 * area using proper allocation, initializes libc and finally moves
 * the data to the environment / malloced areas...
 */

/* this routine is called early at shared/start/start.S 
 * and must be safe with a not properly aligned stack
 */
void
save_boot_params(void *r3, void *r4, void* r5, char *cmdline_start, char *cmdline_end)
{
int		i=cmdline_end-cmdline_start;
	if ( i >= CMDLINE_BUF_SIZE )
		i = CMDLINE_BUF_SIZE-1;
	else if ( i < 0 )
		i = 0;
        memmove(cmdline_buf, cmdline_start, i);
	cmdline_buf[i]=0;
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
#ifdef CONF_VPD
  int i;
#endif
  unsigned char *stack;
  unsigned long	 *r1sp;
#ifdef SHOW_LCR1_REGISTER
  unsigned l1cr;
#endif
#ifdef SHOW_LCR2_REGISTER
  unsigned l2cr;
#endif
#ifdef SHOW_LCR3_REGISTER
  unsigned l3cr;
#endif
  register uint32_t  intrStack;
  register uint32_t *intrStackPtr;
  unsigned char *work_space_start;
  ppc_cpu_id_t myCpu;
  ppc_cpu_revision_t myCpuRevision;
  Triv121PgTbl	pt=0;

  /* Till Straumann: 4/2005
   * Need to map the system registers early, so we can printk...
   * (otherwise we silently die)
   */
  /*
   * Kate Feng : PCI 0 domain memory space, want to leave room for the VME window 
   */
  setdbat(2, PCI0_MEM_BASE, PCI0_MEM_BASE, 0x10000000, IO_PAGE);

  /* Till Straumann: 2004
   * map the PCI 0, 1 Domain I/O space, GT64260B registers
   * and the reserved area so that the size is the power of 2.
   * 
   */
  setdbat(3,PCI0_IO_BASE, PCI0_IO_BASE, 0x2000000, IO_PAGE);


  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type() function
   * store the result in global variables so that it can be used latter...
   */
  myCpu 	= get_ppc_cpu_type();
  myCpuRevision = get_ppc_cpu_revision();

#ifdef SHOW_LCR1_REGISTER
  l1cr = get_L1CR();
  printk("Initial L1CR value = %x\n", l1cr); 
#endif

  /*
   * the initial stack  has aready been set to this value in start.S
   * so there is no need to set it in r1 again... It is just for info
   * so that it can be printed without accessing R1.
   */
  stack = ((unsigned char*) __rtems_end) + INIT_STACK_SIZE - PPC_MINIMUM_STACK_FRAME_SIZE;

 /* tag the bottom (T. Straumann 6/36/2001 <strauman@slac.stanford.edu>) */
  *((uint32_t *)stack) = 0;

  /* fill stack with pattern for debugging */
  __asm__ __volatile__("mr %0, %%r1":"=r"(r1sp));
  while (--r1sp >= (unsigned long*)__rtems_end)
	  *r1sp=0xeeeeeeee;

  /*
   * Initialize the interrupt related settings
   * SPRG0 = interrupt nesting level count
   * SPRG1 = software managed IRQ stack
   *
   * This could be done latter (e.g in IRQ_INIT) but it helps to understand
   * some settings below...
   */
  BSP_heap_start = ((uint32_t) __rtems_end) + INIT_STACK_SIZE + INTR_STACK_SIZE;
  intrStack = BSP_heap_start - PPC_MINIMUM_STACK_FRAME_SIZE;

  /* make sure it's properly aligned */
  intrStack &= ~(CPU_STACK_ALIGNMENT-1);

  /* tag the bottom (T. Straumann 6/36/2001 <strauman@slac.stanford.edu>) */
  intrStackPtr = (uint32_t*) intrStack;
  *intrStackPtr = 0;

  _write_SPRG1(intrStack);

  _write_SPRG0(PPC_BSP_HAS_FIXED_PR288);

  /*
   * Initialize default raw exception hanlders. See vectors/vectors_init.c
   */
  Cpu_table.exceptions_in_RAM = TRUE;
  initialize_exceptions();
  /*
   * Init MMU block address translation to enable hardware
   * access
   * More PCI1 memory mapping to be done after BSP_pgtbl_activate.
   */
  printk("-----------------------------------------\n");
  printk("Welcome to %s on MVME5500-0163\n", _RTEMS_version );
  printk("-----------------------------------------\n");

#ifdef TEST_RETURN_TO_PPCBUG  
  printk("Hit <Enter> to return to PPCBUG monitor\n");
  printk("When Finished hit GO. It should print <Back from monitor>\n");
  debug_getc();
  _return_to_ppcbug();
  printk("Back from monitor\n");
  _return_to_ppcbug();
#endif /* TEST_RETURN_TO_PPCBUG  */

#ifdef TEST_RAW_EXCEPTION_CODE  
  printk("Testing exception handling Part 1\n");
  /*
   * Cause a software exception
   */
  __asm__ __volatile ("sc");
  /*
   * Check we can still catch exceptions and returned coorectly.
   */
  printk("Testing exception handling Part 2\n");
  __asm__ __volatile ("sc");
#endif  

  BSP_mem_size 				=  _512M;
  /* TODO: calculate the BSP_bus_frequency using the REF_CLK bit of System Status  register */
  /* rtems_bsp_delay_in_bus_cycles are defined in registers.h */
  BSP_bus_frequency			= 133333333;
  BSP_processor_frequency		= 1000000000;
  BSP_time_base_divisor			= 4000;/* P94 : 7455 clocks the TB/DECR at 1/4 of the system bus clock frequency */


  /* Maybe not setup yet becuase of the warning message */
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

  printk("Now BSP_mem_size = 0x%x\n",BSP_mem_size); 

  /*
   * Set up our hooks
   * Make sure libc_init is done before drivers initialized so that
   * they can use atexit()
   */

  Cpu_table.pretasking_hook 	 = bsp_pretasking_hook;    /* init libc, etc. */
  Cpu_table.postdriver_hook 	 = bsp_postdriver_hook;
  Cpu_table.do_zero_of_workspace = TRUE;
  Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;
  /* P94 : 7455 TB/DECR is clocked by the system bus clock frequency */
  Cpu_table.clicks_per_usec 	 = BSP_bus_frequency/(BSP_time_base_divisor * 1000);
  Cpu_table.exceptions_in_RAM 	 = TRUE;
  _CPU_Table                     = Cpu_table;/* S. Kate Feng <feng1@bnl.gov>, for rtems_bsp_delay() */

  printk("BSP_Configuration.work_space_size = %x\n", BSP_Configuration.work_space_size); 
  work_space_start = 
    (unsigned char *)BSP_mem_size - BSP_Configuration.work_space_size;

  if ( work_space_start <= ((unsigned char *)__rtems_end) + INIT_STACK_SIZE + INTR_STACK_SIZE) {
    printk( "bspstart: Not enough RAM!!!\n" );
    bsp_cleanup();
  }

  BSP_Configuration.work_space_start = work_space_start;

  /*
   * Initalize RTEMS IRQ system
   */
   BSP_rtems_irq_mng_init(0);

#ifdef SHOW_LCR2_REGISTER
  l2cr = get_L2CR();
  printk("Initial L2CR value = %x\n", l2cr);
#endif  

#ifdef SHOW_LCR3_REGISTER
  /* L3CR needs DEC int. handler installed for bsp_delay()*/
  l3cr = get_L3CR();
  printk("Initial L3CR value = %x\n", l3cr);
#endif  


  /* Activate the page table mappings only after
   * initializing interrupts because the irq_mng_init()
   * routine needs to modify the text
   */           
  if (pt) {
#ifdef SHOW_MORE_INIT_SETTINGS
    printk("Page table setup finished; will activate it NOW...\n");
#endif
    BSP_pgtbl_activate(pt);
  }

  /*
   * PCI 1 domain memory space
   */
  setdbat(1, PCI1_MEM_BASE, PCI1_MEM_BASE, 0x10000000, IO_PAGE);
 

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Going to start PCI buses scanning and initialization\n");
#endif  
  pci_initialize();
#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Number of PCI buses found is : %d\n", pci_bus_count());
#endif

  /* Install our own exception handler (needs PCI) */
  globalExceptHdl = BSP_exceptionHandler;

  /* clear hostbridge errors. MCP signal is not used on the MVME5500
   * PCI config space scanning code will trip otherwise :-(
   */
  _BSP_clear_hostbridge_errors(0, 1 /*quiet*/);

  /* Read Configuration Vital Product Data (VPD) */
  if ( I2Cread_eeprom(0xa8, 4,2, &ConfVPD_buff[0], 150))
     printk("I2Cread_eeprom() error \n");
  else {
#ifdef CONF_VPD
    printk("\n");
    for (i=0; i<150; i++) {
      printk("%2x ", ConfVPD_buff[i]);  
      if ((i % 20)==0 ) printk("\n");
    }
    printk("\n");
#endif
  }

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("MSR %x \n", _read_MSR());
  printk("Exit from bspstart\n");
#endif

}
