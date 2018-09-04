/*
 *  This routine does the bulk of the system initialization.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  Modified to support the MCP750.
 *  Modifications Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 */

#include <string.h>

#include <bsp.h>
#include <bsp/bootcard.h>
#include <rtems/bspIo.h>
#include <rtems/counter.h>
#include <rtems/sysinit.h>
#include <bsp/consoleIo.h>
#include <libcpu/spr.h>
#include <bsp/residual.h>
#include <bsp/pci.h>
#include <bsp/openpic.h>
#include <bsp/irq.h>
#include <libcpu/bat.h>
#include <libcpu/pte121.h>
#include <libcpu/cpuIdent.h>
#include <bsp/vectors.h>
#include <bsp/VME.h>
#include <bsp/motorola.h>
#include <rtems/powerpc/powerpc.h>

extern void _return_to_ppcbug(void);
extern unsigned long __rtems_end[];
extern void L1_caches_enables(void);
extern unsigned get_L2CR(void);
extern void set_L2CR(unsigned);
extern Triv121PgTbl BSP_pgtbl_setup(unsigned int *);
extern void			BSP_pgtbl_activate(Triv121PgTbl);

SPR_RW(SPRG1)

#if defined(DEBUG_BATS)
extern void ShowBATS(void);
#endif

/*
 *  Driver configuration parameters
 */
uint32_t   bsp_clicks_per_usec;

/*
 * Copy of residuals passed by firmware
 */
RESIDUAL residualCopy;
/*
 * Copy Additional boot param passed by boot loader
 */
#define MAX_LOADER_ADD_PARM 80
char loaderParam[MAX_LOADER_ADD_PARM];

char *BSP_commandline_string = loaderParam;
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
 *  Use the shared implementations of the following routines
 */

char *save_boot_params(
  void *r3,
  void *r4,
  void *r5,
  char *cmdline_start,
  char *cmdline_end
)
{

  residualCopy = *(RESIDUAL *)r3;
  strncpy(loaderParam, cmdline_start, MAX_LOADER_ADD_PARM);
  loaderParam[MAX_LOADER_ADD_PARM - 1] ='\0';
  return loaderParam;
}

#if defined(mvme2100)
unsigned int EUMBBAR;

/*
 * Return the current value of the Embedded Utilities Memory Block Base Address
 * Register (EUMBBAR) as read from the processor configuration register using
 * Processor Address Map B (CHRP).
 */
static unsigned int get_eumbbar(void) {
  out_le32( (volatile uint32_t *)0xfec00000, 0x80000078 );
  return in_le32( (volatile uint32_t *)0xfee00000 );
}
#endif

