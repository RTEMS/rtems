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

/*
 *  The original table from the application and our copy of it with
 *  some changes.
 */
 
extern rtems_configuration_table  Configuration;
rtems_configuration_table         BSP_Configuration;
rtems_cpu_table                   Cpu_table;
rtems_unsigned32                  bsp_isr_level;

/*
 *  Use the shared implementations of the following routines
 */

void bsp_postdriver_hook(void);
void bsp_libc_init( void *, unsigned32, int );

/*PAGE
 *
 *  bsp_pretasking_hook
 *
 *  BSP pretasking hook.  Called just before drivers are initialized.
 *  Used to setup libc and install any BSP extensions.
 */

void bsp_pretasking_hook(void)
{
  extern int end;
  rtems_unsigned32 heap_start;
  rtems_unsigned32 heap_size;

  heap_start = (rtems_unsigned32) &end;
  if (heap_start & (CPU_ALIGNMENT-1))
    heap_start = (heap_start + CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

  heap_size = BSP_Configuration.work_space_start - (void *)&end;
  heap_size &= 0xfffffff0;  /* keep it as a multiple of 16 bytes */

  bsp_libc_init((void *) heap_start, heap_size, 0);

#ifdef RTEMS_DEBUG
  rtems_debug_enable( RTEMS_DEBUG_ALL_MASK );
#endif
}

/*PAGE
 *
 *  bsp_predriver_hook
 *
 *  Before drivers are setup initialize interupt vectors.
 */ 

void init_RTC();
void initialize_PMC();

void bsp_predriver_hook(void)
{
  init_RTC();

  init_PCI();
  initialize_universe();
  initialize_PCI_bridge ();

#if (HAS_PMC_PSC8)
  initialize_PMC();
#endif

 /* 
  * Initialize Bsp General purpose vector table.
  */
 initialize_external_exception_vector();

#if (0)
  /* 
   * XXX - Modify this to write a 48000000 (loop to self) command
   *       to each interrupt location.  This is better for debug.
   */
 bsp_spurious_initialize();
#endif

}

/*PAGE
 *
 *  initialize_PMC
 */

void initialize_PMC() {
  volatile rtems_unsigned32 *PMC_addr;
  rtems_unsigned8  data;

#if (0) /* First Values sent */
  /*
   * set PMC base address.
   */
  PMC_addr  = SCORE603E_PCI_DEVICE_ADDRESS( 0x14 );
  *PMC_addr = (SCORE603E_PCI_REGISTER_BASE >> 24) & 0x3f;

  /*
   * Clear status, enable SERR and memory space only.
   */
  PMC_addr = SCORE603E_PCI_DEVICE_ADDRESS( 0x4 );
  *PMC_addr = 0x0201ff37;

  /*
   * Bit 0 and 1 HI cause Medium Loopback to occur.
   */
  PMC_addr = (volatile rtems_unsigned32 *)
        SCORE603E_PMC_SERIAL_ADDRESS( 0x100000 );
  data = *PMC_addr;
  /*   *PMC_addr = data | 0x3;  */
  *PMC_addr = data & 0xfc;

#endif


#if (1)

  /*
   * Clear status, enable SERR and memory space only.
   */
  PMC_addr = SCORE603E_PCI_DEVICE_ADDRESS( 0x4 );
  *PMC_addr = 0x020080cc;

  /*
   * set PMC base address.
   */
  PMC_addr  = SCORE603E_PCI_DEVICE_ADDRESS( 0x14 );
  *PMC_addr = (SCORE603E_PCI_REGISTER_BASE >> 24) & 0x3f;

  PMC_addr = (volatile rtems_unsigned32 *)
      SCORE603E_PMC_SERIAL_ADDRESS( 0x100000 );
  data = *PMC_addr;
  *PMC_addr = data & 0xfc;

#endif
}



/*PAGE
 *
 *  SCORE603e_bsp_postdriver_hook
 *
 *  Standard post driver hook plus some BSP specific stuff.
 */
 
void SCORE603e_bsp_postdriver_hook(void)
{
  extern void Init_EE_mask_init(void);

  bsp_postdriver_hook();

  Init_EE_mask_init();
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
  unsigned char *work_space_start;
  unsigned int  msr_value = 0x0000;
  volatile rtems_unsigned32 *ptr;

  rtems_bsp_delay( 1000 );

  /*
   *  Zero out lots of memory
   */

  memset(
    &end,
    0,
    (unsigned char *)&RAM_END - (unsigned char *) &end
  );

  /*
   *  There are multiple ROM monitors available for this board.  
   */
#if (SCORE603E_USE_SDS)

  /* 
   * Write instruction for Unconditional Branch to ROM vector.
   */
    
   Code = 0x4bf00002;          
   for (Address = 0x100; Address <= 0xe00; Address += 0x100) { 
     A_Vector = (unsigned32    *)Address;
     Code = 0x4bf00002 + Address;
     *A_Vector = Code;
   }
    
   for (Address = 0x1000; Address <= 0x1400; Address += 0x100) { 
     A_Vector = (unsigned32    *)Address;
     Code = 0x4bf00002 + Address;
     *A_Vector = Code;
   }
   
  Cpu_table.exceptions_in_RAM = TRUE;
  msr_value = 0x2030;

#elif (SCORE603E_USE_OPEN_FIRMWARE)
  Cpu_table.exceptions_in_RAM = TRUE;
  msr_value = 0x2030;

#elif (SCORE603E_USE_NONE)
  Cpu_table.exceptions_in_RAM = TRUE;
  msr_value = 0x2030;
  _CPU_MSR_SET( msr_value );
  bsp_set_trap_vectors();

#elif (SCORE603E_USE_DINK)
  Cpu_table.exceptions_in_RAM = TRUE;
  msr_value = 0x2030;
  _CPU_MSR_SET( msr_value );

  /*
   * Override the DINK error on a Decrementor interrupt.
   */
  /* org    dec_vector  - rfi */
  ptr = (rtems_unsigned32 *)0x900;
  *ptr = 0x4c000064;

#else
#error "SCORE603E BSPSTART.C -- what ROM monitor are you using"
#endif

  _CPU_MSR_SET( msr_value );

  /*
   *  Need to "allocate" the memory for the RTEMS Workspace and
   *  tell the RTEMS configuration where it is.  This memory is
   *  not malloc'ed.  It is just "pulled from the air".
   */

  work_space_start = 
    (unsigned char *)&RAM_END - BSP_Configuration.work_space_size;

  if ( work_space_start <= (unsigned char *)&end ) {
    DEBUG_puts( "bspstart: Not enough RAM!!!\n" );
    bsp_cleanup();
  }

  BSP_Configuration.work_space_start = work_space_start;

  /*
   *  initialize the CPU table for this BSP
   */

  /* Cpu_table.exceptions_in_RAM was set above */
  Cpu_table.pretasking_hook = bsp_pretasking_hook; /* init libc, etc. */
  Cpu_table.predriver_hook  = bsp_predriver_hook;   /* Init vectors    */
  Cpu_table.postdriver_hook = SCORE603e_bsp_postdriver_hook;
  Cpu_table.clicks_per_usec = 66 / 4;  /* XXX get from linkcmds */
  Cpu_table.do_zero_of_workspace = TRUE;
  Cpu_table.interrupt_stack_size = CONFIGURE_INTERRUPT_STACK_MEMORY;
  Cpu_table.idle_task_stack_size = (3 * STACK_MINIMUM_SIZE);

#if ( PPC_USE_DATA_CACHE )
  instruction_cache_enable (); 
  data_cache_enable ();
#endif
}

