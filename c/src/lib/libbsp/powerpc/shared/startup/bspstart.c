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
 *  $Id$
 */

#include <string.h>

#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <bsp/consoleIo.h>
#include <libcpu/spr.h>
#include <bsp/residual.h>
#include <bsp/pci.h>
#include <bsp/openpic.h>
#include <bsp/irq.h>
#include <bsp/VME.h>
#include <bsp.h>
#include <libcpu/bat.h>
#include <libcpu/pte121.h>
#include <libcpu/cpuIdent.h>
#include <bsp/vectors.h>
#include <bsp/motorola.h>

extern void _return_to_ppcbug();
extern unsigned long __rtems_end[];
extern void L1_caches_enables();
extern unsigned get_L2CR();
extern void set_L2CR(unsigned);
extern void bsp_cleanup(void);
extern Triv121PgTbl BSP_pgtbl_setup();
extern void			BSP_pgtbl_activate();
extern void			BSP_vme_config();

SPR_RW(SPR0)
SPR_RW(SPR1)

/*
 * Copy of residuals passed by firmware
 */
RESIDUAL residualCopy; 
/*
 * Copy Additional boot param passed by boot loader
 */
#define MAX_LOADER_ADD_PARM 80
char loaderParam[MAX_LOADER_ADD_PARM];
/*
 * Vital Board data Start using DATA RESIDUAL
 */
/*
 * Total memory using RESIDUAL DATA
 */
unsigned int BSP_mem_size;
/*
 * PCI Bus Frequency
 */
unsigned int BSP_bus_frequency;
/*
 * processor clock frequency
 */
unsigned int BSP_processor_frequency;
/*
 * Time base divisior (how many tick for 1 second).
 */
unsigned int BSP_time_base_divisor;
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
 
void bsp_postdriver_hook(void);
void bsp_libc_init( void *, unsigned32, int );

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
 
void bsp_pretasking_hook(void)
{
    rtems_unsigned32        heap_start;    
    rtems_unsigned32        heap_size;

    heap_start = ((rtems_unsigned32) __rtems_end) +INIT_STACK_SIZE + INTR_STACK_SIZE;
    if (heap_start & (CPU_ALIGNMENT-1))
        heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

    heap_size = (BSP_mem_size - heap_start) - BSP_Configuration.work_space_size;

#ifdef SHOW_MORE_INIT_SETTINGS
    printk(" HEAP start %x  size %x\n", heap_start, heap_size);
#endif    
    bsp_libc_init((void *) heap_start, heap_size, 0);

#ifdef RTEMS_DEBUG
    rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif
}

void zero_bss()
{
  /* prevent these from being accessed in the short data areas */
  extern unsigned long __bss_start[], __SBSS_START__[], __SBSS_END__[];
  extern unsigned long __SBSS2_START__[], __SBSS2_END__[];
  memset(__SBSS_START__, 0, ((unsigned) __SBSS_END__) - ((unsigned)__SBSS_START__));
  memset(__SBSS2_START__, 0, ((unsigned) __SBSS2_END__) - ((unsigned)__SBSS2_START__));
  memset(__bss_start, 0, ((unsigned) __rtems_end) - ((unsigned)__bss_start));
}

