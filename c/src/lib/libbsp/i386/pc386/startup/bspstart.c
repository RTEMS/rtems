/*-------------------------------------------------------------------------+
| This file contains the PC386 BSP startup package. It includes application,
| board, and monitor specific initialization and configuration. The generic CPU
| dependent initialization has been performed before this routine is invoked.
+--------------------------------------------------------------------------+
| (C) Copyright 1997 -
| - NavIST Group - Real-Time Distributed Systems and Industrial Automation
|
| http://pandora.ist.utl.pt
|
| Instituto Superior Tecnico * Lisboa * PORTUGAL
+--------------------------------------------------------------------------+
| Disclaimer:
|
| This file is provided "AS IS" without warranty of any kind, either
| expressed or implied.
+--------------------------------------------------------------------------+
| This code is based on:
|   bspstart.c,v 1.8 1996/05/28 13:12:40 joel Exp - go32 BSP
| With the following copyright notice:
| **************************************************************************
| *  COPYRIGHT (c) 1989-1999.
| *  On-Line Applications Research Corporation (OAR).
| *
| *  The license and distribution terms for this file may be
| *  found in found in the file LICENSE in this distribution or at
| *  http://www.rtems.com/license/LICENSE.
| **************************************************************************
|
|  $Id$
+--------------------------------------------------------------------------*/

#include <bsp.h>
#include <rtems/libio.h>
#include <rtems/libcsupport.h>
#include <rtems/pci.h>
#include <libcpu/cpuModel.h>

/*-------------------------------------------------------------------------+
| Global Variables
+--------------------------------------------------------------------------*/
extern uint32_t         _end;         /* End of BSS. Defined in 'linkcmds'. */

/* rudimentary multiboot info */
struct multiboot_info {
	uint32_t	flags;		/* start.S only raises flags for items actually saved; this allows us to check for the size of the data structure */
	uint32_t	mem_lower;	/* avail kB in lower memory */
	uint32_t	mem_upper;	/* avail kB in lower memory */
	/* ... (unimplemented) */
};

extern struct multiboot_info _boot_multiboot_info;
/*
 * Size of heap if it is 0 it will be dynamically defined by memory size,
 * otherwise the value should be changed by binary patch
 */
uint32_t         _heap_size = 0;

/* Alternative way to hardcode the board's memory size [rather than heap size].
 * Can easily be overridden by application.
 */
extern uint32_t bsp_mem_size
  __attribute__ ((weak, alias("bsp_mem_size_default")));
uint32_t bsp_mem_size_default = 0;

/* Size of stack used during initialization. Defined in 'start.s'.  */
extern uint32_t         _stack_size;

uint32_t         rtemsFreeMemStart;
                         /* Address of start of free memory - should be updated
                            after creating new partitions or regions.         */


/*-------------------------------------------------------------------------+
| External Prototypes
+--------------------------------------------------------------------------*/
extern void rtems_irq_mngt_init(void);
void bsp_libc_init( void *, uint32_t, int );

/*-------------------------------------------------------------------------+
|         Function: bsp_pretasking_hook
|      Description: BSP pretasking hook.  Called just before drivers are
|                   initialized. Used to setup libc and install any BSP
|                   extensions. NOTE: Must not use libc (to do io) from here,
|                   since drivers are not yet initialized.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing.
+--------------------------------------------------------------------------*/
void bsp_pretasking_hook(void)
{
  uint32_t topAddr, val;
  int      i, lowest;

  if (rtemsFreeMemStart & (CPU_ALIGNMENT - 1))  /* not aligned => align it */
    rtemsFreeMemStart = (rtemsFreeMemStart+CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

  /* find the lowest 1M boundary to probe */
  lowest = ((rtemsFreeMemStart + (1<<20)) >> 20) + 1;
  if ( lowest  < 2 )
      lowest = 2;

  /* The memory detection algorithm is very crude; try
   * to use multiboot info, if possible (set from start.S)
   */
  if ( bsp_mem_size == 0 &&
       (_boot_multiboot_info.flags & 1) &&
       _boot_multiboot_info.mem_upper ) {
    bsp_mem_size = _boot_multiboot_info.mem_upper * 1024;
  }

  if ( _heap_size == 0 ) {
    if ( bsp_mem_size == 0 ) {
        /*
         * We have to dynamically size memory. Memory size can be anything
         * between no less than 2M and 2048M.
         * let us first write
         */
        for (i=2048; i>=lowest; i--) {
          topAddr = i*1024*1024 - 4;
          *(volatile uint32_t*)topAddr = topAddr;
        }

        for(i=lowest; i<=2048; i++) {
          topAddr = i*1024*1024 - 4;
          val =  *(uint32_t*)topAddr;
          if (val != topAddr) {
            break;
          }
        }
      
        topAddr = (i-1)*1024*1024 - 4;
      } else {
        topAddr = bsp_mem_size;
      }

    if ( rtemsFreeMemStart > topAddr ) {
      printk( "Out of memory -- unable to initialize BSP\n" );
      rtems_fatal_error_occurred( 0x85858585 ); 
    }
 
    _heap_size = topAddr - rtemsFreeMemStart;
  }

  bsp_libc_init((void *)rtemsFreeMemStart, _heap_size, 0);
  rtemsFreeMemStart += _heap_size;           /* HEAP_SIZE  in KBytes */
} /* bsp_pretasking_hook */

/*-------------------------------------------------------------------------+
|         Function: bsp_start
|      Description: Called before main is invoked.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing.
+--------------------------------------------------------------------------*/
void bsp_start_default( void )
{
  int pci_init_retval;
  void Calibrate_loop_1ms(void);

  /*
   * Calibrate variable for 1ms-loop (see timer.c)
   */
  Calibrate_loop_1ms();

  /* set the value of start of free memory. */
  rtemsFreeMemStart = (uint32_t)&_end + _stack_size;

  /* Place RTEMS workspace at beginning of free memory. */

  if (rtemsFreeMemStart & (CPU_ALIGNMENT - 1))  /* not aligned => align it */
    rtemsFreeMemStart = (rtemsFreeMemStart+CPU_ALIGNMENT) & ~(CPU_ALIGNMENT-1);

  Configuration.work_space_start = (void *)rtemsFreeMemStart;
  rtemsFreeMemStart += rtems_configuration_get_work_space_size();

  /*
   * Init rtems interrupt management
   */
  rtems_irq_mngt_init();
  /*
   * Init rtems exceptions management
   */
  rtems_exception_init_mngt();

  /*
   * init PCI Bios interface...
   */
  pci_init_retval = pci_initialize();
  if (pci_init_retval != PCIB_ERR_SUCCESS) {
      printk("PCI bus: could not initialize PCI BIOS interface\n");
  }


#if 0
  printk( "_heap_size = 0x%x\n", _heap_size );
  printk( "_stack_size = 0x%x\n", _stack_size );
  printk( "rtemsFreeMemStart = 0x%x\n", rtemsFreeMemStart );
  printk( "work_space_start = 0x%x\n", rtems_configuration_get_work_space_start() );
  printk( "work_space_size = 0x%x\n", rtems_configuration_get_work_space_size() );
#endif
} /* bsp_start */

/*
 *  By making this a weak alias for bsp_start_default, a brave soul
 *  can override the actual bsp_start routine used.
 */

void bsp_start (void) __attribute__ ((weak, alias("bsp_start_default")));
