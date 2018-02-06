/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
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

#ifndef ATSAM_IOCOPY_H
#define ATSAM_IOCOPY_H

#include <rtems.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Note: This functions are for copying from or to memory that is marked as
 * Peripheral memory. In this regions a misaligned access is not allowed.
 * Therefore memcopy would not work in all cases.
 */
void atsam_copy_to_io(void *dst, const void *src, size_t n);
void atsam_copy_from_io(void *dst, const void *src, size_t n);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ATSAM_IOCOPY_H */
