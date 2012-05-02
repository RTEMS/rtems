/*===============================================================*\
| Project: display driver for HCMS29xx                            |
+-----------------------------------------------------------------+
| File: disp_fonts.h                                              |
+-----------------------------------------------------------------+
|                    Copyright (c) 2008                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| This file declares general data structures for font management  |
\*===============================================================*/

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
