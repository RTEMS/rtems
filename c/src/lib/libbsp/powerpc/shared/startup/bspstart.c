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
 *  $Id$
 */

#include <string.h>

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <bsp/consoleIo.h>
#include <libcpu/spr.h>
#include <bsp/residual.h>
#include <bsp/pci.h>
#include <bsp/openpic.h>
#include <bsp/irq.h>
#include <bsp/VME.h>
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

SPR_RW(SPRG0)
SPR_RW(SPRG1)

#if defined(DEBUG_BATS)
void printBAT( int bat, unsigned32 upper, unsigned32 lower )
{
  unsigned32 lowest_addr;
  unsigned32 size;

  printk("BAT%d raw(upper=0x%08x, lower=0x%08x) ", bat, upper, lower );

  lowest_addr = (upper & 0xFFFE0000);
  size = (((upper & 0x00001FFC) >> 2) + 1) * (128 * 1024);
  printk(" range(0x%08x, 0x%08x) %s%s %s%s%s%s %s\n",
    lowest_addr,
    lowest_addr + (size - 1),
    (upper & 0x01) ? "P" : "p",
    (upper & 0x02) ? "S" : "s",
    (lower & 0x08) ? "G" : "g",
    (lower & 0x10) ? "M" : "m",
    (lower & 0x20) ? "I" : "i",
    (lower & 0x40) ? "W" : "w",
    (lower & 0x01) ? "Read Only" :
      ((lower & 0x02) ? "Read/Write" : "No Access")
  );
}

void ShowBATS(){
  unsigned32 lower;
  unsigned32 upper;

  __MFSPR(536, upper); __MFSPR(537, lower); printBAT( 0, upper, lower );
  __MFSPR(538, upper); __MFSPR(539, lower); printBAT( 1, upper, lower );
  __MFSPR(540, upper); __MFSPR(541, lower); printBAT( 2, upper, lower );
  __MFSPR(542, upper); __MFSPR(543, lower); printBAT( 3, upper, lower );
}
#endif

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
  rtems_unsigned32        heap_sbrk_spared;
  extern rtems_unsigned32 _bsp_sbrk_init(rtems_unsigned32, rtems_unsigned32*);

  heap_start = ((rtems_unsigned32) __rtems_end) +
                INIT_STACK_SIZE + INTR_STACK_SIZE;
  if (heap_start & (CPU_ALIGNMENT-1))
      heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

  heap_size = (BSP_mem_size - heap_start) - BSP_Configuration.work_space_size;
  heap_sbrk_spared=_bsp_sbrk_init(heap_start, &heap_size);

#ifdef SHOW_MORE_INIT_SETTINGS
  printk( "HEAP start %x  size %x (%x bytes spared for sbrk)\n",
             heap_start, heap_size, heap_sbrk_spared);
#endif    

  bsp_libc_init((void *) 0, heap_size, heap_sbrk_spared);

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

#if defined(mpc8240) || defined(mpc8245)
unsigned int EUMBBAR;

/* 
 * Return the current value of the Embedded Utilities Memory Block Base Address
 * Register (EUMBBAR) as read from the processor configuration register using
 * Processor Address Map B (CHRP).
 */ 
unsigned int get_eumbbar() {
  register int a, e;

  asm volatile( "lis %0,0xfec0; ori  %0,%0,0x0000": "=r" (a) );
  asm volatile("sync");
                                                                
  asm volatile("lis %0,0x8000; ori %0,%0,0x0078": "=r"(e) ); 
  asm volatile("stwbrx  %0,0x0,%1": "=r"(e): "r"(a));  
  asm volatile("sync");

  asm volatile("lis %0,0xfee0; ori %0,%0,0x0000": "=r" (a) ); 
  asm volatile("sync");
                                                         
  asm volatile("lwbrx %0,0x0,%1": "=r" (e): "r" (a));
  asm volatile("isync");
  return e;
}
#endif

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
  unsigned char *stack;
  unsigned l2cr;
  register unsigned char* intrStack;
  unsigned char *work_space_start;
  ppc_cpu_id_t myCpu;
  ppc_cpu_revision_t myCpuRevision;
  prep_t boardManufacturer;
  motorolaBoard myBoard;
  Triv121PgTbl	pt=0;

  select_console(CONSOLE_SERIAL);
  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type()
   * function store the result in global variables so that it can be used 
   * later...
   */
  myCpu 	= get_ppc_cpu_type();
  myCpuRevision = get_ppc_cpu_revision();

  /*
   * Init MMU block address translation to enable hardware access
   */

#if !defined(mvme2100)
  /*
   * PC legacy IO space used for inb/outb and all PC compatible hardware
   */
   setdbat(1, _IO_BASE, _IO_BASE, 0x10000000, IO_PAGE);
#endif

  /*
   * PCI devices memory area. Needed to access OpenPIC features
   * provided by the Raven
   *
   * T. Straumann: give more PCI address space
   */
   setdbat(2, PCI_MEM_BASE, PCI_MEM_BASE, 0x10000000, IO_PAGE);

  /*
   * Must have access to OpenPIC PCI ACK registers provided by the Raven
   */
  setdbat(3, 0xf0000000, 0xf0000000, 0x10000000, IO_PAGE);

