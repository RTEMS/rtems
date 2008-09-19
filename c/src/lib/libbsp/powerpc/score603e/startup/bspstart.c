/*  bspstart.c
 *
 *  This set of routines starts the application.  It includes application,
 *  board, and monitor specific initialization and configuration.
 *  The generic CPU dependent initialization has been performed
 *  before any of these are invoked.
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id:
 */

#include <string.h>

#include <bsp.h>
#include <rtems/bspIo.h>
#include <rtems/libio.h>
#include <libcpu/cpuIdent.h>
#define DEBUG 1

/*
 * Where the heap starts; is used by bsp_pretasking_hook;
 */
unsigned int BSP_heap_start;

/*
 * Total RAM available and associated linker symbol
 */
unsigned int BSP_mem_size;
extern char  RamSize[];

/*
 * PCI Bus Frequency
 */
unsigned int BSP_bus_frequency;  /* XXX - Set this based upon the Score board */

/*
 * processor clock frequency
 */
unsigned int BSP_processor_frequency; /* XXX - Set this based upon the Score board */

/*
 * Time base divisior (how many tick for 1 second).
 */
unsigned int BSP_time_base_divisor = 1000;  /* XXX - Just a guess */

extern unsigned long __rtems_end[];

/*
 *  Driver configuration parameters
 */
uint32_t   bsp_clicks_per_usec;

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
 *  bsp_predriver_hook
 *
 *  Before drivers are setup initialize interupt vectors.
 */

void init_RTC();
void initialize_PMC();

void bsp_predriver_hook(void)
{
  #if DEBUG
    printk("bsp_predriver_hook: init_RTC\n");
  #endif
  init_RTC();
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

#if 0
 /*
  * Initialize Bsp General purpose vector table.
  */
  #if DEBUG
    printk("bsp_predriver_hook: initialize_external_exception_vector\n");
  #endif
 initialize_external_exception_vector();
#endif

#if (0)
  /*
   * XXX - Modify this to write a 48000000 (loop to self) command
   *       to each interrupt location.  This is better for debug.
   */
  #if DEBUG
    printk("bsp_predriver_hook: bsp_spurious_initialize\n");
  #endif
 bsp_spurious_initialize();
#endif

  ShowBATS();

  #if DEBUG
    printk("bsp_predriver_hook: End of routine\n");
  #endif

}

/*PAGE
 *
 *  initialize_PMC
 */

void initialize_PMC() {
  volatile uint32_t         *PMC_addr;
  uint8_t          data;

#if (0) /* First Values sent */
  /*
   * set PMC base address.
   */
  PMC_addr  = BSP_PCI_DEVICE_ADDRESS( 0x14 );
  *PMC_addr = (BSP_PCI_REGISTER_BASE >> 24) & 0x3f;

  /*
   * Clear status, enable SERR and memory space only.
   */
  PMC_addr = BSP_PCI_DEVICE_ADDRESS( 0x4 );
  *PMC_addr = 0x0201ff37;

  /*
   * Bit 0 and 1 HI cause Medium Loopback to occur.
   */
  PMC_addr = (volatile uint32_t*)
        BSP_PMC_SERIAL_ADDRESS( 0x100000 );
  data = *PMC_addr;
  /*   *PMC_addr = data | 0x3;  */
  *PMC_addr = data & 0xfc;

#endif

#if (1)

  /*
   * Clear status, enable SERR and memory space only.
   */
  #if DEBUG
    printk("initialize_PMC: set Device Address 0x4 \n");
  ShowBATS();
  #endif
  PMC_addr = BSP_PCI_DEVICE_ADDRESS( 0x4 );
  *PMC_addr = 0x020080cc;

  /*
   * set PMC base address.
   */
  #if DEBUG
    printk("initialize_PMC: set Device Address 0x14 \n");
  ShowBATS();
  #endif
  PMC_addr  = BSP_PCI_DEVICE_ADDRESS( 0x14 );
  *PMC_addr = (BSP_PCI_REGISTER_BASE >> 24) & 0x3f;

  #if DEBUG
    printk("initialize_PMC: set PMC Serial Address 0x100000\n");
  #endif
   PMC_addr = (volatile uint32_t*)
      BSP_PMC_SERIAL_ADDRESS( 0x100000 );
  data = *PMC_addr;
  *PMC_addr = data & 0xfc;

#endif
}

/*PAGE
 *
 *  bsp_postdriver_hook
 *
 *  Standard post driver hook plus some BSP specific stuff.
 */

