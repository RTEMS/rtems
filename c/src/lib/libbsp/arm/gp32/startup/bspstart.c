/*-------------------------------------------------------------------------+
| This file contains the ARM BSP startup package. It includes application,
| board, and monitor specific initialization and configuration. The generic CPU
| dependent initialization has been performed before this routine is invoked. 
+--------------------------------------------------------------------------+
|
| Copyright (c) 2000 Canon Research Centre France SA.
| Emmanuel Raguet, mailto:raguet@crf.canon.fr
|
|   The license and distribution terms for this file may be
|   found in found in the file LICENSE in this distribution or at
|   http://www.rtems.com/license/LICENSE.
|
+--------------------------------------------------------------------------*/


#include <bsp.h>
#include <rtems/libcsupport.h>
#include <rtems/libio.h>
#include <rtems/bspIo.h>
#include <s3c2400.h>

/*-------------------------------------------------------------------------+
| Global Variables
+--------------------------------------------------------------------------*/

extern void            *_sdram_size;
extern void            *_sdram_base;
extern void            *_bss_free_start;

unsigned long           free_mem_start;
unsigned long           free_mem_end;

/* The original BSP configuration table from the application and our copy of it
   with some changes. */

extern rtems_configuration_table  Configuration;
       rtems_configuration_table  BSP_Configuration;

rtems_cpu_table Cpu_table;                     /* CPU configuration table.    */
char            *rtems_progname = "RTEMS";               /* Program name - from main(). */

/*-------------------------------------------------------------------------+
| External Prototypes
+--------------------------------------------------------------------------*/
extern void rtems_irq_mngt_init(void);
void bsp_libc_init( void *, uint32_t, int );
void bsp_postdriver_hook(void);

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

} /* bsp_pretasking_hook */

void bsp_idle_task(void)
{
	while(1){
		asm volatile ("MCR p15,0,r0,c7,c0,4     \n");
	}
}
 
/*-------------------------------------------------------------------------+
|         Function: bsp_start
|      Description: Called before main is invoked.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing. 
+--------------------------------------------------------------------------*/
void bsp_start_default( void )
{
    uint32_t cr;
    uint32_t pend,last;
    uint32_t REFCNT;
    int i;
    /* If we don't have command line arguments set default program name. */

    Cpu_table.pretasking_hook      = bsp_pretasking_hook; /* init libc, etc. */
    Cpu_table.predriver_hook       = NULL;                /* use system's    */
    Cpu_table.postdriver_hook      = bsp_postdriver_hook;
    Cpu_table.idle_task            = bsp_idle_task;
    Cpu_table.interrupt_stack_size = 4096;
    Cpu_table.extra_mpci_receive_server_stack = 0;

    /* stop RTC */
    rTICINT=0x0;

    /* stop watchdog,ADC and timers */
    rWTCON=0x0;
    rTCON=0x0;
    rADCCON=0x0;

    /* disable interrupts */
    rINTMOD=0x0;
    rINTMSK=BIT_ALLMSK; /* unmasked by drivers */

    last=0;
    for(i=0; i<4; i++) {
	pend=rSRCPND;
	if(pend == 0 || pend == last)
	    break;
	rSRCPND=pend;
	rINTPND=pend;
	last=pend;
    }

    /* setup clocks */
    rCLKDIVN=M_CLKDIVN;
    rMPLLCON=((M_MDIV<<12)+(M_PDIV<<4)+M_SDIV);
    /* setup rREFRESH */
    REFCNT=2048+1-(15.6*get_HCLK()/1000000); /* period=15.6 us, HCLK=66Mhz, (2048+1-15.6*66) */
    rREFRESH=((REFEN<<23)+(TREFMD<<22)+(Trp<<20)+(Trc<<18)+(Tchr<<16)+REFCNT);

    /* set prescaler for timers 2,3,4 to 16(15+1) */
    cr=rTCFG0 & 0xFFFF00FF;
    rTCFG0=(cr | (15<<8));

    /* set prescaler for timers 0,1 to 1(0+1) */
    cr=rTCFG0 & 0xFFFFFF00;
    rTCFG0=(cr | (0<<0));

    /* Place RTEMS workspace at beginning of free memory. */
    BSP_Configuration.work_space_start = (void *)&_bss_free_start;

    free_mem_start = ((uint32_t)&_bss_free_start + BSP_Configuration.work_space_size);
    
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
    printk( "work_space_size = 0x%x\n", BSP_Configuration.work_space_size );
    printk( "maximum_extensions = 0x%x\n", BSP_Configuration.maximum_extensions );
    printk( "microseconds_per_tick = 0x%x\n",
            BSP_Configuration.microseconds_per_tick );
    printk( "ticks_per_timeslice = 0x%x\n",
            BSP_Configuration.ticks_per_timeslice );
    printk( "number_of_device_drivers = 0x%x\n",
            BSP_Configuration.number_of_device_drivers );
    printk( "Device_driver_table = 0x%x\n",
            BSP_Configuration.Device_driver_table );
    
    printk( "_heap_size = 0x%x\n", _heap_size );
    /*  printk( "_stack_size = 0x%x\n", _stack_size );*/
    printk( "rtemsFreeMemStart = 0x%x\n", rtemsFreeMemStart );
    printk( "work_space_start = 0x%x\n", BSP_Configuration.work_space_start );
    printk( "work_space_size = 0x%x\n", BSP_Configuration.work_space_size );
#endif

}

/*
 *  By making this a weak alias for bsp_start_default, a brave soul
 *  can override the actual bsp_start routine used.
 */

void bsp_start (void) __attribute__ ((weak, alias("bsp_start_default")));

void bsp_reset(void)
{
    rtems_interrupt_level level;
    _CPU_ISR_Disable(level);
    printk("bsp_reset.....\n");
        /* disable mmu, invalide i-cache and call swi #4 */
        asm volatile(""
                "mrc    p15,0,r0,c1,c0,0        \n"
                "bic    r0,r0,#1                        \n"
                "mcr    p15,0,r0,c1,c0,0        \n"
                "nop                                            \n"
                "nop                                            \n"
                "nop                                            \n"
                "nop                                            \n"
                "nop                                            \n"
                "mov    r0,#0                                   \n"
                "MCR    p15,0,r0,c7,c5,0                        \n"
                "nop                                            \n"
                "nop                                            \n"
                "nop                                            \n"
                "nop                                            \n"
                "nop                                            \n"
                "swi    #4                                      "
        :
        :
        : "r0"
	);
	/* we should be back in bios now */
}
