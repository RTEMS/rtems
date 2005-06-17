/*
 *  This is where the real hardware setup is done. A minimal stack
 *  has been provided by the start.S code. No normal C or RTEMS
 *  functions can be called from here.
 * 
 * This routine is pretty simple for the uC5235 because all the hard
 * work has been done by the bootstrap dBUG code.
 */

#include <rtems.h>
#include <bsp.h>
#define m68k_set_cacr(_cacr) asm volatile ("movec %0,%%cacr" : : "d" (_cacr))
#define m68k_set_acr0(_acr0) asm volatile ("movec %0,%%acr0" : : "d" (_acr0))
#define m68k_set_acr1(_acr1) asm volatile ("movec %0,%%acr1" : : "d" (_acr1))
#define MM_SDRAM_BASE		(0x00000000)

void Init5235 (void)
{
    extern void CopyDataClearBSSAndStart (void);
    int x;
    int temp = 0;
    
    //Setup the GPIO Registers
    MCF5235_GPIO_UART=0x3FFF;
    MCF5235_GPIO_PAR_AD=0xE1;
    
    //Setup the Chip Selects so CS0 is flash
    MCF5235_CS_CSAR0 =(0xFFE00000 & 0xffff0000)>>16;
    MCF5235_CS_CSMR0 = 0x001f0001;
    MCF5235_CS_CSCR0 = 0x1980;
    
    //Setup the SDRAM 
    for(x=0; x<20000; x++)
    {
	temp +=1;
    }
    MCF5235_SDRAMC_DCR  = 0x042E;
    MCF5235_SDRAMC_DACR0 = 0x00001300;
    MCF5235_SDRAMC_DMR0 = (0x00FC0000) | (0x00000001);
    for(x=0; x<20000; x++)
    {
	temp +=1;
    }
    // set ip ( bit 3 ) in dacr
    MCF5235_SDRAMC_DACR0 |= (0x00000008) ; 
    // init precharge
    *((unsigned long *)MM_SDRAM_BASE) = 0xDEADBEEF;
    // set RE in dacr
    MCF5235_SDRAMC_DACR0 |= (0x00008000);  
    // wait
    for(x=0; x<20000; x++)
    {
	temp +=1;
    }
    // issue IMRS
    MCF5235_SDRAMC_DACR0 |= (0x00000040);
    *((short *)MM_SDRAM_BASE) = 0;
    for(x=0; x<60000; x++)
    { 
	temp +=1;
    }
    *((unsigned long*)MM_SDRAM_BASE)=0x12345678;
	
    /* Copy the interrupt vector table to address 0x0 in SDRAM */
    {
        extern void INTERRUPT_VECTOR();
        uint32_t *inttab = (uint32_t *)&INTERRUPT_VECTOR;
        uint32_t *intvec = (uint32_t *)0x0;
        register int i;
        for (i = 0; i < 256; i++)
        {
            *(intvec++) = *(inttab++);
        }
    }

     /*
     * Copy data, clear BSS and call boot_card()
     */
    CopyDataClearBSSAndStart ();
}
