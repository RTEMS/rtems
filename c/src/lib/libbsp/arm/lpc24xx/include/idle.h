/**
 * @file
 *
 * @author Sebastian Huber <sebastian.huber@embedded-brains.de>
 *
 * @ingroup lpc24xx
 *
 * @brief Idle task
 */

/*
 * Copyright (c) 2009
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LPC24XX_IDLE_H
#define LIBBSP_ARM_LPC24XX_IDLE_H

#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

Thread lpc24xx_idle( uint32_t ignored);

#define BSP_IDLE_TASK_BODY lpc24xx_idle

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC24XX_IDLE_H */
