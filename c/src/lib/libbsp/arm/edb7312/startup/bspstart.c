/*
 * Cirrus EP7312 Startup code
 *
 * Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *	
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *
 *  $Id$
*/

#include <bsp.h>
#include <rtems/libcsupport.h>
#include <rtems/libio.h>
#include <ep7312.h>
#include <uart.h>

/*************************************************************/
/* Macros                                                    */
/*************************************************************/

/*************************************************************/
/* Data Structures                                           */
/*************************************************************/

/*************************************************************/
/* Global Variables                                          */
/*************************************************************/
extern void            *_flash_size;
extern void            *_flash_base;
extern void            *_sdram_size;
extern void            *_sdram_base;
extern void            *_bss_free_start;

unsigned long           free_mem_start;
unsigned long           free_mem_end;

/* The original BSP configuration table from the application and our copy of it
   with some changes. */

extern rtems_configuration_table  Configuration;
       rtems_configuration_table  BSP_Configuration;

rtems_cpu_table Cpu_table;                    /* CPU configuration table.    */
char            *rtems_progname;              /* Program name - from main(). */

/*************************************************************/
/* Function prototypes                                       */
/*************************************************************/
extern void rtems_irq_mngt_init(void);
void bsp_libc_init( void *, unsigned32, int );
void bsp_postdriver_hook(void);

/**************************************************************************/
/*                                                                        */
/* NAME: bps_pretasking_hook - Function to setup system before startup    */
/*                                                                        */
/* DESCRIPTION:                                                           */
/*   This function is called before drivers are initialized and used      */
/*   to setup libc and BSP extensions. It configures non cacheable        */
/*   SDRAM, SRAM, AIM, and ADM regions and sets up the CPU's memory       */
/*   protection unit.                                                     */
/*                                                                        */
/* GLOBALS USED:                                                          */
/*   free_mem_start                                                 */
/*   free_mem_end                                                   */
/*                                                                        */
/* RESTRICTIONS/LIMITATIONS:                                              */
/*   Since this function is setting up libc, it cannot use and libc       */
/*   functions.                                                           */
/*                                                                        */
/**************************************************************************/
void bsp_pretasking_hook(void)
{
    unsigned32 heap_start;
    unsigned32 heap_size;


    /* 
     * Set up the heap. It uses all free SDRAM except that reserved
     * for non-cached uses.
     */
    heap_start =  free_mem_start;
    
    /*   heap_size = (free_mem_end - heap_start - MEM_NOCACHE_SIZE); */
    heap_size = 0x200000;

    bsp_libc_init((void *)heap_start, heap_size, 0);

#ifdef RTEMS_DEBUG

  rtems_debug_enable(RTEMS_DEBUG_ALL_MASK);

#endif /* RTEMS_DEBUG */

} /* bsp_pretasking_hook */
 

/**************************************************************************/
/*                                                                        */
/* NAME: bsp_start_default - BSP initialization function                  */
/*                                                                        */
/* DESCRIPTION:                                                           */
/*   This function is called before RTEMS is initialized and used         */
/*   adjust the kernel's configuration.                                   */
/*                                                                        */
/*   This function also configures the CPU's memory protection unit.      */
/*                                                                        */
/* GLOBALS USED:                                                          */
/*    CPU_table                                                    */
/*    BSP_Configuration                                            */
/*    free_mem_start                                               */
/*    free_mem_end                                                 */
/*    free_mem_nocache_start                                       */
/*    _bss_free_start                                              */
/*    mpu_region_tbl                                               */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* RESTRICTIONS/LIMITATIONS:                                              */
/*   Since RTEMS is not configured, no RTEMS functions can be called.     */
/*                                                                        */
/**************************************************************************/
void bsp_start_default( void )
{
    /* disable interrupts */
    *EP7312_INTMR1 = 0;
    *EP7312_INTMR2 = 0;
    Cpu_table.pretasking_hook        = bsp_pretasking_hook;
    Cpu_table.postdriver_hook        = bsp_postdriver_hook;
    Cpu_table.do_zero_of_workspace   = TRUE;
    
    /* Place RTEMS workspace at beginning of free memory. */
    BSP_Configuration.work_space_start = (void *)&_bss_free_start;
    
    free_mem_start = ((unsigned32)&_bss_free_start + 
                      BSP_Configuration.work_space_size);
    
    free_mem_end = ((unsigned32)&_sdram_base + (unsigned32)&_sdram_size);
    
    /*
     * Init rtems exceptions management
     */
    rtems_exception_init_mngt();
    
    /*
     * Init rtems interrupt management
     */
    rtems_irq_mngt_init(); 
    
    
    /*
     *  The following information is very useful when debugging.
     */
#if 0
    printk( "work_space_size = 0x%x\n", BSP_Configuration.work_space_size );
    printk( "maximum_extensions = 0x%x\n", BSP_Configuration.maximum_extensions );
    printk( "microseconds_per_tick = 0x%x\n",
            BSP_Configuration.microseconds_per_tick );
    printk( "ticks_per_timeslice = 0x%x\n",
            BSP_Configuration.ticks_per_timeslice );
    printk( "maximum_devices = 0x%x\n", BSP_Configuration.maximum_devices );
    printk( "number_of_device_drivers = 0x%x\n",
            BSP_Configuration.number_of_device_drivers );
    printk( "Device_driver_table = 0x%x\n",
            BSP_Configuration.Device_driver_table );
    
    /*  printk( "_stack_size = 0x%x\n", _stack_size );*/
    printk( "work_space_start = 0x%x\n", BSP_Configuration.work_space_start );
    printk( "work_space_size = 0x%x\n", BSP_Configuration.work_space_size );
#endif
} /* bsp_start */




/*
 *  By making this a weak alias for bsp_start_default, a brave soul
 *  can override the actual bsp_start routine used.
 */

void bsp_start (void) __attribute__ ((weak, alias("bsp_start_default")));
