/*
 *  This routine starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before this routine is invoked.
 *
 *  COPYRIGHT (c) 1989-2007.
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
 *  (C) by Brookhaven National Lab., S. Kate Feng <feng1@bnl.gov>, 2003-2009
 */

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <rtems/system.h>
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

/*
#define SHOW_MORE_INIT_SETTINGS
#define SHOW_LCR1_REGISTER
#define SHOW_LCR2_REGISTER
#define SHOW_LCR3_REGISTER
#define CONF_VPD
*/

extern uint32_t probeMemoryEnd(void); /* from shared/startup/probeMemoryEnd.c */

BSP_output_char_function_type     BSP_output_char = BSP_output_char_via_serial;
BSP_polling_getchar_function_type BSP_poll_char = NULL;

extern void _return_to_ppcbug(void);
extern unsigned long __rtems_end[];
extern unsigned get_L1CR(void), get_L2CR(void), get_L3CR(void);
extern Triv121PgTbl BSP_pgtbl_setup(unsigned int *);
extern void BSP_pgtbl_activate(Triv121PgTbl);
extern int I2Cread_eeprom(unsigned char I2cBusAddr, uint32_t devA2A1A0, uint32_t AddrBytes, unsigned char *pBuff, uint32_t numBytes);
extern void BSP_vme_config(void);

extern unsigned char ReadConfVPD_buff(int offset);

uint32_t bsp_clicks_per_usec;

typedef struct CmdLineRec_ {
    unsigned long  size;
    char           buf[0];
} CmdLineRec, *CmdLine;


#define mtspr(reg, val)  \
  __asm __volatile("mtspr %0,%1" : : "K"(reg), "r"(val))


#define mfspr(reg) \
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
unsigned int BSP_bus_frequency;
/*
 * processor clock frequency
 */
unsigned int BSP_processor_frequency;
/*
 * Time base divisior (how many tick for 1 second).
 */
unsigned int BSP_time_base_divisor;
static unsigned char ConfVPD_buff[200];

#define CMDLINE_BUF_SIZE  2048

static char cmdline_buf[CMDLINE_BUF_SIZE];
char *BSP_commandline_string = cmdline_buf;

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
char *
save_boot_params(
  void *r3,
  void *r4,
  void* r5,
  char *cmdline_start,
  char *cmdline_end
)
{
  int i=cmdline_end-cmdline_start;

  if ( i >= CMDLINE_BUF_SIZE )
    i = CMDLINE_BUF_SIZE-1;
  else if ( i < 0 )
    i = 0;

  memmove(cmdline_buf, cmdline_start, i);
  cmdline_buf[i]=0;
  return cmdline_buf;
}

/*
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
#ifdef CONF_VPD
  int i;
#endif
#ifdef SHOW_LCR1_REGISTER
  unsigned l1cr;
#endif
#ifdef SHOW_LCR2_REGISTER
  unsigned l2cr;
#endif
#ifdef SHOW_LCR3_REGISTER
  unsigned l3cr;
#endif
  uintptr_t intrStackStart;
  uintptr_t intrStackSize;
  ppc_cpu_id_t myCpu;
  ppc_cpu_revision_t myCpuRevision;
  Triv121PgTbl  pt=0;

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
   * 2009 : map the entire 256 M space
   *
   */
  setdbat(3,PCI0_IO_BASE, PCI0_IO_BASE, 0x10000000, IO_PAGE);


  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type() function
   * store the result in global variables so that it can be used latter...
   */
  myCpu   = get_ppc_cpu_type();
  myCpuRevision = get_ppc_cpu_revision();

#ifdef SHOW_LCR1_REGISTER
  l1cr = get_L1CR();
  printk("Initial L1CR value = %x\n", l1cr);
#endif

  /*
   * Initialize the interrupt related settings.
   */
  intrStackStart = (uintptr_t) __rtems_end;
  intrStackSize = rtems_configuration_get_interrupt_stack_size();

  /*
   * Initialize default raw exception handlers.
   */
  sc = ppc_exc_initialize(
    PPC_INTERRUPT_DISABLE_MASK_DEFAULT,
    intrStackStart,
    intrStackSize
  );
  if (sc != RTEMS_SUCCESSFUL) {
    BSP_panic("cannot initialize exceptions");
  }

  /*
   * Init MMU block address translation to enable hardware
   * access
   * More PCI1 memory mapping to be done after BSP_pgtbl_activate.
   */
  printk("-----------------------------------------\n");
  printk("Welcome to %s on MVME5500-0163\n", _RTEMS_version );
  printk("-----------------------------------------\n");

  BSP_mem_size         =  probeMemoryEnd();

  /* TODO: calculate the BSP_bus_frequency using the REF_CLK bit
   *       of System Status  register
   */
  /* rtems_bsp_delay_in_bus_cycles are defined in registers.h */
  BSP_bus_frequency      = 133333333;
  BSP_processor_frequency    = 1000000000;
  /* P94 : 7455 clocks the TB/DECR at 1/4 of the system bus clock frequency */
  BSP_time_base_divisor      = 4000;

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

  /* P94 : 7455 TB/DECR is clocked by the system bus clock frequency */

  bsp_clicks_per_usec    = BSP_bus_frequency/(BSP_time_base_divisor * 1000);

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

#ifdef SHOW_MORE_INIT_SETTINGS
  printk("MSR %x \n", _read_MSR());
  printk("Exit from bspstart\n");
#endif

}

unsigned char ReadConfVPD_buff(int offset)
{
  return(ConfVPD_buff[offset]);
}
