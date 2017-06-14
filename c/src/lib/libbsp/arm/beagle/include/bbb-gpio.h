/**
 * @file
 *
 * @ingroup arm_beagle
 *
 * @brief BeagleBone Black BSP definitions.
 */

/**
 * Copyright (c) 2015 Ketul Shah <ketulshah1993 at gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_BEAGLE_BBB_GPIO_H
#define LIBBSP_ARM_BEAGLE_BBB_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief  BeagleBone Black GPIO functions.
 */
#define BBB_DIGITAL_IN  2
#define BBB_DIGITAL_OUT 1

/**
 * @brief  BeagleBone Black GPIO pad configuration.
 */
#define BBB_PUDEN (1 << 3)
#define BBB_PUDDIS ~BBB_PUDEN
#define BBB_PU_EN (1 << 4)
#define BBB_PD_EN ~BBB_PU_EN
#define BBB_MUXMODE(X) (X & 0x7)
#define BBB_RXACTIVE (1 << 5)
#define BBB_SLEWCTRL (1 << 6)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_BEAGLE_BBB_GPIO_H */
