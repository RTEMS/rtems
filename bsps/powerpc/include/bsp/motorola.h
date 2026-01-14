/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief Motorola (MVME) board identification
 *
 *  This include file describe the data structure and the functions implemented
 *  by rtems to identify motorola boards.
 */

/*
 * Copyright (c) 1999 Eric Valette <eric.valette@free.fr>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LIBBSP_POWERPC_SHARED_MOTOROLA_MOTOROLA_H
#define LIBBSP_POWERPC_SHARED_MOTOROLA_MOTOROLA_H

#include <bsp/residual.h>
#include <bsp/pci.h>

typedef enum {
  PREP_IBM 	= 0,
  PREP_Radstone = 1,
  PREP_Motorola = 2
} prep_t;

typedef enum {
  MVME_2400 			= 0,
  MVME_2400_750			= 1,
  GENESIS   			= 2,
  POWERSTACK_E			= 3,
  BLACKAWK			= 4,
  OMAHA				= 5,
  UTAH				= 6,
  POWERSTACK_EX			= 7,
  MESQUITE			= 8,
  SITKA				= 9,
  MESQUITE_W_HAC		= 10,
  MTX_PLUS			= 11,
  MTX_WO_PP			= 12,
  MTX_W_PP			= 13,
  MVME_2300			= 14,
  MVME_2300SC_2600		= 15,
  MVME_2600_W_MVME712M		= 16,
  MVME_2600_2700_W_MVME761	= 17,
  MVME_3600_W_MVME712M		= 18,
  MVME_3600_W_MVME761		= 19,
  MVME_1600			= 20,
  /* In the table, slot 21 is the marker for end of automatic probe and scan */
  MVME_2100			= 22,
  MOTOROLA_UNKNOWN		= 255
} motorolaBoard;

typedef enum {
  HOST_BRIDGE_RAVEN	= 0,
  HOST_BRIDGE_HAWK	= 1,
  HOST_BRIDGE_UNKNOWN	= 255
} motorolaHostBridge;

#define MOTOROLA_CPUTYPE_REG	0x800
#define MOTOROLA_BASETYPE_REG	0x803

extern prep_t 			checkPrepBoardType(RESIDUAL *res);
extern prep_t 			currentPrepType;
extern motorolaBoard		getMotorolaBoard(void);
extern motorolaBoard		currentBoard;
extern const char*		motorolaBoardToString(motorolaBoard);
extern const struct _int_map    *motorolaIntMap(motorolaBoard board);
extern const void               *motorolaIntSwizzle(motorolaBoard board);

#endif /* LIBBSP_POWERPC_SHARED_MOTOROLA_MOTOROLA_H */