#if defined(mvme2100)
  EUMBBAR = get_eumbbar(); 
#endif

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
   * the initial stack has already been set to this value in start.S
   * so there is no need to set it in r1 again... It is just for info
   * so that It can be printed without accessing R1.
   */
  stack = ((unsigned char*) __rtems_end) +
               INIT_STACK_SIZE - CPU_MINIMUM_STACK_FRAME_SIZE;

  /* tag the bottom (T. Straumann 6/36/2001 <strauman@slac.stanford.edu>) */
  *((unsigned32 *)stack) = 0;

  /*
   * Initialize the interrupt related settings
   * SPRG1 = software managed IRQ stack
   *
   * This could be done later (e.g in IRQ_INIT) but it helps to understand
   * some settings below...
   */
  intrStack = ((unsigned char*) __rtems_end) +
    INIT_STACK_SIZE + INTR_STACK_SIZE - CPU_MINIMUM_STACK_FRAME_SIZE;

  /* make sure it's properly aligned */
  (unsigned32)intrStack &= ~(CPU_STACK_ALIGNMENT-1);

  /* tag the bottom (T. Straumann 6/36/2001 <strauman@slac.stanford.edu>) */
  *((unsigned32 *)intrStack) = 0;

  _write_SPRG1((unsigned int)intrStack);

  /* signal them that we have fixed PR288 - eventually, this should go away */
  _write_SPRG0(PPC_BSP_HAS_FIXED_PR288);

  /* initialize_exceptions() evaluates the exceptions_in_RAM flag */
  Cpu_table.exceptions_in_RAM 	 = TRUE;
  /*
   * Initialize default raw exception handlers. See vectors/vectors_init.c
   */
  initialize_exceptions();

  select_console(CONSOLE_LOG);

  /*
   * We check that the keyboard is present and immediately 
   * select the serial console if not.
   */
#if defined(BSP_KBD_IOBASE)
  { int err;
    err = kbdreset();
    if (err) select_console(CONSOLE_SERIAL);
  }
#else
  select_console(CONSOLE_SERIAL);
#endif

  boardManufacturer   =  checkPrepBoardType(&residualCopy);
  if (boardManufacturer != PREP_Motorola) {
    printk("Unsupported hardware vendor\n");
    while (1);
  }
  myBoard = getMotorolaBoard();
  
  printk("-----------------------------------------\n");
  printk("Welcome to %s on %s\n", _RTEMS_version,
                                    motorolaBoardToString(myBoard));
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
    const struct _int_map *bspmap = motorolaIntMap(currentBoard);
    if( bspmap ) {
       printk("pci : Configuring interrupt routing for '%s'\n",
          motorolaBoardToString(currentBoard));
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
   * Check we can still catch exceptions and return correctly.
   */
  printk("Testing exception handling Part 2\n");
  __asm__ __volatile ("sc");

  /*
   *  Somehow doing the above seems to clobber SPRG0 on the mvme2100.  It
   *  is probably a not so subtle hint that you do not want to use PPCBug
   *  once RTEMS is up and running.  Anyway, we still needs to indicate
   *  that we have fixed PR288.  Eventually, this should go away.
   */
  _write_SPRG0(PPC_BSP_HAS_FIXED_PR288);
#endif  

  BSP_mem_size            = residualCopy.TotalMemory;
  BSP_bus_frequency       = residualCopy.VitalProductData.ProcessorBusHz;
  BSP_processor_frequency = residualCopy.VitalProductData.ProcessorHz;
  BSP_time_base_divisor   = (residualCopy.VitalProductData.TimeBaseDivisor?
                    residualCopy.VitalProductData.TimeBaseDivisor : 4000);

  /* clear hostbridge errors but leave MCP disabled -
   * PCI config space scanning code will trip otherwise :-(
   */
  _BSP_clear_hostbridge_errors(0 /* enableMCP */, 0/*quiet*/);

  /* Allocate and set up the page table mappings
   * This is only available on >604 CPUs.
   *
   * NOTE: This setup routine may modify the available memory
   *       size. It is essential to call it before
   *       calculating the workspace etc.
   */
  pt = BSP_pgtbl_setup(&BSP_mem_size);

  if (!pt || TRIV121_MAP_SUCCESS != triv121PgTblMap(
            pt, TRIV121_121_VSID, 0xfeff0000, 1,
            TRIV121_ATTR_IO_PAGE, TRIV121_PP_RW_PAGE)) {
	printk("WARNING: unable to setup page tables VME "
               "bridge must share PCI space\n");
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

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("BSP_Configuration.work_space_size = %x\n",
          BSP_Configuration.work_space_size);
#endif  
  work_space_start = 
    (unsigned char *)BSP_mem_size - BSP_Configuration.work_space_size;

  if ( work_space_start <=
       ((unsigned char *)__rtems_end) + INIT_STACK_SIZE + INTR_STACK_SIZE) {
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
   * Initialize VME bridge - needs working PCI and IRQ subsystems...
   */
#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Going to initialize VME bridge\n");
#endif
  /*
   * VME initialization is in a separate file so apps which don't use VME or
   * want a different configuration may link against a customized routine.
   */
  BSP_vme_config();


#if defined(DEBUG_BATS)
  ShowBATS();
#endif

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Exit from bspstart\n");
#endif  
}
