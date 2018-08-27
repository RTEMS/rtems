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

#ifndef SPPERCPUDATA01_H
#define SPPERCPUDATA01_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <rtems/score/percpudata.h>

PER_CPU_DATA_ITEM_DECLARE(unsigned int, g);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SPPERCPUDATA01_H */
