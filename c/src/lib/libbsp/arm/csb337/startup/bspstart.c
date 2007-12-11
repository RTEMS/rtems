/*
 * Cogent CSB337 - AT91RM9200 Startup code
 *
 * Copyright (c) 2004 by Cogent Computer Systems
 * Written by Jay Monkman <jtm@lopingdog.com>
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
#include <at91rm9200.h>
#include <at91rm9200_pmc.h>
#include <at91rm9200_emac.h>

/* Global Variables */
extern void            *_flash_size;
extern void            *_flash_base;
extern void            *_sdram_size;
extern void            *_sdram_base;
extern void            *_bss_free_start;

unsigned long           free_mem_start;
unsigned long           free_mem_end;

/* Function prototypes */
extern void rtems_irq_mngt_init(void);
void bsp_libc_init( void *, uint32_t, int );
void bsp_postdriver_hook(void);
static void fix_mac_addr();

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
void bsp_start_default( void )
{
    /* disable interrupts */
    AIC_CTL_REG(AIC_IDCR) = 0xffffffff;

    /* 
     * Some versions of the bootloader have the MAC address
     * reversed. This fixes it, if necessary.
     */
    fix_mac_addr();

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
    printk( "work_space_size = 0x%x\n\r", 
            rtems_configuration_get_work_space_size() );
    printk( "microseconds_per_tick = 0x%x\n\r",
            rtems_configuration_get_microseconds_per_tick() );
    printk( "ticks_per_timeslice = 0x%x\n\r",
            rtems_configuration_get_ticks_per_timeslice() );
    printk( "work_space_size = 0x%x\n\r", 
            rtems_configuration_get_work_space_size() );
#endif
} /* bsp_start */

/* 
 * Some versions of the bootloader shipped with the CSB337
 * reverse the MAC address. This function tests for that,
 * and fixes the MAC address.
 */
static void fix_mac_addr()
{
    uint8_t addr[6];

    /* Read the MAC address */
    addr[0] = (EMAC_REG(EMAC_SA1L) >>  0) & 0xff;
    addr[1] = (EMAC_REG(EMAC_SA1L) >>  8) & 0xff;
    addr[2] = (EMAC_REG(EMAC_SA1L) >> 16) & 0xff;
    addr[3] = (EMAC_REG(EMAC_SA1L) >> 24) & 0xff;
    addr[4] = (EMAC_REG(EMAC_SA1H) >>  0) & 0xff;
    addr[5] = (EMAC_REG(EMAC_SA1H) >>  8) & 0xff;

    /* Check which 3 bytes have Cogent's OUI */
    if ((addr[5] == 0x00) && (addr[4] == 0x23) && (addr[3] == 0x31)) {
        EMAC_REG(EMAC_SA1L) = ((addr[5] <<  0) |
                               (addr[4] <<  8) |
                               (addr[3] << 16) |
                               (addr[2] << 24));

        EMAC_REG(EMAC_SA1H) = ((addr[1] <<  0) |
                               (addr[0] <<  8));
    }
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
void bsp_reset(void)
{
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);

    /* Enable the watchdog timer, then wait for the world to end. */
    ST_REG(ST_WDMR) = ST_WDMR_RSTEN | 1;

    while(1);
}
