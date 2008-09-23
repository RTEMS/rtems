/**
 * @file
 *
 * @brief Utility definitions and functions.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBCPU_SHARED_UTILITY_H
#define LIBCPU_SHARED_UTILITY_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GET_REG_FIELD( reg, mask, shift) \
  (((reg) & (mask)) >> (shift))

#define SET_REG_FIELD( reg, val, mask, shift) \
  (((reg) & ~(mask)) | (((val) << (shift)) & (mask)))

#define REG_FLAG_IS_SET( reg, flag) \
  (((reg) & (flag)) != 0)

#define REG_FLAG_IS_CLEARED( reg, flag) \
  (((reg) & (flag)) == 0)

#define SET_REG_FLAG( reg, flag) \
  ((reg) | (flag))

#define CLEAR_REG_FLAG( reg, flag) \
  ((reg) & ~(flag))

#define SET_REG_FLAGS( reg, flags, mask) \
  (((reg) & ~(mask)) | (flags))

#define CLEAR_REG_FLAGS( reg, flags) \
  ((reg) & ~(flags))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCPU_SHARED_UTILITY_H */
