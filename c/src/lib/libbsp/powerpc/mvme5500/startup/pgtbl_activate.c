#include <rtems.h>
#include <libcpu/pte121.h>
#include <libcpu/bat.h>

/* Default activation of the page tables. This is a weak
 * alias, so applications may easily override this
 * default activation procedure.
 */

/* Author: Till Straumann, <strauman@slac.stanford.edu>, 4/2002
 * Kate Feng <feng1@bnl.gov> ported it to MVME5500, 4/2004
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

	/* finally, switch off DBAT0 & DBAT1 */
	setdbat(0,0,0,0,0);
	setdbat(1,0,0,0,0);  /* <skf> */
	/* At this point, DBAT0 is available for other use... */
}
