/*
 * Copyright (c) 2008 by Matthieu Bucchianeri <mbucchia@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE
 */

#ifndef BSP_NDS_TYPES_H_
#define BSP_NDS_TYPES_H_

#include <stdint.h>

/*
 * volatile types for registers.
 */

typedef volatile char vint8_t;
typedef volatile short int vint16_t;
typedef volatile int vint32_t;

typedef volatile unsigned char vuint8_t;
typedef volatile unsigned short int vuint16_t;
typedef volatile unsigned int vuint32_t;

#endif
