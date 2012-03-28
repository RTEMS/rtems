/*
 *  Early dynamic memory allocation (not freeable) for BSP
 *  boot routines. Minimum alignment 8 bytes. Memory is
 *  allocated after _end, it will shrink the workspace.
 *
 *  COPYRIGHT (c) 2011.
 *  Aeroflex Gaisler AB
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <stdlib.h>

/* Tells us where to put the workspace in case remote debugger is present. */
extern uint32_t rdb_start;

/* Must be aligned to 8 */
extern unsigned int early_mem;

/* must be identical to STACK_SIZE in start.S */
#define STACK_SIZE (16 * 1024)

/* Allocate 8-byte aligned non-freeable pre-malloc() memory. The function
 * can be called at any time. The work-area will shrink when called before
 * bsp_get_work_area(). malloc() is called to get memory when this function
 * is called after bsp_get_work_area().
 */
void *bsp_early_malloc(int size)
{
	void *start;

	/* Not early anymore? */
	if (early_mem == ~0)
		return malloc(size);

	size = (size + 7) & ~0x7;
	if (rdb_start - STACK_SIZE - early_mem < size)
		return NULL;

	start = (void *)early_mem;
	early_mem += size;

	return start;
}
