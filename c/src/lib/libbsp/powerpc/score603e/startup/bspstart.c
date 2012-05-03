/*  bspstart.c
 *
 *  This set of routines starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before any of these are invoked.
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <string.h>

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <rtems/bspIo.h>
#include <libcpu/cpuIdent.h>
#include <bsp/irq.h>

#define DEBUG 0

/*
 * Where the heap starts; is used by bsp_pretasking_hook;
 */
unsigned int BSP_heap_start;

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
 * Note: Calibrated with an application using a 20ms timer and
 * a scope.
 */
unsigned int BSP_time_base_divisor = 3960;

/*
 *  Driver configuration parameters
 */
uint32_t   bsp_clicks_per_usec;

/*
 * Memory on this board.
 */
extern char RamSize[];
uint32_t BSP_mem_size;

extern unsigned long __rtems_end[];

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
 *  Use the shared implementations of the following routines
 */

void bsp_libc_init( void *, uint32_t, int );

/*PAGE
 *
 *  bsp_predriver_hook
 *
 *  Before drivers are setup initialize interupt vectors.
 */

void init_RTC(void);
void initialize_PMC(void);

void bsp_predriver_hook(void)
{
  init_PCI();
  initialize_universe();

  #if DEBUG
    printk("bsp_predriver_hook: initialize_PCI_bridge\n");
  #endif
  initialize_PCI_bridge ();

#if (HAS_PMC_PSC8)
  #if DEBUG
    printk("bsp_predriver_hook: initialize_PMC\n");
  #endif
  initialize_PMC();
#endif

  #if DEBUG
    printk("bsp_predriver_hook: End of routine\n");
  #endif

}

/*PAGE
 *
 *  initialize_PMC
 */

void initialize_PMC(void) {
  volatile uint32_t     *PMC_addr;
  uint32_t               data;

  /*
   * Clear status, enable SERR and memory space only.
   */
  PMC_addr = BSP_PCI_DEVICE_ADDRESS( 0x4 );
  *PMC_addr = 0x020080cc;
  #if DEBUG
    printk("initialize_PMC: 0x%x = 0x%x\n", PMC_addr, 0x020080cc);
  #endif

  /*
   * set PMC base address.
   */
  PMC_addr  = BSP_PCI_DEVICE_ADDRESS( 0x14 );
  *PMC_addr = (BSP_PCI_REGISTER_BASE >> 24) & 0x3f;
  #if DEBUG
    printk("initialize_PMC: 0x%x = 0x%x\n", PMC_addr, ((BSP_PCI_REGISTER_BASE >> 24) & 0x3f));
  #endif

   PMC_addr = (volatile uint32_t*)
      BSP_PMC_SERIAL_ADDRESS( 0x100000 );
  data = *PMC_addr;
  #if DEBUG
    printk("initialize_PMC: Read 0x%x (0x%x)\n", PMC_addr, data );
    printk("initialize_PMC: Read 0x%x (0x%x)\n", PMC_addr, data & 0xfc );
  #endif
  *PMC_addr = data & 0xfc;
}

/*PAGE
 *
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  unsigned int         msr_value = 0x0000;
  uintptr_t            intrStackStart;
  uintptr_t            intrStackSize;
  ppc_cpu_id_t         myCpu;
  ppc_cpu_revision_t   myCpuRevision;

  rtems_bsp_delay( 1000 );

  /*
   *  Zero out lots of memory
   */
  #if DEBUG
    printk("bsp_start: Zero out lots of memory\n");
  #endif

  BSP_processor_frequency = 266000000;
  BSP_bus_frequency       =  66000000;

  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type()
   * function store the result in global variables so that it can be used
   * later...
   */
  myCpu         = get_ppc_cpu_type();
  myCpuRevision = get_ppc_cpu_revision();
  printk("Cpu: 0x%x  Revision: %d\n", myCpu, myCpuRevision);
  printk("Cpu %s\n", get_ppc_cpu_type_name(myCpu) );

  /*
   * Initialize the interrupt related settings.
   */
  intrStackStart = (uintptr_t) __rtems_end;
  intrStackSize = rtems_configuration_get_interrupt_stack_size();
  printk("Interrupt Stack Start: 0x%x Size: 0x%x  Heap Start: 0x%x\n",
    intrStackStart, intrStackSize, BSP_heap_start
  );

  BSP_mem_size = (uint32_t) RamSize;
  printk("BSP_mem_size: %p\n", RamSize );

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

  msr_value = 0x2030;
  _CPU_MSR_SET( msr_value );
  __asm__ volatile("sync; isync");

  /*
   *  initialize the device driver parameters
   */
  #if DEBUG
    printk("bsp_start: set clicks poer usec\n");
  #endif
  bsp_clicks_per_usec = 66 / 4;

  #if BSP_DATA_CACHE_ENABLED
    #if DEBUG
      printk("bsp_start: cache_enable\n");
    #endif
    instruction_cache_enable ();
    data_cache_enable ();
    #if DEBUG
      printk("bsp_start: END BSP_DATA_CACHE_ENABLED\n");
    #endif
  #endif

  /*
   * Initalize RTEMS IRQ system
   */
  #if DEBUG
    printk("bspstart: Call BSP_rtems_irq_mng_init\n");
  #endif
  BSP_rtems_irq_mng_init(0);

  #if DEBUG
    printk("bsp_start: end BSPSTART\n");
    ShowBATS();
  #endif
}
