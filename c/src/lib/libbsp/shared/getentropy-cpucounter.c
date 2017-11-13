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

/*
 * ATTENTION: THIS IS A VERY LIMITED ENTROPY SOURCE.
 *
 * This implementation uses a time-based value for it's entropy. The only thing
 * that makes it random are interrupts from external sources. Don't use it if
 * you need for example a strong crypto.
 */

#include <unistd.h>
#include <string.h>
#include <rtems/sysinit.h>
#include <rtems/counter.h>

int getentropy(void *ptr, size_t n)
{
	uint8_t *dest = ptr;

	while (n > 0) {
		rtems_counter_ticks ticks;

		ticks = rtems_counter_read();

		if (n >= sizeof(ticks)) {
			memcpy(dest, &ticks, sizeof(ticks));
			n -= sizeof(ticks);
			dest += sizeof(ticks);
		} else {
			/*
			 * Fill the remaining bytes with only the least
			 * significant byte of the time. That is the byte with
			 * the most changes.
			 */
			*dest = ticks & 0xFF;
			--n;
			++dest;
		}
	}

	return 0;
}
