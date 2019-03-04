/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
 *
 * @brief API definitions of the Watchdog driver for the lpc176x bsp in RTEMS.
 */

/*
 * Copyright (c) 2014 Taller Technologies.
 *
 * @author  Boretto Martin    (martin.boretto@tallertechnologies.com)
 * @author  Diaz Marcos (marcos.diaz@tallertechnologies.com)
 * @author  Lenarduzzi Federico  (federico.lenarduzzi@tallertechnologies.com)
 * @author  Daniel Chicco  (daniel.chicco@tallertechnologies.com)
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LPC176X_WATCHDOG_DEFS_H
#define LIBBSP_ARM_LPC176X_WATCHDOG_DEFS_H

#include <rtems/score/cpu.h>
#include <bsp.h>
#include <bspopts.h>
#include <bsp/utility.h>
#include <bsp/irq.h>
#include <bsp/common-types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LPC176X_WDMOD_BASE 0x40000000U
#define LPC176X_WDFEED_CON 0XAAU
#define LPC176X_WDFEED_CFG 0X55U
#define LPC176X_WD_PRESCALER_DIVISOR 4000000U
#define LPC176X_WWDT_MOD_WDEN BSP_BIT32( 0 )
#define LPC176X_WWDT_MOD_WDRESET BSP_BIT32( 1 )
#define LPC176X_WWDT_MOD_WDTOF BSP_BIT32( 2 )
#define LPC176X_WWDT_MOD_WDINT BSP_BIT32( 3 )
#define LPC176X_WWDT_CLKSEL_WDSEL_IRC 0x0U
#define LPC176X_WWDT_CLKSEL_WDSEL_PCLK 0x1U
#define LPC176X_WWDT_CLKSEL_WDSEL_RTC 0x2U
#define LPC176X_WD_INTERRUPT_VECTOR_NUMBER 0U
#define LPC176X_WDMOD ( *(volatile uint32_t *) ( LPC176X_WDMOD_BASE + 0x00U ) )
#define LPC176X_WDTC ( *(volatile uint32_t *) ( LPC176X_WDMOD_BASE + 0x04U ) )
#define LPC176X_WDFEED ( *(volatile uint32_t *) ( LPC176X_WDMOD_BASE + \
                                                  0x08U ) )
#define LPC176X_WDTV ( *(volatile uint32_t *) ( LPC176X_WDMOD_BASE + 0x0CU ) )
#define LPC176X_WDCLKSEL ( *(volatile uint32_t *) ( LPC176X_WDMOD_BASE + \
                                                    0x10U ) )

/**
 * @brief A function that attends an interruption for a watchdog.
 */
typedef rtems_interrupt_handler lpc176x_wd_isr_funct;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /* LIBBSP_ARM_LPC176X_WATCHDOG_DEFS_H */
