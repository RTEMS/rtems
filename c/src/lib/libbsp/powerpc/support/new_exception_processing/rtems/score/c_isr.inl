RTEMS_INLINE_ROUTINE boolean _ISR_Is_in_progress( void )
{
	register unsigned int isr_nesting_level;
	/*
         * Move from special purpose register 0 (mfspr SPRG0, r3)
	 */
	asm volatile ("mfspr	%0, 272" : "=r" (isr_nesting_level));
	return isr_nesting_level;
}
