/*
 *  ppc_idle.c
 *
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2010,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */
#include <bsp.h>
#include <stdint.h>

#ifdef BSP_IDLE_TASK_BODY

/* Provide an idle-task body which switches the
 * CPU into power-save mode when idle. Any exception
 * (including an interrupt/external-exception)
 * wakes it up.
 *
 * IIRC - this cannot be used on real hardware due
 *        to errata on many chips which is a real
 *        pity. However, when used under qemu it
 *        saves host-CPU cycles.
 *        While qemu-0.12.4 needed to be patched
 *        (would otherwise hang because an exception
 *        didn't clear MSR_POW) qemu-0.14.1 seems
 *        to work fine.
 */

#include <rtems/powerpc/registers.h>
#include <libcpu/cpuIdent.h>
#include <libcpu/spr.h>

SPR_RW(HID0)

void *
bsp_ppc_idle_task_body(uintptr_t ignored)
{
uint32_t msr;

	switch ( current_ppc_cpu ) {

		case PPC_7400:
		case PPC_7455:
		case PPC_7457:
			/* Must enable NAP mode in HID0 for MSR_POW to work */
			_write_HID0( _read_HID0() | HID0_NAP );
		break;

		default:
		break;
	}

	for ( ;; ) {
		_CPU_MSR_GET(msr);
		msr |= MSR_POW;
		asm volatile(
				"1: sync       \n"
				"	mtmsr %0   \n"
				"   isync      \n"
				"   b 1b       \n"
				::"r"(msr)
				);
	}

	return 0;
}

#endif
