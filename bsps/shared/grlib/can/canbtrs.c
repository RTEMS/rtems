/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup can
 *
 * @brief Common CAN baud-rate routines for OCCAN/GRCAN/GRCANFD controllers
 *
 * Implements common routines for calculating CAN baud-rate parameters from
 * a user provided baud-rate speed.
 */

/*
 * Copyright (C) 2019, 2020 Cobham Gailer AB
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

#include <grlib/canbtrs.h>

/*#define GRLIB_CANBTRS_DEBUG*/

/* Calculate CAN baud-rate generation parameters from requested baud-rate */
int grlib_canbtrs_calc_timing(
	unsigned int baud,
	unsigned int core_hz,
	unsigned int sampl_pt,
	struct grlib_canbtrs_ranges *br,
	struct grlib_canbtrs_timing *timing
	)
{
	int best_error = 2000000000, best_tseg=0, best_scaler=0;
	int tseg=0, tseg1=0, tseg2=0, sc, tmp, error;

	/* Default to 80% sample point */
	if ((sampl_pt < 50) || (sampl_pt > 99))
		sampl_pt = 80;

	/* step though all TSEG1+TSEG2 values possible */
	for (tseg = (br->min_tseg1 + br->min_tseg2);
	     tseg <= (br->max_tseg1 + br->max_tseg2);
	     tseg++) {
		/* calculate scaler */
		tmp = ((br->divfactor + tseg) * baud);
		sc = (core_hz * 2)/ tmp - core_hz / tmp;
		if (sc <= 0 || sc > br->max_scaler)
			continue;
		if (br->has_bpr &&
		    (((sc > 256 * 1) && (sc <= 256 * 2) && (sc & 0x1)) ||
		    ((sc > 256 * 2) && (sc <= 256 * 4) && (sc & 0x3)) ||
		    ((sc > 256 * 4) && (sc <= 256 * 8) && (sc & 0x7))))
			continue;

		error = baud - core_hz / (sc * (br->divfactor + tseg));
#ifdef GRLIB_CANBTRS_DEBUG
		printf("  baud=%d, tseg=%d, sc=%d, error=%d\n",
		       baud, tseg, sc, error);
#endif
		if (error < 0)
			error = -error;

		/* tseg is increasing, so we accept higher tseg with the same
		 * baudrate to get better sampling point.
		 */
		if (error <= best_error) {
			best_error = error;
			best_tseg = tseg;
			best_scaler = sc;
#ifdef GRLIB_CANBTRS_DEBUG
			printf("  ! best baud=%d\n",
			       core_hz/(sc * (br->divfactor + tseg)));
#endif
		}
	}

	/* return an error if 5% off baud-rate */
	if (best_error && (baud / best_error <= 5)) {
		return -2;
	} else if (!timing) {
		return 0; /* nothing to store result in, but a valid bitrate can be calculated */
	}

	tseg2 = (best_tseg + br->divfactor) -
	        ((sampl_pt * (best_tseg + br->divfactor)) / 100);
	if (tseg2 < br->min_tseg2) {
		tseg2 = br->min_tseg2;
	} else if (tseg2 > br->max_tseg2) {
		tseg2 = br->max_tseg2;
	}

	tseg1 = best_tseg - tseg2;
	if (tseg1 > br->max_tseg1) {
		tseg1 = br->max_tseg1;
		tseg2 = best_tseg - tseg1;
	} else if (tseg1 < br->min_tseg1) {
		tseg1 = br->min_tseg1;
		tseg2 = best_tseg - tseg1;
	}

	/* Get scaler and BPR from pseudo SCALER clock */
	if (best_scaler <= 256) {
		timing->scaler = best_scaler - 1;
		timing->bpr = 0;
	} else if (best_scaler <= 256 * 2) {
		timing->scaler = ((best_scaler + 1) >> 1) - 1;
		timing->bpr = 1;
	} else if (best_scaler <= 256 * 4) {
		timing->scaler = ((best_scaler + 1) >> 2) - 1;
		timing->bpr = 2;
	} else {
		timing->scaler = ((best_scaler + 1) >> 3) - 1;
		timing->bpr = 3;
	}

	timing->ps1    = tseg1;
	timing->ps2    = tseg2;
	timing->rsj    = 1;

#ifdef GRLIB_CANBTRS_DEBUG
	printf("  ! result: sc=%d,bpr=%d,ps1=%d,ps2=%d\n", timing->scaler, timing->bpr, timing->ps1, timing->ps2);
#endif

	return 0;
}
