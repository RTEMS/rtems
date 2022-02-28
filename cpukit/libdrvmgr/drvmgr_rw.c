/* SPDX-License-Identifier: BSD-2-Clause */

/* Driver Manager Read/Write Interface Implementation.
 *
 * COPYRIGHT (c) 2009 Cobham Gaisler AB.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
