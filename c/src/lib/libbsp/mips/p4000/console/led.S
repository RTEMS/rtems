/*
 *  $Id$
 */

#include <rtems/mips/iregdef.h>
#include <rtems/mips/idtcpu.h>
#include <rtems/asm.h>

FRAME(mips_leddisplay,sp,0,ra)
	j ra
	nop
ENDFRAME(mips_leddisplay)

FRAME(sbddisplay,sp,0,ra)
	j	ra
ENDFRAME(sbddisplay)

FRAME(sbdblank,sp,0,ra)
	j	ra
ENDFRAME(sbdblank)
