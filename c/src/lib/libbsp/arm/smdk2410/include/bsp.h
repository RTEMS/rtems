/**
 * @file
 * @ingroup arm_smdk2410
 * @brief Global BSP definitions.
 */

/*-------------------------------------------------------------------------+
| bsp.h - ARM BSP
|
| Copyright (c) Ray,Xu  mailto:ray.cn AT gmail dot com
|
|  The license and distribution terms for this file may be
|  found in the file LICENSE in this distribution or at
|  http://www.rtems.org/license/LICENSE.
+--------------------------------------------------------------------------*/


#ifndef LIBBSP_ARM_SMDK2410_BSP_H
#define LIBBSP_ARM_SMDK2410_BSP_H

#include <bspopts.h>
#include <bsp/default-initial-extension.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup arm_smdk2410 SMDK2410 Support
 * @ingroup bsp_arm
 * @brief SMDK2410 Support Package
 */

/**
 *  This file will not be pre-installed because the smdk2410 BSP uses
 *  the bsp.h in gp32 currently.  This file is a placeholder. If you
 *  need to add something specical for your 2410 BSP please override
 *  this file with your own and change Makefile.am
 */

#ifdef __cplusplus
}
#endif

#endif /* _BSP_H */

