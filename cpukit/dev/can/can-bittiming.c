/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup CANFDStack
 *
 * @brief This source file is part of CAN/CAN FD bus common support. It
 *        implements bit timing calculation.
 *
 * Implementation is based on original LinCAN - Linux CAN bus driver
 * Part of OrtCAN project https://ortcan.sourceforge.net/
 */

/*
 * Copyright (C) 2023-2024 Michal Lenc <michallenc@seznam.cz>
 * Copyright (C) 2002-2009 DCE FEE CTU Prague
 * Copyright (C) 2002-2024 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 * Copyright (C) 2005      Stanislav Marek
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

#include <limits.h>
#include <stdlib.h>

#include <dev/can/can.h>
#include <dev/can/can-devcommon.h>

#define CAN_CALC_MAX_ERROR 50  /* in one-tenth of a percent */
#define CAN_CALC_SYNC_SEG 1

static int can_update_sample_point(
  const struct rtems_can_bittiming_const *btc,
  unsigned int sample_point_nominal,
  unsigned int tseg,
  unsigned int *tseg1_ptr,
  unsigned int *tseg2_ptr,
  unsigned int *sample_point_error_ptr
)
{
  unsigned int sample_point_error;
  unsigned int best_sample_point_error;
  unsigned int sample_point;
  unsigned int best_sample_point;
  unsigned int tseg1;
  unsigned int tseg2;
  int i;

  best_sample_point_error = UINT_MAX;
  sample_point = 0;
  best_sample_point = 0;

  for ( i = 0; i <= 1; i++ ) {
    tseg2 = tseg + CAN_CALC_SYNC_SEG - (sample_point_nominal *
                ( tseg + CAN_CALC_SYNC_SEG ) ) /  1000 - i;

    if ( tseg2 < btc->tseg2_min ) {
      tseg2 = btc->tseg2_min;
    } else if ( tseg2 > btc->tseg2_max ) {
      tseg2 = btc->tseg2_max;
    }

    tseg1 = tseg - tseg2;
    if ( tseg1 > btc->tseg1_max ) {
      tseg1 = btc->tseg1_max;
      tseg2 = tseg - tseg1;
    }

    sample_point = 1000 * ( tseg + CAN_CALC_SYNC_SEG - tseg2 ) /
        ( tseg + CAN_CALC_SYNC_SEG );

    sample_point_error = sample_point_nominal > sample_point ?
                         sample_point_nominal - sample_point :
                         sample_point - sample_point_nominal;

    if ( ( sample_point <= sample_point_nominal ) &&
        ( sample_point_error < best_sample_point_error ) ) {

      best_sample_point = sample_point;
      best_sample_point_error = sample_point_error;
      *tseg1_ptr = tseg1;
      *tseg2_ptr = tseg2;
    }
  }

  if ( sample_point_error_ptr )
    *sample_point_error_ptr = best_sample_point_error;

  return best_sample_point;
}

int rtems_can_bitrate2bittiming(
  struct rtems_can_chip *chip,
  struct rtems_can_bittiming *bt,
  const struct rtems_can_bittiming_const *btc
)
{
  unsigned int bitrate;
  unsigned int bitrate_error;
  unsigned int best_bitrate_error;
  unsigned int sample_point_error;
  unsigned int best_sample_point_error;
  unsigned int sample_point_nominal;
  unsigned int best_tseg;
  unsigned int best_brp;
  unsigned int brp;
  unsigned int tsegall;
  unsigned int tseg;
  unsigned int tseg1;
  unsigned int tseg2;
  uint64_t v64;

  best_bitrate_error = UINT_MAX;
  best_sample_point_error = UINT_MAX;
  best_tseg = 0;
  best_brp = 0;
  tseg1 = 0;
  tseg2 = 0;

  sample_point_nominal = bt->sample_point;

  /* Use CiA recommended sample points */
  if ( bt->sample_point == 0 ) {
    if ( bt->bitrate > 800000 ) {
      sample_point_nominal = 750;
    } else if ( bt->bitrate > 500000 ) {
      sample_point_nominal = 800;
    } else {
      sample_point_nominal = 875;
    }
  }

  /* tseg even = round down, odd = round up */

  for (
    tseg = ( btc->tseg1_max + btc->tseg2_max ) * 2 + 1;
    tseg >= ( btc->tseg1_min + btc->tseg2_min ) * 2;
    tseg--
  ) {
    tsegall = CAN_CALC_SYNC_SEG + tseg / 2;

    /* Compute all possible tseg choices (tseg = tseg1 + tseg2) */
    brp = chip->freq / (tsegall * bt->bitrate) + tseg % 2;

    /* choose brp step which is possible in system */
    brp = (brp / btc->brp_inc) * btc->brp_inc;
    if ( ( brp < btc->brp_min ) || ( brp > btc->brp_max ) ) {
      continue;
    }

    bitrate = chip->freq / ( brp * tsegall );
    bitrate_error = bt->bitrate > bitrate ?
                    bt->bitrate - bitrate :
                    bitrate - bt->bitrate;

    /* tseg brp biterror */
    if ( bitrate_error > best_bitrate_error ) {
      continue;
    }

    /* reset sample point error if we have a better bitrate */
    if ( bitrate_error < best_bitrate_error )
      best_sample_point_error = UINT_MAX;

    can_update_sample_point(
      btc,
      sample_point_nominal,
      tseg / 2,
      &tseg1,
      &tseg2,
      &sample_point_error
    );

    if ( sample_point_error > best_sample_point_error ) {
      continue;
    }

    best_sample_point_error = sample_point_error;
    best_bitrate_error = bitrate_error;
    best_tseg = tseg / 2;
    best_brp = brp;

    if ( ( bitrate_error == 0 ) && ( sample_point_error == 0 ) ) {
      break;
    }
  }

  if ( best_bitrate_error != 0 ) {
    v64 = (uint64_t) best_bitrate_error * 1000;
    v64 = v64 / bt->bitrate;
    bitrate_error = (uint32_t) v64;
    if ( bitrate_error > CAN_CALC_MAX_ERROR ) {
      return -EDOM;
    }
  }

  /* real sample point */
  bt->sample_point = can_update_sample_point(
    btc,
    sample_point_nominal,
    best_tseg,
    &tseg1,
    &tseg2,
    NULL
  );

  v64 = (uint64_t) best_brp * 1000 * 1000 * 1000;
  v64 = v64 / chip->freq;
  bt->tq = (uint32_t) v64;
  bt->prop_seg = tseg1 / 2;
  bt->phase_seg1 = tseg1 - bt->prop_seg;
  bt->phase_seg2 = tseg2;

  /* check for sjw user settings */
  if ( ( bt->sjw == 0 ) || ( btc->sjw_max == 0 ) ) {
    bt->sjw = 1;
  } else {
    /* bt->sjw is at least 1 -> sanitize upper bound to sjw_max */
    if ( bt->sjw > btc->sjw_max ) {
      bt->sjw = btc->sjw_max;
    }
    /* bt->sjw must not be higher than tseg2 */
    if ( tseg2 < bt->sjw ) {
      bt->sjw = tseg2;
    }
  }

  bt->brp = best_brp;

  /* Calculate real bitrate */
  bt->bitrate = chip->freq / ( bt->brp *
            ( CAN_CALC_SYNC_SEG + tseg1 + tseg2 ) );

  return 0;
}
