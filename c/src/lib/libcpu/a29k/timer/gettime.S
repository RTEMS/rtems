/* @(#)gettime.S       03/26/96     1.1 */

#include "iregdef.h"
#include "idtcpu.h"
#include "idtmon.h"

FRAME(mips_read_timer,sp,0,ra)
	.set noreorder
        mfc0 v0,C0_COUNT
        nop
        j ra
	.set reorder
ENDFRAME(mips_read_timer)