uint32_t _CPU_Counter_frequency(void)
{
  return BSP_bus_frequency / (BSP_time_base_divisor / 1000);
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
#if !defined(mvme2100)
  unsigned l2cr;
#endif
  prep_t boardManufacturer;
  motorolaBoard myBoard;
  Triv121PgTbl	pt=0;

  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type()
   * function store the result in global variables so that it can be used
   * later...
   */
  get_ppc_cpu_type();
  get_ppc_cpu_revision();

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
  setdbat(2, PCI_MEM_BASE+PCI_MEM_WIN0, PCI_MEM_BASE+PCI_MEM_WIN0, 0x10000000, IO_PAGE);

  /*
   * Must have acces to open pic PCI ACK registers provided by the RAVEN
   */
#ifndef qemu
  setdbat(3, 0xf0000000, 0xf0000000, 0x10000000, IO_PAGE);
#else
  setdbat(3, 0xb0000000, 0xb0000000, 0x10000000, IO_PAGE);
#endif

#if defined(mvme2100)
  /* Need 0xfec00000 mapped for this */
  EUMBBAR = get_eumbbar();
#endif

  /*
   * enables L1 Cache. Note that the L1_caches_enables() codes checks for
   * relevant CPU type so that the reason why there is no use of myCpu...
   */
  L1_caches_enables();

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


#if !defined(mvme2100)
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
#endif

  ppc_exc_initialize();

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

  pci_initialize();
  {
    const struct _int_map *bspmap  = motorolaIntMap(currentBoard);
    if( bspmap ) {
       printk("pci : Configuring interrupt routing for '%s'\n",
          motorolaBoardToString(currentBoard));
       FixupPCI(bspmap, motorolaIntSwizzle(currentBoard));
    }
    else
       printk("pci : Interrupt routing not available for this bsp\n");
 }

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Number of PCI buses found is : %d\n", pci_bus_count());
#endif
#ifdef TEST_RAW_EXCEPTION_CODE
  printk("Testing exception handling Part 1\n");
  /*
   * Cause a software exception
   */
  __asm__ __volatile ("sc");
  /*
   * Check we can still catch exceptions and return coorectly.
   */
  printk("Testing exception handling Part 2\n");
  __asm__ __volatile ("sc");

  /*
   * Somehow doing the above seems to clobber SPRG0 on the mvme2100.  The
   * interrupt disable mask is stored in SPRG0. Is this a problem?
   */
  ppc_interrupt_set_disable_mask( PPC_INTERRUPT_DISABLE_MASK_DEFAULT);

#endif

/* See above */

  BSP_mem_size            = residualCopy.TotalMemory;
  BSP_bus_frequency       = residualCopy.VitalProductData.ProcessorBusHz;
  BSP_processor_frequency = residualCopy.VitalProductData.ProcessorHz;
  BSP_time_base_divisor   = (residualCopy.VitalProductData.TimeBaseDivisor?
                    residualCopy.VitalProductData.TimeBaseDivisor : 4000);

  /* clear hostbridge errors but leave MCP disabled -
   * PCI config space scanning code will trip otherwise :-(
   */
  _BSP_clear_hostbridge_errors(0 /* enableMCP */, 0/*quiet*/);

  if (BSP_mem_size > 0x10000000)
  {
    /* Support cases of system memory size larger than 256Mb.
     *
     * We use BAT3 in order to obtain access to the top section of the RAM.
     * We also need to do this just before setting up the page table because
     * this is where the page table will be located.
     */
    const unsigned int mem256Count = (BSP_mem_size / 0x10000000);
    const unsigned int BAT3Addr    = ((BSP_mem_size % 0x10000000)  ?
                                       (mem256Count     * 0x10000000) :
                                      ((mem256Count-1) * 0x10000000));
    setdbat(3, BAT3Addr, BAT3Addr, 0x10000000, IO_PAGE);
#ifdef SHOW_MORE_INIT_SETTINGS
    printk("Setting up BAT3 for large memory support. (BAT3 --> 0x%x)\n", BAT3Addr);
#endif
  }

  /* Allocate and set up the page table mappings
   * This is only available on >604 CPUs.
   *
   * NOTE: This setup routine may modify the available memory
   *       size. It is essential to call it before
   *       calculating the workspace etc.
   */
  pt = BSP_pgtbl_setup(&BSP_mem_size);

  if (!pt || TRIV121_MAP_SUCCESS != triv121PgTblMap(
            pt, TRIV121_121_VSID,
#ifndef qemu
            0xfeff0000,
#else
            0xbffff000,
#endif
            1,
            TRIV121_ATTR_IO_PAGE, TRIV121_PP_RW_PAGE)) {
	printk("WARNING: unable to setup page tables VME "
               "bridge must share PCI space\n");
  }

  /*
   *  initialize the device driver parameters
   */
  bsp_clicks_per_usec 	 = BSP_bus_frequency/(BSP_time_base_divisor * 1000);

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

#if defined(DEBUG_BATS)
  ShowBATS();
#endif

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("Exit from bspstart\n");
#endif
}

RTEMS_SYSINIT_ITEM(
  BSP_vme_config,
  RTEMS_SYSINIT_BSP_PRE_DRIVERS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
