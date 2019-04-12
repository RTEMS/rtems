/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup can
 * @brief Common CAN baud-rate routines for OCCAN/GRCAN/GRCANFD controllers
 */

/*
 * Copyright (C) 2019, 2020 Cobham Gaisler AB
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

#ifndef __GRLIB_CANBTRS_H__
#define __GRLIB_CANBTRS_H__

/**
 * @defgroup can CAN
 *
 * @ingroup RTEMSBSPsSharedGRLIB
 *
 * @brief CAN routines shared between OCCAN, GRCAN and GRCANFD controllers
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/* CAN Baud-rate parameters, range of valid parameter values */
struct grlib_canbtrs_ranges {
	unsigned int max_scaler;
	char has_bpr;
	unsigned char divfactor;
	unsigned char min_tseg1;
	unsigned char max_tseg1;
	unsigned char min_tseg2;
	unsigned char max_tseg2;
};

struct grlib_canbtrs_timing {
	unsigned char scaler;
	unsigned char ps1;
	unsigned char ps2;
	unsigned char rsj;
	unsigned char bpr;
};

/* @brief Calculate CAN baud-rate generation parameters from requested baud-rate
 *
 * @param baud            The CAN baud rate requested
 * @param core_hz         Input clock [Hz] to CAN core
 * @param sampl_pt        CAN sample point in %, 80 means 80%
 * @param br              CAN Baud-rate parameters limitations
 * @param[out] timing     result is placed here
 *
 * @retval 0 Baud-rate parameters sucessfully calculated
 * @retval negative Failure to generate parameters with less than 5% error
 *   margin from requested baud-rate
 */
int grlib_canbtrs_calc_timing(
	unsigned int baud,
	unsigned int core_hz,
	unsigned int sampl_pt,
	struct grlib_canbtrs_ranges *br,
	struct grlib_canbtrs_timing *timing
	);

#ifdef __cplusplus
}
#endif

#endif
