/**
 * @file
 *
 * @ingroup lm3s69xx_syscon
 *
 * @brief Syscon support.
 */

/*
 * Copyright © 2013 Eugeniy Meshcheryakov <eugen@debian.org>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */
#ifndef LIBBSP_ARM_LM3S69XX_SYSCON_H
#define LIBBSP_ARM_LM3S69XX_SYSCON_H
#include <stdbool.h>

/**
 * @defgroup lm3s69xx_syscon Syscon Support
 *
 * @ingroup RTEMSBSPsARMLM3S69XX
 *
 * @brief Syscon Support
 */

#ifdef __cplusplus
extern "C" {
#endif

void lm3s69xx_syscon_enable_gpio_clock(unsigned int port, bool enable);
void lm3s69xx_syscon_enable_uart_clock(unsigned int port, bool enable);
void lm3s69xx_syscon_enable_ssi_clock(unsigned int port, bool enable);
void lm3s69xx_syscon_enable_pwm_clock(bool enable);
void lm3s69xx_syscon_set_pwmdiv(unsigned int div);
void lm3s69xx_syscon_delay_3x_clocks(unsigned long x_count);

#ifdef __cplusplus
}
#endif

#endif /* LIBBSP_ARM_LM3S69XX_SYSCON_H */
