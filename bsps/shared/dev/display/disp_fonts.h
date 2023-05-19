/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Display driver for HCMS29xx.
 *
 * This file declares general data structures for font management.
 */

/*
 * Copyright (c) 2008 embedded brains GmbH & Co. KG
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

#ifndef DISP_FONTS_H
#define DISP_FONTS_H

#include <rtems.h>

typedef int8_t disp_font_dimen;

struct disp_font_bounding_box
{
  disp_font_dimen w, h, x, y;
};

struct disp_font_glyph
{
  struct disp_font_bounding_box bb;
  disp_font_dimen wx, wy;
  const unsigned char *bitmap;
};

struct disp_font_base
{
  int8_t trans;
  struct disp_font_bounding_box fbb;
  disp_font_dimen ascent, descent;
  uint8_t default_char;
  struct disp_font_glyph *latin1[256];
};

typedef struct disp_font_base *disp_font_t;

/* Prototypes ------------------------------------------------- */

/* End -------------------------------------------------------- */

#endif /* not defined DISP_FONTS_H */
