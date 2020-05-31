/**
 * @file
 *
 * @ingroup RTEMSBSPsI386
 *
 * @brief Sizes of Global and Interrupt descriptor tables.
 */

/*
 * This header file is also used in assembler modules.
 *
 * Copyright (C) 2014  Jan Doležal (dolezj21@fel.cvut.cz)
 *                     CTU in Prague.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bspopts.h>

#define IDT_SIZE (256)
/* We have 3 fixed segments (NULL, text, data) + a GS segment for TLS */
#ifdef RTEMS_SMP
/* Need one GS segment for each processor (x86 can have up to 256 processors) */
#define NUM_SYSTEM_GDT_DESCRIPTORS 3+256
#else
#define NUM_SYSTEM_GDT_DESCRIPTORS 3+1
#endif
#define GDT_SIZE (NUM_SYSTEM_GDT_DESCRIPTORS + NUM_APP_DRV_GDT_DESCRIPTORS)
