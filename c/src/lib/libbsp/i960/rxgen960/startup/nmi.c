/*
 *  $Id$
 */

int mach_error_expected = 0;
void nmi_isr(void)
{
	if( mach_error_expected)
	{
	 	mach_error_expected = 0;
	}
	else{
		kkprintf("NMI Interrupt Occured \n");
	}
}
