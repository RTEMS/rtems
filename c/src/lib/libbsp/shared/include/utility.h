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

#define GET_FIELD( val, mask, shift) \
  (((val) & (mask)) >> (shift))

#define SET_FIELD( val, field, mask, shift) \
  (((val) & ~(mask)) | (((field) << (shift)) & (mask)))

#define CLEAR_FIELD( val, mask) \
  ((val) & ~(mask))

#define IS_FLAG_SET( val, flag) \
  (((val) & (flag)) != 0)

#define IS_FLAG_CLEARED( val, flag) \
  (((val) & (flag)) == 0)

#define ARE_FLAGS_SET( val, flags) \
  (((val) & (flags)) == (flags))

#define IS_ANY_FLAG_SET( val, flags) \
  (((val) & (flags)) != 0)

#define ARE_FLAGS_CLEARED( val, flags) \
  (((val) & (flags)) == 0)

#define SET_FLAG( val, flag) \
  ((val) | (flag))

#define CLEAR_FLAG( val, flag) \
  ((val) & ~(flag))

#define SET_FLAGS( val, flags) \
  ((val) | (flags))

#define CLEAR_FLAGS( val, flags) \
  ((val) & ~(flags))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCPU_SHARED_UTILITY_H */
