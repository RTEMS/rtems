/*
 * (c) 1998, Radstone Technology plc.
 *
 *
 * This is an unpublished work the copyright  in  which   vests
 * in  Radstone Technology plc.  All rights reserved.
 *
 * The information contained herein is the property of Radstone
 * Technology  plc.  and  is  supplied  without  liability  for
 * errors or omissions and no part may be reproduced,  used  or
 * disclosed except as authorized by contract or  other written
 * permission.     The    copyright    and    the     foregoing
 * restriction   on  reproduction, use and disclosure extend to
 * all  the   media   in   which   this  information   may   be
 * embodied.
 *
 */
#include <rtems/score/targopts.h>
#include "ppc-asm.h"

	.file	"bsptrap.s"
	.text
FUNC_START(bsp_trap)
	sc
FUNC_END(bsp_trap)
