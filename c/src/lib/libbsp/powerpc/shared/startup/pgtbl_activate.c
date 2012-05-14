#include <rtems.h>
#include <libcpu/pte121.h>
#include <libcpu/bat.h>

/* Default activation of the page tables. This is a weak
 * alias, so applications may easily override this
 * default activation procedure.
 */

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 4/2002,
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

void
BSP_pgtbl_activate(Triv121PgTbl) __attribute__ (( weak, alias("__BSP_default_pgtbl_activate") ));

void
__BSP_default_pgtbl_activate(Triv121PgTbl pt)
{
	if (!pt) return;

	/* switch the text/ro sements to RO only after
	 * initializing the interrupts because the irq_mng
	 * installs some code...
	 *
	 * activate the page table; it is still masked by the
	 * DBAT0, however
	 */
	triv121PgTblActivate(pt);

	/* finally, switch off DBAT0 */
	setdbat(0,0,0,0,0);
	/* At this point, DBAT0 is available for other use... */
}