void save_boot_params(RESIDUAL* r3, void *r4, void* r5, char *additional_boot_options)
{
  
  residualCopy = *r3;
  strncpy(loaderParam, additional_boot_options, MAX_LOADER_ADD_PARM);
  loaderParam[MAX_LOADER_ADD_PARM - 1] ='\0';
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
  int err;
  unsigned char *stack;
  unsigned l2cr;
  register unsigned char* intrStack;
  register unsigned int intrNestingLevel = 0;
  unsigned char *work_space_start;
  ppc_cpu_id_t myCpu;
  ppc_cpu_revision_t myCpuRevision;
  prep_t boardManufacturer;
  motorolaBoard myBoard;
  Triv121PgTbl	pt=0;
  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type() function
   * store the result in global variables so that it can be used latter...
   */
  myCpu 	= get_ppc_cpu_type();
  myCpuRevision = get_ppc_cpu_revision();
  /*
   * enables L1 Cache. Note that the L1_caches_enables() codes checks for
   * relevant CPU type so that the reason why there is no use of myCpu...
   */
  L1_caches_enables();
  /*
   * Enable L2 Cache. Note that the set_L2CR(L2CR) codes checks for
   * relevant CPU type (mpc750)...
   */
  l2cr = get_L2CR();
#ifdef SHOW_LCR2_REGISTER
  printk("Initial L2CR value = %x\n", l2cr);
#endif  
  if ( (! (l2cr & 0x80000000)) && ((int) l2cr == -1))
    set_L2CR(0xb9A14000);
  /*
   * the initial stack  has aready been set to this value in start.S
   * so there is no need to set it in r1 again... It is just for info
   * so that It can be printed without accessing R1.
   */
  stack = ((unsigned char*) __rtems_end) + INIT_STACK_SIZE - CPU_MINIMUM_STACK_FRAME_SIZE;

 /* tag the bottom (T. Straumann 6/36/2001 <strauman@slac.stanford.edu>) */
  *((unsigned32 *)stack) = 0;

  /*
   * Initialize the interrupt related settings
   * SPRG0 = interrupt nesting level count
   * SPRG1 = software managed IRQ stack
   *
   * This could be done latter (e.g in IRQ_INIT) but it helps to understand
   * some settings below...
   */
  intrStack = ((unsigned char*) __rtems_end) + INIT_STACK_SIZE + INTR_STACK_SIZE - CPU_MINIMUM_STACK_FRAME_SIZE;

  /* make sure it's properly aligned */
  (unsigned32)intrStack &= ~(CPU_STACK_ALIGNMENT-1);

  /* tag the bottom (T. Straumann 6/36/2001 <strauman@slac.stanford.edu>) */
  *((unsigned32 *)intrStack) = 0;

  _write_SPR1((unsigned int)intrStack);
  _write_SPR0(intrNestingLevel);
  /*
   * Initialize default raw exception hanlders. See vectors/vectors_init.c
   */
  initialize_exceptions();
  /*
   * Init MMU block address translation to enable hardware
   * access
   */
  /*
   * PC legacy IO space used for inb/outb and all PC
   * compatible hardware
   */
  setdbat(1, _IO_BASE, _IO_BASE, 0x10000000, IO_PAGE);
  /*
   * PCI devices memory area. Needed to access OPENPIC features
   * provided by the RAVEN
   */
  /* T. Straumann: give more PCI address space */
  setdbat(2, PCI_MEM_BASE, PCI_MEM_BASE, 0x30000000, IO_PAGE);
  /*
   * Must have acces to open pic PCI ACK registers 
   * provided by the RAVEN
   * 
   */
  setdbat(3, 0xf0000000, 0xf0000000, 0x10000000, IO_PAGE);

  select_console(CONSOLE_LOG);

  /* We check that the keyboard is present and immediately 
   * select the serial console if not.
   */
  err = kbdreset();
  if (err) select_console(CONSOLE_SERIAL);

  boardManufacturer   =  checkPrepBoardType(&residualCopy);
  if (boardManufacturer != PREP_Motorola) {
    printk("Unsupported hardware vendor\n");
    while (1);
  }
  myBoard = getMotorolaBoard();
  
  printk("-----------------------------------------\n");
  printk("Welcome to %s on %s\n", _RTEMS_version, motorolaBoardToString(myBoard));
  printk("-----------------------------------------\n");
#ifdef SHOW_MORE_INIT_SETTINGS  
  printk("Residuals are located at %x\n", (unsigned) &residualCopy);
  printk("Additionnal boot options are %s\n", loaderParam);
  printk("Initial system stack at %x\n",stack);
  printk("Software IRQ stack at %x\n",intrStack);
  printk("-----------------------------------------\n");
#endif

#ifdef TEST_RETURN_TO_PPCBUG  
  printk("Hit <Enter> to return to PPCBUG monitor\n");
  printk("When Finished hit GO. It should print <Back from monitor>\n");
  debug_getc();
  _return_to_ppcbug();
  printk("Back from monitor\n");
  _return_to_ppcbug();
#endif /* TEST_RETURN_TO_PPCBUG  */

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Going to start PCI buses scanning and initialization\n");
#endif  
  InitializePCI();

 {
    struct _int_map     *bspmap   = motorolaIntMap(currentBoard);
    if( bspmap )
    {
       printk("pci : Configuring interrupt routing for '%s'\n", motorolaBoardToString(currentBoard));
       FixupPCI(bspmap, motorolaIntSwizzle(currentBoard) );
    }
    else
       printk("pci : Interrupt routing not available for this bsp\n");

 }



#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Number of PCI buses found is : %d\n", BusCountPCI());
#endif
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


  BSP_mem_size 				= residualCopy.TotalMemory;
  BSP_bus_frequency			= residualCopy.VitalProductData.ProcessorBusHz;
  BSP_processor_frequency		= residualCopy.VitalProductData.ProcessorHz;
  BSP_time_base_divisor			= (residualCopy.VitalProductData.TimeBaseDivisor?
					   residualCopy.VitalProductData.TimeBaseDivisor : 4000);

  /* clear hostbridge errors and enable MCP */
  _BSP_clear_hostbridge_errors(1/*enableMCP*/, 0/*quiet*/);

  /* Allocate and set up the page table mappings
   * This is only available on >604 CPUs.
   *
   * NOTE: This setup routine may modify the available memory
   *       size. It is essential to call it before
   *       calculating the workspace etc.
   */
  pt = BSP_pgtbl_setup(&BSP_mem_size);

  if (!pt ||
	  TRIV121_MAP_SUCCESS != triv121PgTblMap(
			  							pt,
										TRIV121_121_VSID,
										0xfeff0000,
										1,
										TRIV121_ATTR_IO_PAGE,
										TRIV121_PP_RW_PAGE
										)) {
	printk("WARNING: unable to setup page tables VME bridge must share PCI space\n");
  }
  
  /*
   * Set up our hooks
   * Make sure libc_init is done before drivers initialized so that
   * they can use atexit()
   */

  Cpu_table.pretasking_hook 	 = bsp_pretasking_hook;    /* init libc, etc. */
  Cpu_table.postdriver_hook 	 = bsp_postdriver_hook;
  Cpu_table.do_zero_of_workspace = TRUE;
  Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;
  Cpu_table.clicks_per_usec 	 = BSP_processor_frequency/(BSP_time_base_divisor * 1000);
  Cpu_table.exceptions_in_RAM 	 = TRUE;

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("BSP_Configuration.work_space_size = %x\n", BSP_Configuration.work_space_size);
#endif  
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

  
  /* Activate the page table mappings only after
   * initializing interrupts because the irq_mng_init()
   * routine needs to modify the text
   */           
  if (pt) {
#ifdef  SHOW_MORE_INIT_SETTINGS
    printk("Page table setup finished; will activate it NOW...\n");
#endif
    BSP_pgtbl_activate(pt);
  	/* finally, switch off DBAT3 */
	setdbat(3, 0, 0, 0, 0); 
  }

  /*
   * Initialize VME bridge - needs working PCI
   * and IRQ subsystems...
   */
#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Going to initialize VME bridge\n");
#endif
  /* VME initialization is in a separate file so apps which don't use
   * VME or want a different configuration may link against a customized
   * routine.
   */
  BSP_vme_config();

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Exit from bspstart\n");
#endif  
}
