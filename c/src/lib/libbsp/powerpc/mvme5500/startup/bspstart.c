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
 *  Modified to support the Synergy VGM & Motorola PowerPC boards.
 *  Many thanks to Till Straumann for providing assistance to port the 
 *  BSP_pgtbl_xxx().
 *  (C) by Till Straumann, <strauman@slac.stanford.edu>, 2002, 2004
 *
 *  Modified to support the MVME5500 board
 *  (C) by S. Kate Feng <feng1@bnl.gov>, 2003, 2004
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

uint32_t
_bsp_sbrk_init(uint32_t heap_start, uint32_t *heap_size_p);

/* provide access to the command line parameters */
char *BSP_commandline_string = 0;

BSP_output_char_function_type BSP_output_char = BSP_output_char_via_serial;

extern void _return_to_ppcbug();
extern unsigned long __rtems_end[];
extern void L1_caches_enables();
extern unsigned get_L1CR(), get_L2CR(), get_L3CR();
extern unsigned set_L2CR(unsigned);
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

/*
 * system init stack and soft ir stack size
 */
#define INIT_STACK_SIZE 0x1000
#define INTR_STACK_SIZE CONFIGURE_INTERRUPT_STACK_MEMORY

/* calculate the heap start */
static unsigned long
heapStart(void)
{
unsigned long rval;
    rval = ((uint32_t) __rtems_end) +INIT_STACK_SIZE + INTR_STACK_SIZE;
    if (rval & (CPU_ALIGNMENT-1))
        rval = (rval + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);
	return rval;
}

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
    uint32_t        heap_start=heapStart();    
    uint32_t        heap_size,heap_sbrk_spared;
    extern uint32_t _bsp_sbrk_init(uint32_t, uint32_t*);

    heap_size = (BSP_mem_size - heap_start) - BSP_Configuration.work_space_size;

    heap_sbrk_spared=_bsp_sbrk_init(heap_start, &heap_size);

#ifdef SHOW_MORE_INIT_SETTINGS
   	printk(" HEAP start %x  size %x (%x bytes spared for sbrk)\n", heap_start, heap_size, heap_sbrk_spared);
#endif    

    bsp_libc_init((void *) 0, heap_size, heap_sbrk_spared);

#ifdef RTEMS_DEBUG
    rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif
}

void zero_bss()
{
  /* prevent these from being accessed in the short data areas */
  extern unsigned long __bss_start[], __sbss_start[], __sbss_end[];
  extern unsigned long __sbss2_start[], __sbss2_end[];
  memset(__sbss_start, 0, ((unsigned) __sbss_end) - ((unsigned)__sbss_start));
  memset(__sbss2_start, 0, ((unsigned) __sbss2_end) - ((unsigned)__sbss2_start));
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
CmdLine future_heap=(CmdLine)heapStart();

 	/* get the string out of the stack area into the future heap region;
	 * assume there's enough memory...
	 */
	memmove(future_heap->buf,cmdline_start,i);
	/* make sure there's an end of string marker */
	future_heap->buf[i++]=0;
	future_heap->size=i;
}


