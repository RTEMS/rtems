/*
 *  $Id$
 */

#include <iregdef.h>
#include <idtmon.h>
#include <idtcpu.h>


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

