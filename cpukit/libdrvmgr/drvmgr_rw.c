/* Driver Manager Read/Write Interface Implementation.
 *
 * COPYRIGHT (c) 2009 Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <string.h>
#include <drvmgr/drvmgr.h>

/* Set a range of memory in 128 byte chunks.
 * This call will take 128 bytes for buffer on stack
 */
void drvmgr_rw_memset(
	void *dstadr,
	int c,
	size_t n,
	void *a,
	drvmgr_wmem_arg wmem
	)
{
	unsigned long long buf[16+1]; /* Extra bytes after data are reserved
				       * for optimizations by write_mem */
	int txlen;
	char *adr;

	if (n <= 0)
		return;

	if (n > sizeof(unsigned long long)*16)
		txlen = sizeof(unsigned long long)*16;
	else
		txlen = n;

	memset(buf, c, txlen);

	adr = dstadr;
	do {
		wmem(adr, (const void *)&buf[0], txlen, a);
		adr += txlen;
		n -= txlen;

		/* next length to transmitt */
		if (n > 16*sizeof(unsigned long long))
			txlen = 16*sizeof(unsigned long long);
		else
			txlen = n;
	} while (n > 0);
}
