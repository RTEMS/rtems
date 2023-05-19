/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
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

#include <libchip/chip.h>
#include <unistd.h>
#include <string.h>
#include <rtems/sysinit.h>

static void atsam_trng_enable(void)
{
	PMC_EnablePeripheral(ID_TRNG);
	TRNG_Enable();
}

int getentropy(void *ptr, size_t n)
{
	while (n > 0) {
		uint32_t random;
		size_t copy;

		while ((TRNG_GetStatus() & TRNG_ISR_DATRDY) == 0) {
			/* wait */
		}
		random = TRNG_GetRandData();

		/*
		 * Read TRNG status one more time to avoid race condition.
		 * Otherwise we can read (and clear) an old ready status but get
		 * a new value. The ready status for this value wouldn't be
		 * reset.
		 */
		TRNG_GetStatus();

		copy = sizeof(random);
		if (n < copy ) {
			copy = n;
		}
		memcpy(ptr, &random, copy);
		n -= copy;
		ptr += copy;
	}

	return 0;
}

RTEMS_SYSINIT_ITEM(
  atsam_trng_enable,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_LAST_BUT_5
);
