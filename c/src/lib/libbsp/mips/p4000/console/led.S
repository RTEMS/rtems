/*
 *  $Id$
 */

#include <iregdef.h>
#include <idtcpu.h>
#include <asm.h>


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

