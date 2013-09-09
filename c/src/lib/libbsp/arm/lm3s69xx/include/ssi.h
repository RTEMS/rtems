/*
 * Copyright � 2013 Eugeniy Meshcheryakov <eugen@debian.org>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */
#ifndef LIBBSP_ARM_LM3S69XX_SSI_H
#define LIBBSP_ARM_LM3S69XX_SSI_H
#include <rtems/libi2c.h>
#include <bspopts.h>

#ifdef __cplusplus
extern "C" {
#endif

extern rtems_libi2c_bus_t * const lm3s69xx_ssi_0;

#if LM3S69XX_NUM_SSI_BLOCKS > 1
extern rtems_libi2c_bus_t * const lm3s69xx_ssi_1;
#endif

#ifdef __cplusplus
}
#endif

#endif /* LIBBSP_ARM_LM3S69XX_SSI_H */
