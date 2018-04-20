/*
 * (c) 2007, Thomas Doerfler <Thomas.Doerfler@embedded-brains.de>
 *
 *
 *  This file contains the entry point vector needed by some bootloaders
 *  derived from "vectors.S"
 */

#include <rtems/asm.h>
#include <rtems/score/cpu.h>

	PUBLIC_VAR (__rtems_start)
	.section .entry_point_section,"awx",@progbits
/*
 * Entry point information used by bootloader code
 */
SYM (__rtems_start):
	.long   __rtems_entry_point

	/*
	 * end of special Entry point section
	 */
