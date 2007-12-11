/*
 * Cogent CSB336 - MC9328MXL SBC startup code
 *
 * Copyright (c) 2004 by Cogent Computer Systems
 * Written by Jay Monkman <jtm@lopingdog.com>
 *      
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
#include <bsp.h>
#include <rtems/libcsupport.h>
#include <rtems/bspIo.h>
#include <rtems/libio.h>
#include <mc9328mxl.h>

/* Global Variables */
extern void            *_flash_size;
extern void            *_flash_base;
extern void            *_sdram_size;
extern void            *_sdram_base;
extern void            *_bss_free_start;

unsigned long           free_mem_start;
unsigned long           free_mem_end;

extern void rtems_irq_mngt_init(void);
void bsp_libc_init( void *, uint32_t, int );
void bsp_postdriver_hook(void);

/**************************************************************************/
/*                                                                        */
/* NAME: bsp_pretasking_hook - Function to setup system before startup    */
/*                                                                        */
/* DESCRIPTION:                                                           */
/*   This function is called before drivers are initialized and used      */
/*   to setup libc and BSP extensions.                                    */
/*                                                                        */
/* RESTRICTIONS/LIMITATIONS:                                              */
/*   Since this function is setting up libc, it cannot use and libc       */
/*   functions.                                                           */
/*                                                                        */
/**************************************************************************/
void bsp_pretasking_hook(void)
{
    uint32_t heap_start;
    uint32_t heap_size;

    /* 
     * Set up the heap. 
     */
    heap_start =  free_mem_start;
    heap_size = free_mem_end - free_mem_start;

    /* call rtems lib init - malloc stuff */
    bsp_libc_init((void *)heap_start, heap_size, 0);

#ifdef RTEMS_DEBUG

    rtems_debug_enable(RTEMS_DEBUG_ALL_MASK);

#endif /* RTEMS_DEBUG */

} 
 

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
/* RESTRICTIONS/LIMITATIONS:                                              */
/*   Since RTEMS is not configured, no RTEMS functions can be called.     */
/*                                                                        */
/**************************************************************************/
void mmu_set_cpu_async_mode(void);
void bsp_start_default( void )
{
    int i;

    /* Set the MCU prescaler to divide by 1 */
    MC9328MXL_PLL_CSCR &= ~MC9328MXL_PLL_CSCR_PRESC;

    /* Enable the MCU PLL */
    MC9328MXL_PLL_CSCR |= MC9328MXL_PLL_CSCR_MPEN;

    /* Delay to allow time for PLL to get going */
    for (i = 0; i < 100; i++) {
        asm volatile ("nop\n");
    }

    /* Set the CPU to asynchrous clock mode, so it uses its fastest clock */
    mmu_set_cpu_async_mode();

    /* disable interrupts */
    MC9328MXL_AITC_INTENABLEL = 0;
    MC9328MXL_AITC_INTENABLEH = 0;

    /* Set interrupt priority to -1 (allow all priorities) */
    MC9328MXL_AITC_NIMASK = 0x1f;

    /* Place RTEMS workspace at beginning of free memory. */
    Configuration.work_space_start = (void *)&_bss_free_start;
    
    free_mem_start = ((uint32_t)&_bss_free_start + 
                      rtems_configuration_get_work_space_size());
    
    free_mem_end = ((uint32_t)&_sdram_base + (uint32_t)&_sdram_size);

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
    printk( "work_space_start = 0x%x\n", 
            Configuration.work_space_start );
    printk( "work_space_size = 0x%x\n", 
            rtems_configuration_get_work_space_size() );
#endif
} /* bsp_start */



/* Calcuate the frequency for perclk1 */
int get_perclk1_freq(void)
{
    unsigned int fin;
    unsigned int fpll;
    unsigned int pd;
    unsigned int mfd;
    unsigned int mfi;
    unsigned int mfn;
    uint32_t reg;
    int perclk1;

    if (MC9328MXL_PLL_CSCR & MC9328MXL_PLL_CSCR_SYSSEL) {
        /* Use external oscillator */
        fin = BSP_OSC_FREQ;
    } else {
        /* Use scaled xtal freq */
        fin = BSP_XTAL_FREQ * 512;
    }

    /* calculate the output of the system PLL */
    reg = MC9328MXL_PLL_SPCTL0;
    pd = ((reg & MC9328MXL_PLL_SPCTL_PD_MASK) >> 
          MC9328MXL_PLL_SPCTL_PD_SHIFT);
    mfd = ((reg & MC9328MXL_PLL_SPCTL_MFD_MASK) >> 
           MC9328MXL_PLL_SPCTL_MFD_SHIFT);
    mfi = ((reg & MC9328MXL_PLL_SPCTL_MFI_MASK) >> 
           MC9328MXL_PLL_SPCTL_MFI_SHIFT);
    mfn = ((reg & MC9328MXL_PLL_SPCTL_MFN_MASK) >> 
           MC9328MXL_PLL_SPCTL_MFN_SHIFT);

#if 0
    printk("fin = %d\n", fin);
    printk("pd = %d\n", pd);
    printk("mfd = %d\n", mfd);
    printk("mfi = %d\n", mfi);
    printk("mfn = %d\n", mfn);
    printk("rounded (fin * mfi) / (pd + 1) = %d\n", (fin * mfi) / (pd + 1));
    printk("rounded (fin * mfn) / ((pd + 1) * (mfd + 1)) = %d\n",
           ((long long)fin * mfn) / ((pd + 1) * (mfd + 1)));
#endif

    fpll = 2 * ( ((fin * mfi  + (pd + 1) / 2) / (pd + 1)) +
                 (((long long)fin * mfn + ((pd + 1) * (mfd + 1)) / 2) / 
		 ((pd + 1) * (mfd + 1))) );

    /* calculate the output of the PERCLK1 divider */
    reg = MC9328MXL_PLL_PCDR;
    perclk1 = fpll / (1 + ((reg & MC9328MXL_PLL_PCDR_PCLK1_MASK) >>
                           MC9328MXL_PLL_PCDR_PCLK1_SHIFT));

    return perclk1;
}

/*
 *  By making this a weak alias for bsp_start_default, a brave soul
 *  can override the actual bsp_start routine used.
 */

void bsp_start (void) __attribute__ ((weak, alias("bsp_start_default")));


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

    _CPU_ISR_Disable(level);

    printk("\n\rI should reset here.\n\r");
    while(1);
}
