/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  AMBA Plug & Play routines
 *
 *  COPYRIGHT (c) 2011
 *  Aeroflex Gaisler
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

#include <grlib/ambapp.h>

/* AMBAPP helper routine to find a device by index. The function is given to
 * ambapp_for_each, the argument may be NULL (find first device) or a pointer
 * to a index which is downcounted until 0 is reached. If the int-pointer
 * points to a value of:
 *   0  - first device is returned
 *   1  - second device is returned
 *   ...
 *
 * The matching device is returned, which will stop the ambapp_for_each search.
 * If zero is returned from ambapp_for_each no device matching the index was
 * found
 */
int ambapp_find_by_idx(struct ambapp_dev *dev, int index, void *pcount)
{
  int *pi = pcount;

  if (pi) {
    if ((*pi)-- == 0)
      return (int)dev;
    else
      return 0;
  } else {
    /* Satisfied with first matching device, stop search */
    return (int)dev;
  }
}
