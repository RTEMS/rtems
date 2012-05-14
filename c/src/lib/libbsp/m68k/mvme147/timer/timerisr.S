/*  timer_isr()
 *
 *  This routine provides the ISR for the PCC timer on the MVME147
 *  board.   The timer is set up to generate an interrupt at maximum
 *  intervals.
 *
 *  MVME147 port for TNI - Telecom Bretagne
 *  by Dominique LE CAMPION (Dominique.LECAMPION@enst-bretagne.fr)
 *  May 1996
 */

#include <rtems/asm.h>

BEGIN_CODE

.set T1_CONTROL_REGISTER,  0xfffe1018    | timer 1 control register

        PUBLIC (timerisr)
SYM (timerisr):
	orb	#0x80, T1_CONTROL_REGISTER | clear T1 int status bit
        addql   #1, SYM (Ttimer_val)     | increment timer value
end_timerisr:
	rte

END_CODE
END
