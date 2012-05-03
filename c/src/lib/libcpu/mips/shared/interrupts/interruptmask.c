#include <rtems.h>

/*
 *  This function returns a mask value which is used to select the bits
 *  in the processor status register that can be set to enable interrupts.
 *  The mask value should not include the 2 software interrupt enable bits.
 */

uint32_t mips_interrupt_mask( void )
{
	uint32_t interrupt_mask;

#ifdef TX49
	interrupt_mask = 0x00000400;			/* Toshiba TX49 processors have a non-standard interrupt mask */
#else
	interrupt_mask = 0x0000fc00;
#endif

	return(interrupt_mask);
}
