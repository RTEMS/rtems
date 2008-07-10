/**
 * @file
 *
 * @ingroup powerpc_shared
 *
 * @brief Source file for tic-tac code.
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

#include <stdint.h>

/**
 * @brief Referenc value for tac().
 */
uint32_t ppc_tic_tac = 0;

/**
 * @brief Referenc value for bam().
 */
uint32_t ppc_boom_bam = 0;