void bsp_postdriver_hook(void)
{
  extern void Init_EE_mask_init(void);
  extern void open_dev_console(void);

  #if DEBUG
    printk("bsp_postdriver_hook: initialize libio\n");
  #endif
  if (rtems_libio_supp_helper)
    (*rtems_libio_supp_helper)();
  ShowBATS();

  #if DEBUG
    printk("bsp_postdriver_hook: Init_EE_mask_init\n");
  #endif
  Init_EE_mask_init();
  ShowBATS();
  #if DEBUG
    printk("bsp_postdriver_hook: Finished procedure\n");
  #endif
}

void bsp_set_trap_vectors( void );

/*PAGE
 *
 *  bsp_start
 *
 *  This routine does the bulk of the system initialization.
 */

void bsp_start( void )
{
  unsigned int         msr_value = 0x0000;
  uint32_t             intrStackStart;
  uint32_t             intrStackSize;
  volatile uint32_t   *ptr;
  ppc_cpu_id_t         myCpu;
  ppc_cpu_revision_t   myCpuRevision;
 
  rtems_bsp_delay( 1000 );

  /*
   *  Zero out lots of memory
   */
  #if DEBUG
    printk("bsp_start: Zero out lots of memory\n");
    ShowBATS();
  #endif

  memset(
    &end,
    0,
    (unsigned char *)&RAM_END - (unsigned char *) &end
  );

  BSP_processor_frequency = 266000000;
  BSP_bus_frequency       =  66000000;

  /*
   * Get CPU identification dynamically. Note that the get_ppc_cpu_type()
   * function store the result in global variables so that it can be used 
   * later...
   */
  myCpu         = get_ppc_cpu_type();
  myCpuRevision = get_ppc_cpu_revision();

  /*
   * Initialize the interrupt related settings.
   */
  intrStackStart = (uint32_t) __rtems_end;
  intrStackSize = rtems_configuration_get_interrupt_stack_size();

  BSP_heap_start = intrStackStart + intrStackSize;
  BSP_mem_size = (uintptr_t) RamSize;

  /*
   * Initialize default raw exception handlers.
   */
printk("ppc_exc_initialize\n");
  ppc_exc_initialize(
    PPC_INTERRUPT_DISABLE_MASK_DEFAULT,
    intrStackStart,
    intrStackSize
  );

  /*
   *  There are multiple ROM monitors available for this board.
   */
#if (SCORE603E_USE_SDS)
  #if DEBUG
    printk("bsp_start: USE SDS\n");
  #endif


  /*
   * Write instruction for Unconditional Branch to ROM vector.
   */

   Code = 0x4bf00002;
   for (Address = 0x100; Address <= 0xe00; Address += 0x100) {
     A_Vector = (uint32_t*)Address;
     Code = 0x4bf00002 + Address;
     *A_Vector = Code;
   }

   for (Address = 0x1000; Address <= 0x1400; Address += 0x100) {
     A_Vector = (uint32_t*)Address;
     Code = 0x4bf00002 + Address;
     *A_Vector = Code;
   }

  msr_value = 0x2030;

#elif (SCORE603E_USE_OPEN_FIRMWARE)
  #if DEBUG
    printk("bsp_start: USE OPEN FIRMWARE\n");
  #endif
  msr_value = 0x2030;

#elif (SCORE603E_USE_NONE)
  #if DEBUG
    printk("bsp_start: USE NONE\n");
  #endif
  msr_value = 0x2030;
  _CPU_MSR_SET( msr_value );
  bsp_set_trap_vectors();

#elif (SCORE603E_USE_DINK)
  #if DEBUG
    printk("bsp_start: USE DINK\n");
  #endif
  msr_value = 0x2030;
  _CPU_MSR_SET( msr_value );

  /*
   * Override the DINK error on a Decrementor interrupt.
   */
  /* org    dec_vector  - rfi */
  ptr = (uint32_t*)0x900;
  *ptr = 0x4c000064;

#else
  #if DEBUG
    printk("bsp_start: ERROR unknow ROM Monitor\n");
  #endif
#error "SCORE603E BSPSTART.C -- what ROM monitor are you using"
#endif

  _CPU_MSR_SET( msr_value );

  /*
   *  initialize the device driver parameters
   */
  #if DEBUG
    printk("bsp_start: set clicks poer usec\n");
  #endif
  bsp_clicks_per_usec = 66 / 4;  /* XXX get from linkcmds */

#if ( PPC_USE_DATA_CACHE )
  #if DEBUG
    printk("bsp_start: cache_enable\n");
  #endif
  instruction_cache_enable ();
  data_cache_enable ();
  #if DEBUG
    printk("bsp_start: END PPC_USE_DATA_CACHE\n");
  #endif
#endif
  #if DEBUG
    printk("bsp_start: end BSPSTART\n");
  ShowBATS();
  #endif
}
