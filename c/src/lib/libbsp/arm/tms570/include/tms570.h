/**
 * @file tms570.h
 *
 * @ingroup tms570
 *
 * @brief Specific register definitions according to tms570 family boards.
 */

/*
 * Copyright (c) 2015 Taller Technologies.
 *
 * @author Martin Galvan <martin.galvan@tallertechnologies.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_TMS570_H
#define LIBBSP_ARM_TMS570_H

#define SYSECR (*(uint32_t *)0xFFFFFFE0u) /* System Exception Control Register */
#define ESMIOFFHR (*(uint32_t *)0xFFFFF528) /* ESM Interrupt Offset High Register */
#define ESMSR1 (*(uint32_t *)0xFFFFF518u) /* ESM Status Register 1 */
#define ESMSR2 (*(uint32_t *)0xFFFFF51Cu) /* ESM Status Register 2 */
#define ESMSR3 (*(uint32_t *)0xFFFFF520u) /* ESM Status Register 3 */
#define ESMSR4 (*(uint32_t *)0xFFFFF558u) /* ESM Status Register 4 */

#define SYSECR_RESET 0x80000u

#endif /* LIBBSP_ARM_TMS570_H */
