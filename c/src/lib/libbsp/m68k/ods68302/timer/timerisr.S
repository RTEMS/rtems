/*
 * Handle 68302 TIMER2 interrupts.
 *
 * All code in this routine is pure overhead which can perturb the
 * accuracy of RTEMS' timing test suite.
 *
 * See also:	benchmark_timer_read()
 *
 * To reduce overhead this is best to be the "rawest" hardware interupt
 * handler you can write.  This should be the only interrupt which can
 * occur during the measured time period.
 *
 * An external counter, Timer_interrupts, is incremented.
 */

#include <rtems/asm.h>

BEGIN_CODE
	PUBLIC(timerisr)
SYM(timerisr):
	move.w	#0x0040,SYM(m302)+2072	| clear interrupt in-service bit
	move.b	#3,SYM(m302)+2137	| clear timer interrupt event register
	addq.l	#1,SYM(Timer_interrupts) | increment timer value
	rte
END_CODE
END
