/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This header file provides interfaces of the parity selftest support.
 */

/*
 * Copyright (C) 2016 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
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

#ifndef LIBBSP_ARM_TMS570_SELFTEST_PARITY_H
#define LIBBSP_ARM_TMS570_SELFTEST_PARITY_H

#include <stdint.h>

/**
 * The magic number used to switch most of the peripherals
 * into parity protection test mode
 */
#define TMS570_SELFTEST_PAR_CR_KEY 0xA

typedef struct tms570_selftest_par_desc tms570_selftest_par_desc_t;

typedef void tms570_selftest_par_fnc_t( const tms570_selftest_par_desc_t *desc );

/**
 * Decriptor specifying registers addresses and values used to test
 * that parity protection is working for given hardware
 * module/peripheral. It is used during initial chip self-tests.
 */
struct tms570_selftest_par_desc {
  unsigned char esm_prim_grp;    /**< ESM primary signalling group number. */
  unsigned char esm_prim_chan;   /**< ESM primary signalling channel number. */
  unsigned char esm_sec_grp;     /**< ESM optional/alternative signalling group. */
  unsigned char esm_sec_chan;    /**< ESM optional/alternative signalling channel. */
  int fail_code;                 /**< Error code reported to
                  bsp_selftest_fail_notification() in the case of the test failure. */
  volatile uint32_t *ram_loc;    /**< Address of memory protected by parity. */
  volatile uint32_t *par_loc;    /**< Address of mapping of parity bits into CPU
                                  * address space. */
  uint32_t par_xor;              /**< Bitmask used to alter parity to cause
                                  * intentional parity failure. */
  volatile uint32_t *par_cr_reg; /**< Address of module parity test control register. */
  uint32_t par_cr_test;          /**< Mask of bit which cause switch to a test mode. */
  volatile uint32_t *par_st_reg; /**< Optional module parity status register which. */
  uint32_t par_st_clear;         /**< Optional value which is written to status register
                                  * to clear error. */
  tms570_selftest_par_fnc_t *partest_fnc; /**< Function which specialized for given kind
                                  * of peripheral/module mechanism testing. */
  volatile void *fnc_data;       /**< Pointer to the base of tested peripheral registers.
                                  * It is required by some test functions (CAN and MibSPI) */
};

extern const tms570_selftest_par_desc_t
  tms570_selftest_par_het1_desc;
extern const tms570_selftest_par_desc_t
  tms570_selftest_par_htu1_desc;
extern const tms570_selftest_par_desc_t
  tms570_selftest_par_het2_desc;
extern const tms570_selftest_par_desc_t
  tms570_selftest_par_htu2_desc;
extern const tms570_selftest_par_desc_t
  tms570_selftest_par_adc1_desc;
extern const tms570_selftest_par_desc_t
  tms570_selftest_par_adc2_desc;
extern const tms570_selftest_par_desc_t
  tms570_selftest_par_can1_desc;
extern const tms570_selftest_par_desc_t
  tms570_selftest_par_can2_desc;
extern const tms570_selftest_par_desc_t
  tms570_selftest_par_can3_desc;
extern const tms570_selftest_par_desc_t
  tms570_selftest_par_vim_desc;
extern const tms570_selftest_par_desc_t
  tms570_selftest_par_dma_desc;
extern const tms570_selftest_par_desc_t
  tms570_selftest_par_spi1_desc;
extern const tms570_selftest_par_desc_t
  tms570_selftest_par_spi3_desc;
extern const tms570_selftest_par_desc_t
  tms570_selftest_par_spi5_desc;

extern const tms570_selftest_par_desc_t *const
tms570_selftest_par_list[];

extern const int tms570_selftest_par_list_size;

void tms570_selftest_par_check_std( const tms570_selftest_par_desc_t *desc );

void tms570_selftest_par_check_can( const tms570_selftest_par_desc_t *desc );

void tms570_selftest_par_check_mibspi( const tms570_selftest_par_desc_t *desc );

void tms570_selftest_par_run(
  const tms570_selftest_par_desc_t *
  const *desc_arr,
  int    desc_cnt
);

#endif /* LIBBSP_ARM_TMS570_SELFTEST_PARITY_H */
