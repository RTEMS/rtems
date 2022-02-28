/* SPDX-License-Identifier: BSD-2-Clause */

/* Driver Manager optional dynamic function interface
 *
 * COPYRIGHT (c) 2011 Cobham Gaisler AB.
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

#include <drvmgr/drvmgr.h>

/* Get Function from Function ID */
int drvmgr_func_get(void *obj, int funcid, void **func)
{
	int objtype;
	struct drvmgr_func *f;

	if (!obj)
		return DRVMGR_FAIL;
	objtype = *(int *)obj;

	if (objtype == DRVMGR_OBJ_BUS)
		f = ((struct drvmgr_bus *)obj)->funcs;
	else if (objtype == DRVMGR_OBJ_DRV)
		f = ((struct drvmgr_drv *)obj)->funcs;
	else
		return DRVMGR_FAIL;

	if (f == NULL)
		return DRVMGR_FAIL;

	while (f->funcid != DRVMGR_FUNCID_NONE) {
		if (f->funcid == funcid) {
			*func = f->func;
			return DRVMGR_OK;
		}
		f++;
	}

	return DRVMGR_FAIL;
}