/* Configure and enable the L3CR */
void config_enable_L3CR(unsigned l3cr)
{
	unsigned x;

	/* By The Book (numbered steps from section 3.7.3.1 of MPC7450UM) */				
	/*
	 * 1: Set all L3CR bits for final config except L3E, L3I, L3PE, and
	 *    L3CLKEN.  (also mask off reserved bits in case they were included
	 *    in L3CR_CONFIG)
	 */
	l3cr &= ~(L3CR_L3E|L3CR_L3I|L3CR_LOCK_745x|L3CR_L3PE|L3CR_L3CLKEN|L3CR_RESERVED);
	mtspr(L3CR, l3cr);

	/* 2: Set L3CR[5] (otherwise reserved bit) to 1 */
	l3cr |= 0x04000000;
	mtspr(L3CR, l3cr);

	/* 3: Set L3CLKEN to 1*/
	l3cr |= L3CR_L3CLKEN;
	mtspr(L3CR, l3cr);

	/* 4/5: Perform a global cache invalidate (ref section 3.7.3.6) */
	__asm __volatile("dssall;sync");
	/* L3 cache is already disabled, no need to clear L3E */
	mtspr(L3CR, l3cr|L3CR_L3I);

	do {
	       x = mfspr(L3CR);
	} while (x & L3CR_L3I);
	
	/* 6: Clear L3CLKEN to 0 */
	l3cr &= ~L3CR_L3CLKEN;
	mtspr(L3CR, l3cr);

	/* 7: Perform a 'sync' and wait at least 100 CPU cycles */
	__asm __volatile("sync");
	rtems_bsp_delay_in_bus_cycles(100);

	/* 8: Set L3E and L3CLKEN */
	l3cr |= (L3CR_L3E|L3CR_L3CLKEN);
	mtspr(L3CR, l3cr);

	/* 9: Perform a 'sync' and wait at least 100 CPU cycles */
	__asm __volatile("sync");

	rtems_bsp_delay_in_bus_cycles(100);
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
  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type() function
   * store the result in global variables so that it can be used latter...
   */
  myCpu 	= get_ppc_cpu_type();
  myCpuRevision = get_ppc_cpu_revision();

  /*
   * enables L1 Cache. Note that the L1_caches_enables() codes checks for
   * relevant CPU type so that the reason why there is no use of myCpu...
   *
   * MOTLoad default is good. Otherwise, one would have to disable L2, L3
   * first before settting L1.  Then L1->L2->L3.
   * 
   L1_caches_enables();*/
  
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
  intrStack = ((uint32_t) __rtems_end) + 
          INIT_STACK_SIZE + INTR_STACK_SIZE - PPC_MINIMUM_STACK_FRAME_SIZE;

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
  initialize_exceptions();
  /*
   * Init MMU block address translation to enable hardware
   * access
   * More PCI1 memory mapping to be done after BSP_pgtbl_activate.
   */
  /*
   * PCI 0 domain memory space, want to leave room for the VME window 
   */
  setdbat(2, PCI0_MEM_BASE, PCI0_MEM_BASE, 0x10000000, IO_PAGE);

  /* map the PCI 0, 1 Domain I/O space, GT64260B registers
   * and the reserved area so that the size is the power of 2.
   */
  setdbat(3,PCI0_IO_BASE, PCI0_IO_BASE, 0x2000000, IO_PAGE);

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
  _CPU_Table                     = Cpu_table;/* <skf> for rtems_bsp_delay() */

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

  /*
   * Enable L2 Cache. Note that the set_L2CR(L2CR) codes checks for
   * relevant CPU type (mpc750)...
   *
   * It also takes care of flushing the cache under certain conditions:
   *   current    going to (E==enable, I==invalidate)
   *     E           E | I	-> __NOT_FLUSHED_, invalidated, stays E
   *     E               I	-> flush & disable, invalidate
   *     E           E		-> nothing, stays E
   *     0           E | I	-> not flushed, invalidated, enabled
   *     0             | I	-> not flushed, invalidated, stays off
   *     0           E      -> not flushed, _NO_INVALIDATE, enabled
   *
   * The first and the last combinations are potentially dangerous!
   *
   * NOTE: we assume the essential cache parameters (speed, size etc.)
   *       have been set correctly by the firmware!
   *
   */
#ifdef SHOW_LCR2_REGISTER
  l2cr = get_L2CR();
  printk("Initial L2CR value = %x\n", l2cr);
#endif  
#if 0
  /* Again, MOTload setup seems to be fine. Otherwise, one would
   * have to disable the L3 cahce, then R2 ->R3
   */
  if ( -1 != (int)l2cr ) {
	/* -1 would mean that this machine doesn't support L2 */

	l2cr &= ~( L2CR_LOCK_745x); /* clear 'data only' and 'instruction only' */
	l2cr |= L2CR_L3OH0;    /* L3 output hold 0 should be set */
	if ( ! (l2cr & L2CR_L2E) ) {
	    /* we are going to enable the L2 - hence we
	     * MUST invalidate it first; however, if
	     * it was enabled already, we MUST NOT
	     * invalidate it!!
	     */
	     l2cr |= L2CR_L2E | L2CR_L2I;
	     l2cr=set_L2CR(l2cr);
        }
	l2cr=set_L2CR(l2cr);
  }
#endif

#ifdef SHOW_LCR3_REGISTER
  /* L3CR needs DEC int. handler installed for bsp_delay()*/
  l3cr = get_L3CR();
  printk("Initial L3CR value = %x\n", l3cr);
#endif  

#if 0 
  /* Again, use the MOTLoad default for L3CR again */
  if ( -1 != (int)l3cr ) {
 	/* -1 would mean that this machine doesn't support L3 */
        /* BSD : %2 , SDRAM late wirte
	   l3cr |= L3SIZ_2M|L3CLK_20|L3RT_PIPELINE_LATE; */
        /* MOTLOad :0xDF826000-> %5, 4 clocks sample point,3 p-clocks SP */
        l3cr |= L3CR_L3PE| L3SIZ_2M|L3CLK_50|L3CKSP_4|L3PSP_3;

	/* TOCHECK MOTload had L2 cache enabled, try to set nothing first */
	if ( !(l3cr & L3CR_L3E)) {
           l3cr |= L3CR_L3E | L3CR_L3I;
	   config_enable_L3CR(l3cr);
        }
  }
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
