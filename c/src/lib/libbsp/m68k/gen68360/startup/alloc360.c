/*
 * MC68360 buffer descriptor allocation routines
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/m68k/m68360.h>
#include <rtems/error.h>

/*
 * Allocation order:
 *	- Dual-Port RAM section 1
 *	- Dual-Port RAM section 3
 *	- Dual-Port RAM section 0
 *	- Dual-Port RAM section 2
 */
static struct {
	uint8_t        *base;
	uint32_t	size;
	uint32_t	used;
} bdregions[] = {
	{ (uint8_t *)&m360.dpram1[0],	sizeof m360.dpram1,	0 },
	{ (uint8_t *)&m360.dpram3[0],	sizeof m360.dpram3,	0 },
	{ (uint8_t *)&m360.dpram0[0],	sizeof m360.dpram0,	0 },
	{ (uint8_t *)&m360.dpram2[0],	sizeof m360.dpram2,	0 },
};

/*
 * Send a command to the CPM RISC processer
 */
void *
M360AllocateBufferDescriptors (int count)
{
	unsigned int i;
	ISR_Level level;
	void *bdp = NULL;
	unsigned int want = count * sizeof(m360BufferDescriptor_t);

	/*
	 * Running with interrupts disabled is usually considered bad
	 * form, but this routine is probably being run as part of an
	 * initialization sequence so the effect shouldn't be too severe.
	 */
	_ISR_Disable (level);
	for (i = 0 ; i < sizeof(bdregions) / sizeof(bdregions[0]) ; i++) {
		/*
		 * Verify that the region exists.
		 * This test is necessary since some chips have
		 * less dual-port RAM.
		 */
		if (bdregions[i].used == 0) {
			volatile uint8_t *cp = bdregions[i].base;
			*cp = 0xAA;
			if (*cp != 0xAA) {
				bdregions[i].used = bdregions[i].size;
				continue;
			}
			*cp = 0x55;
			if (*cp != 0x55) {
				bdregions[i].used = bdregions[i].size;
				continue;
			}
			*cp = 0x0;
		}
		if (bdregions[i].size - bdregions[i].used >= want) {
			bdp = bdregions[i].base + bdregions[i].used;
			bdregions[i].used += want;
			break;
		}
	}
	_ISR_Enable (level);
	if (bdp == NULL)
		rtems_panic ("Can't allocate %d buffer descriptor(s).\n", count);
	return bdp;
}

void *
M360AllocateRiscTimers (int count)
{
	/*
	 * Convert the count to the number of buffer descriptors
	 * of equal or larger size.  This ensures that all buffer
	 * descriptors are allocated with appropriate alignment.
	 */
	return M360AllocateBufferDescriptors (((count * 4) +
					sizeof(m360BufferDescriptor_t) - 1) /
					sizeof(m360BufferDescriptor_t));
}
