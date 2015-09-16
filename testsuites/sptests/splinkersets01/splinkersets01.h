/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef SPLINKERSET01_H
#define SPLINKERSET01_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <rtems/linkersets.h>

extern const int a[4];

extern const int ca[5];

RTEMS_LINKER_RWSET_DECLARE(test_rw, const int *);

RTEMS_LINKER_ROSET_DECLARE(test_ro, const int *);

RTEMS_LINKER_RWSET_ITEM_DECLARE(test_rw, const int *, a1);

RTEMS_LINKER_ROSET_ITEM_DECLARE(test_ro, const int *, ca2);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SPLINKERSET01_H */
