/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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
