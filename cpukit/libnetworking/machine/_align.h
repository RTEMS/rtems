/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#ifndef _MACHINE_INCLUDE__ALIGN_H_
#define	_MACHINE_INCLUDE__ALIGN_H_

#include <sys/_types.h>

#define	_ALIGNBYTES	(sizeof(long long) - 1)
#define	_ALIGN(_p)	(((__uintptr_t)(_p) + _ALIGNBYTES) & ~_ALIGNBYTES)

#endif /* !_MACHINE_INCLUDE__ALIGN_H_ */
