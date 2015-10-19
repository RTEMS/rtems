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
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_SHARED_FDT_H
#define LIBBSP_SHARED_FDT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void bsp_fdt_copy(const void *src);

const void *bsp_fdt_get(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_SHARED_FDT_H */
