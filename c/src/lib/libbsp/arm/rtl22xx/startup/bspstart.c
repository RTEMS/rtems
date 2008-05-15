/*
 *  LPC22XX/LPC21xx Startup code
 *
 *  Copyright (c) 2007 by Ray Xu <rayx.cn@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 *  $Id$
*/

#include <bsp.h>
#include <rtems/libcsupport.h>
#include <rtems/libio.h>
#include <lpc22xx.h>


/*************************************************************/
/* Macros                                                    */
/*************************************************************/

/*************************************************************/
/* Data Structures                                           */
/*************************************************************/

/*************************************************************/
/* Global Variables                                          */
/*************************************************************/
/*FIX ME:we do not have SDRAM, but can I define mutiple SRAM?*/

extern void            *_flash_size;
extern void            *_flash_base;
extern void            *_sdram_size;
extern void            *_sdram_base;
extern void            *_bss_free_start;
extern void            *_bss_start_;
extern void            *_bss_end_;

unsigned long           free_mem_start;
unsigned long           free_mem_end;

/*************************************************************/
/* Function prototypes                                       */
/*************************************************************/
extern void rtems_irq_mngt_init(void);
void bsp_libc_init( void *, uint32_t, int );
extern void  UART0_Ini(void);
extern void printi(unsigned long);
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
    uint32_t   heap_start;
    uint32_t   heap_size;

    /*
     * Set up the heap. It uses all free SDRAM except that reserved
     * for non-cached uses.
     */
    heap_start =  free_mem_start;

    /*   heap_size = (free_mem_end - heap_start - MEM_NOCACHE_SIZE); */
    /*the board seems to have only 512K memory, we use 256K as heap, 256K to 
    store the .text*/
    heap_size = 0x40000;

    bsp_libc_init((void *)heap_start, heap_size, 0);
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
/*    Configuration                                                       */
/*    free_mem_start                                                      */
/*    free_mem_end                                                        */
/*    free_mem_nocache_start                                              */
/*    _bss_free_start                                                     */
/*    mpu_region_tbl                                                      */
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
    PINSEL2 =0x0f814914;
    BCFG0 = 0x1000ffef;
    BCFG1 = 0x1000ffef;

    MEMMAP = 0x2;  //debug and excute outside chip

    PLLCON = 1;
#if (Fpclk / (Fcclk / 4)) == 1
    VPBDIV = 0;
#endif
#if (Fpclk / (Fcclk / 4)) == 2
    VPBDIV = 2;
#endif
#if (Fpclk / (Fcclk / 4)) == 4
    VPBDIV = 1;
#endif

#if (Fcco / Fcclk) == 2
    PLLCFG = ((Fcclk / Fosc) - 1) | (0 << 5);
#endif
#if (Fcco / Fcclk) == 4
    PLLCFG = ((Fcclk / Fosc) - 1) | (1 << 5);
#endif
#if (Fcco / Fcclk) == 8
    PLLCFG = ((Fcclk / Fosc) - 1) | (2 << 5);
#endif
#if (Fcco / Fcclk) == 16
    PLLCFG = ((Fcclk / Fosc) - 1) | (3 << 5);
#endif
    PLLFEED = 0xaa;
    PLLFEED = 0x55;
    while((PLLSTAT & (1 << 10)) == 0);
    PLLCON = 3;
    PLLFEED = 0xaa;
    PLLFEED = 0x55;

    /* memory configure */
    /* it is not needed in my formatter board */
    //MAMCR = 0;
    // MAMTIM = 3;
    //MAMCR = 2;

    /* init VIC */
    VICIntEnClr = 0xffffffff;
    VICVectAddr = 0;
    VICIntSelect = 0;

    /* disable interrupts */
    /* Setup interrupt controller.*/
    VICProtection = 0;

    /* Place RTEMS workspace at beginning of free memory. */
    Configuration.work_space_start = (void *)&_bss_free_start;

    free_mem_start = ((uint32_t)&_bss_free_start +
                      rtems_configuration_get_work_space_size());

    free_mem_end = ((uint32_t)&_sdram_base + (uint32_t)&_sdram_size);

    UART0_Ini();

    #if 0
    printk(" bsp_start_default 0x%08x\n", (int)&bsp_start_defalt);
    printk(" _bss_free_start 0x%08x\n", (int)&_bss_free_start);
    printk(" free_mem_start 0x%08x\n", (int)free_mem_start);
    printk(" _sdram_base 0x%08x\n", (int)&_sdram_base);
    printk(" _sdram_size 0x%08x\n", (int)&_sdram_size);
    printk(" free_mem_end 0x%08x\n", (int)free_mem_end);
    #endif

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

    printk( "work_space_size = 0x%x\n",
            rtems_configuration_get_work_space_size() );
    printk( "microseconds_per_tick = 0x%x\n",
            rtems_configuration_get_microseconds_per_tick() );
    printk( "ticks_per_timeslice = 0x%x\n",
            rtems_configuration_get_ticks_per_timeslice() );

    /*  printk( "_stack_size = 0x%x\n", _stack_size );*/
    printk( "work_space_start = 0x%x\n", Configuration.work_space_start );
    printk( "work_space_size = 0x%x\n", rtems_configuration_get_work_space_size() );
#endif
} /* bsp_start */


/**
 *  Reset the system.
 *
 *  This functions enables the watchdog and waits for it to 
 *  fire, thus resetting the system.
 */
/**
 *  Reset the system.
 *
 *  This functions enables the watchdog and waits for it to 
 *  fire, thus resetting the system.
 */
void bsp_reset(void)
{
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);

    while(1);
}

/*
 *  By making this a weak alias for bsp_start_default, a brave soul
 *  can override the actual bsp_start routine used.
 */

void bsp_start (void) __attribute__ ((weak, alias("bsp_start_default")));
