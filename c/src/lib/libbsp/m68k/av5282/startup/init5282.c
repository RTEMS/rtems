/*
 *  This is where the real hardware setup is done. A minimal stack
 *  has been provided by the start.S code. No normal C or RTEMS
 *  functions can be called from here.
 *
 * This routine is pretty simple for the uC5282 because all the hard
 * work has been done by the bootstrap dBUG code.
 */

#include <rtems.h>
#include <bsp.h>
#define m68k_set_cacr(_cacr) __asm__ volatile ("movec %0,%%cacr" : : "d" (_cacr))
#define m68k_set_acr0(_acr0) __asm__ volatile ("movec %0,%%acr0" : : "d" (_acr0))
#define m68k_set_acr1(_acr1) __asm__ volatile ("movec %0,%%acr1" : : "d" (_acr1))
#define MM_SDRAM_BASE		(0x00000000)

/*
 * External methods used by this file
 */
extern void CopyDataClearBSSAndStart (void);
extern void INTERRUPT_VECTOR(void);

void Init5282 (void)
{
    int x;
    int temp = 0;

    /*Setup the GPIO Registers */
    MCF5282_GPIO_PBCDPAR = 0x80;
    MCF5282_GPIO_PEPAR = 0x5100;
    MCF5282_GPIO_PJPAR = 0xFF;
    MCF5282_GPIO_PASPAR =  0x0000;
    MCF5282_GPIO_PEHLPAR = 0xC0;
    MCF5282_GPIO_PUAPAR = 0x0F;
    MCF5282_QADC_DDRQB = 0x07;
    MCF5282_GPTA_GPTDDR = 0x0C;
    MCF5282_GPTA_GPTPORT = 0x4;

    /*Setup the Chip Selects so CS0 is flash */
    MCF5282_CS0_CSAR =(0xff800000 & 0xffff0000)>>16;
    MCF5282_CS0_CSMR = 0x007f0001;
    MCF5282_CS0_CSCR =(((0xf)&0x0F)<<10)|(1<<8)|(0x80);

	/*Setup the SDRAM  */
	for(x=0; x<20000; x++)
	{
		temp +=1;
	}
	MCF5282_SDRAMC_DCR  = 0x00000239;
	MCF5282_SDRAMC_DACR0 = 0x00001320;
	MCF5282_SDRAMC_DMR0 = (0x00FC0000) | (0x00000001);
	for(x=0; x<20000; x++)
	{
		temp +=1;
	}
	/* set ip ( bit 3 ) in dacr */
	MCF5282_SDRAMC_DACR0 |= (0x00000008) ;
	/* init precharge */
	*((short *)MM_SDRAM_BASE) = 0;
	/* set RE in dacr */
	MCF5282_SDRAMC_DACR0 |= (0x00008000);
	/* wait */
	for(x=0; x<20000; x++)
	{
		temp +=1;
	}
	/* issue IMRS */
	MCF5282_SDRAMC_DACR0 |= (0x00000040);
	*((short *)MM_SDRAM_BASE) = 0x0000;
	for(x=0; x<60000; x++)
	{
		temp +=1;
	}
	*((unsigned long*)MM_SDRAM_BASE)=0x12345678;

    /* Copy the interrupt vector table to address 0x0 in SDRAM */
    {
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
