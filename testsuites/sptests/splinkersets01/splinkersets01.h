/*
 * Copyright (c) 2015, 2016 embedded brains GmbH.  All rights reserved.
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

RTEMS_LINKER_RWSET_DECLARE(test_content_rw, char);

RTEMS_LINKER_ROSET_DECLARE(test_content_ro, char);

RTEMS_LINKER_RWSET_ITEM_DECLARE(test_rw, const int *, a1);

RTEMS_LINKER_ROSET_ITEM_DECLARE(test_ro, const int *, ca2);

extern int content_rw_1;
extern char content_rw_2;
extern char content_rw_3;

extern const int content_ro_1;
extern const char content_ro_2;
extern const char content_ro_3;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SPLINKERSET01_H */
