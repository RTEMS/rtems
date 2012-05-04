/*===============================================================*\
| Project: display driver for HCMS29xx                            |
+-----------------------------------------------------------------+
| File: font_hcms29xx.c                                           |
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
+-----------------------------------------------------------------+
| This file defines the 5x7 bit font used in disp_hcms29xx        |
\*===============================================================*/

#include <stddef.h>
#include "disp_fonts.h"

const unsigned char bitmap_hp_fixed_5_7_0 [5] = {
  0x08,
  0x1c,
  0x3e,
  0x7f,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_0 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_0,
};

const unsigned char bitmap_hp_fixed_5_7_1 [5] = {
  0x30,
  0x45,
  0x48,
  0x40,
  0x30
};
struct disp_font_glyph glyph_hp_fixed_5_7_1 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_1,
};

const unsigned char bitmap_hp_fixed_5_7_2 [5] = {
  0x45,
  0x29,
  0x11,
  0x29,
  0x45
};
struct disp_font_glyph glyph_hp_fixed_5_7_2 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_2,
};

const unsigned char bitmap_hp_fixed_5_7_3 [5] = {
  0x7d,
  0x09,
  0x11,
  0x21,
  0x7d
};
struct disp_font_glyph glyph_hp_fixed_5_7_3 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_3,
};

const unsigned char bitmap_hp_fixed_5_7_4 [5] = {
  0x7d,
  0x09,
  0x05,
  0x05,
  0x79
};
struct disp_font_glyph glyph_hp_fixed_5_7_4 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_4,
};

const unsigned char bitmap_hp_fixed_5_7_5 [5] = {
  0x38,
  0x44,
  0x44,
  0x38,
  0x44
};
struct disp_font_glyph glyph_hp_fixed_5_7_5 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_5,
};

const unsigned char bitmap_hp_fixed_5_7_6 [5] = {
  0x7e,
  0x01,
  0x29,
  0x2e,
  0x10
};
struct disp_font_glyph glyph_hp_fixed_5_7_6 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_6,
};

const unsigned char bitmap_hp_fixed_5_7_7 [5] = {
  0x30,
  0x4a,
  0x4d,
  0x49,
  0x30
};
struct disp_font_glyph glyph_hp_fixed_5_7_7 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_7,
};

const unsigned char bitmap_hp_fixed_5_7_8 [5] = {
  0x60,
  0x50,
  0x48,
  0x50,
  0x60
};
struct disp_font_glyph glyph_hp_fixed_5_7_8 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_8,
};

const unsigned char bitmap_hp_fixed_5_7_9 [5] = {
  0x1e,
  0x04,
  0x04,
  0x38,
  0x40
};
struct disp_font_glyph glyph_hp_fixed_5_7_9 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_9,
};

const unsigned char bitmap_hp_fixed_5_7_10 [5] = {
  0x3e,
  0x49,
  0x49,
  0x49,
  0x3e
};
struct disp_font_glyph glyph_hp_fixed_5_7_10 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_10,
};

const unsigned char bitmap_hp_fixed_5_7_11 [5] = {
  0x62,
  0x14,
  0x08,
  0x10,
  0x60
};
struct disp_font_glyph glyph_hp_fixed_5_7_11 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_11,
};

const unsigned char bitmap_hp_fixed_5_7_12 [5] = {
  0x40,
  0x3c,
  0x20,
  0x20,
  0x1c
};
struct disp_font_glyph glyph_hp_fixed_5_7_12 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_12,
};

const unsigned char bitmap_hp_fixed_5_7_13 [5] = {
  0x08,
  0x7c,
  0x04,
  0x7c,
  0x02
};
struct disp_font_glyph glyph_hp_fixed_5_7_13 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_13,
};

const unsigned char bitmap_hp_fixed_5_7_14 [5] = {
  0x38,
  0x44,
  0x44,
  0x3c,
  0x04
};
struct disp_font_glyph glyph_hp_fixed_5_7_14 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_14,
};

const unsigned char bitmap_hp_fixed_5_7_15 [5] = {
  0x41,
  0x63,
  0x55,
  0x49,
  0x41
};
struct disp_font_glyph glyph_hp_fixed_5_7_15 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_15,
};

const unsigned char bitmap_hp_fixed_5_7_16 [5] = {
  0x10,
  0x08,
  0x78,
  0x08,
  0x04
};
struct disp_font_glyph glyph_hp_fixed_5_7_16 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_16,
};

const unsigned char bitmap_hp_fixed_5_7_17 [5] = {
  0x18,
  0x24,
  0x7e,
  0x24,
  0x18
};
struct disp_font_glyph glyph_hp_fixed_5_7_17 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_17,
};

const unsigned char bitmap_hp_fixed_5_7_18 [5] = {
  0x5e,
  0x61,
  0x01,
  0x61,
  0x5e
};
struct disp_font_glyph glyph_hp_fixed_5_7_18 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_18,
};

const unsigned char bitmap_hp_fixed_5_7_19 [5] = {
  0x78,
  0x14,
  0x15,
  0x14,
  0x78
};
struct disp_font_glyph glyph_hp_fixed_5_7_19 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_19,
};

const unsigned char bitmap_hp_fixed_5_7_20 [5] = {
  0x38,
  0x44,
  0x45,
  0x3c,
  0x40
};
struct disp_font_glyph glyph_hp_fixed_5_7_20 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_20,
};

const unsigned char bitmap_hp_fixed_5_7_21 [5] = {
  0x78,
  0x15,
  0x14,
  0x15,
  0x78
};
struct disp_font_glyph glyph_hp_fixed_5_7_21 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_21,
};

const unsigned char bitmap_hp_fixed_5_7_22 [5] = {
  0x38,
  0x45,
  0x44,
  0x3d,
  0x40
};
struct disp_font_glyph glyph_hp_fixed_5_7_22 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_22,
};

const unsigned char bitmap_hp_fixed_5_7_23 [5] = {
  0x3c,
  0x43,
  0x42,
  0x43,
  0x3c
};
struct disp_font_glyph glyph_hp_fixed_5_7_23 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_23,
};

const unsigned char bitmap_hp_fixed_5_7_24 [5] = {
  0x38,
  0x45,
  0x44,
  0x45,
  0x38
};
struct disp_font_glyph glyph_hp_fixed_5_7_24 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_24,
};

const unsigned char bitmap_hp_fixed_5_7_25 [5] = {
  0x3c,
  0x41,
  0x40,
  0x41,
  0x3c
};
struct disp_font_glyph glyph_hp_fixed_5_7_25 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_25,
};

const unsigned char bitmap_hp_fixed_5_7_26 [5] = {
  0x38,
  0x42,
  0x40,
  0x42,
  0x38
};
struct disp_font_glyph glyph_hp_fixed_5_7_26 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_26,
};

const unsigned char bitmap_hp_fixed_5_7_27 [5] = {
  0x08,
  0x08,
  0x2a,
  0x1c,
  0x08
};
struct disp_font_glyph glyph_hp_fixed_5_7_27 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_27,
};

const unsigned char bitmap_hp_fixed_5_7_28 [5] = {
  0x20,
  0x7e,
  0x02,
  0x02,
  0x02
};
struct disp_font_glyph glyph_hp_fixed_5_7_28 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_28,
};

const unsigned char bitmap_hp_fixed_5_7_29 [5] = {
  0x12,
  0x19,
  0x15,
  0x12,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_29 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_29,
};

const unsigned char bitmap_hp_fixed_5_7_30 [5] = {
  0x48,
  0x7e,
  0x49,
  0x41,
  0x42
};
struct disp_font_glyph glyph_hp_fixed_5_7_30 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_30,
};

const unsigned char bitmap_hp_fixed_5_7_31 [5] = {
  0x01,
  0x12,
  0x7c,
  0x12,
  0x01
};
struct disp_font_glyph glyph_hp_fixed_5_7_31 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_31,
};

const unsigned char bitmap_hp_fixed_5_7_32 [5] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_32 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_32,
};

const unsigned char bitmap_hp_fixed_5_7_33 [5] = {
  0x00,
  0x5f,
  0x00,
  0x00,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_33 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_33,
};

const unsigned char bitmap_hp_fixed_5_7_34 [5] = {
  0x00,
  0x03,
  0x00,
  0x03,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_34 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_34,
};

const unsigned char bitmap_hp_fixed_5_7_35 [5] = {
  0x14,
  0x7f,
  0x14,
  0x7f,
  0x14
};
struct disp_font_glyph glyph_hp_fixed_5_7_35 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_35,
};

const unsigned char bitmap_hp_fixed_5_7_36 [5] = {
  0x24,
  0x2a,
  0x7f,
  0x2a,
  0x12
};
struct disp_font_glyph glyph_hp_fixed_5_7_36 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_36,
};

const unsigned char bitmap_hp_fixed_5_7_37 [5] = {
  0x23,
  0x13,
  0x08,
  0x64,
  0x62
};
struct disp_font_glyph glyph_hp_fixed_5_7_37 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_37,
};

const unsigned char bitmap_hp_fixed_5_7_38 [5] = {
  0x36,
  0x49,
  0x56,
  0x20,
  0x50
};
struct disp_font_glyph glyph_hp_fixed_5_7_38 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_38,
};

const unsigned char bitmap_hp_fixed_5_7_39 [5] = {
  0x00,
  0x0b,
  0x07,
  0x00,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_39 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_39,
};

const unsigned char bitmap_hp_fixed_5_7_40 [5] = {
  0x00,
  0x00,
  0x3e,
  0x41,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_40 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_40,
};

const unsigned char bitmap_hp_fixed_5_7_41 [5] = {
  0x00,
  0x41,
  0x3e,
  0x00,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_41 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_41,
};

const unsigned char bitmap_hp_fixed_5_7_42 [5] = {
  0x08,
  0x2a,
  0x1c,
  0x2a,
  0x08
};
struct disp_font_glyph glyph_hp_fixed_5_7_42 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_42,
};

const unsigned char bitmap_hp_fixed_5_7_43 [5] = {
  0x08,
  0x08,
  0x3e,
  0x08,
  0x08
};
struct disp_font_glyph glyph_hp_fixed_5_7_43 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_43,
};

const unsigned char bitmap_hp_fixed_5_7_44 [5] = {
  0x00,
  0x58,
  0x38,
  0x00,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_44 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_44,
};

const unsigned char bitmap_hp_fixed_5_7_45 [5] = {
  0x08,
  0x08,
  0x08,
  0x08,
  0x08
};
struct disp_font_glyph glyph_hp_fixed_5_7_45 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_45,
};

const unsigned char bitmap_hp_fixed_5_7_46 [5] = {
  0x00,
  0x30,
  0x30,
  0x00,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_46 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_46,
};

const unsigned char bitmap_hp_fixed_5_7_47 [5] = {
  0x20,
  0x10,
  0x08,
  0x04,
  0x02
};
struct disp_font_glyph glyph_hp_fixed_5_7_47 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_47,
};

const unsigned char bitmap_hp_fixed_5_7_48 [5] = {
  0x3e,
  0x51,
  0x49,
  0x45,
  0x3e
};
struct disp_font_glyph glyph_hp_fixed_5_7_48 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_48,
};

const unsigned char bitmap_hp_fixed_5_7_49 [5] = {
  0x00,
  0x42,
  0x7f,
  0x40,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_49 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_49,
};

const unsigned char bitmap_hp_fixed_5_7_50 [5] = {
  0x62,
  0x51,
  0x49,
  0x49,
  0x46
};
struct disp_font_glyph glyph_hp_fixed_5_7_50 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_50,
};

const unsigned char bitmap_hp_fixed_5_7_51 [5] = {
  0x22,
  0x41,
  0x49,
  0x49,
  0x36
};
struct disp_font_glyph glyph_hp_fixed_5_7_51 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_51,
};

const unsigned char bitmap_hp_fixed_5_7_52 [5] = {
  0x18,
  0x14,
  0x12,
  0x7f,
  0x10
};
struct disp_font_glyph glyph_hp_fixed_5_7_52 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_52,
};

const unsigned char bitmap_hp_fixed_5_7_53 [5] = {
  0x27,
  0x45,
  0x45,
  0x45,
  0x39
};
struct disp_font_glyph glyph_hp_fixed_5_7_53 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_53,
};

const unsigned char bitmap_hp_fixed_5_7_54 [5] = {
  0x3c,
  0x4a,
  0x49,
  0x49,
  0x30
};
struct disp_font_glyph glyph_hp_fixed_5_7_54 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_54,
};

const unsigned char bitmap_hp_fixed_5_7_55 [5] = {
  0x01,
  0x71,
  0x09,
  0x05,
  0x03
};
struct disp_font_glyph glyph_hp_fixed_5_7_55 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_55,
};

const unsigned char bitmap_hp_fixed_5_7_56 [5] = {
  0x36,
  0x49,
  0x49,
  0x49,
  0x36
};
struct disp_font_glyph glyph_hp_fixed_5_7_56 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_56,
};

const unsigned char bitmap_hp_fixed_5_7_57 [5] = {
  0x06,
  0x49,
  0x49,
  0x29,
  0x1e
};
struct disp_font_glyph glyph_hp_fixed_5_7_57 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_57,
};

const unsigned char bitmap_hp_fixed_5_7_58 [5] = {
  0x00,
  0x36,
  0x36,
  0x00,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_58 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_58,
};

const unsigned char bitmap_hp_fixed_5_7_59 [5] = {
  0x00,
  0x5b,
  0x3b,
  0x00,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_59 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_59,
};

const unsigned char bitmap_hp_fixed_5_7_60 [5] = {
  0x00,
  0x08,
  0x14,
  0x22,
  0x41
};
struct disp_font_glyph glyph_hp_fixed_5_7_60 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_60,
};

const unsigned char bitmap_hp_fixed_5_7_61 [5] = {
  0x14,
  0x14,
  0x14,
  0x14,
  0x14
};
struct disp_font_glyph glyph_hp_fixed_5_7_61 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_61,
};

const unsigned char bitmap_hp_fixed_5_7_62 [5] = {
  0x41,
  0x22,
  0x14,
  0x08,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_62 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_62,
};

const unsigned char bitmap_hp_fixed_5_7_63 [5] = {
  0x02,
  0x01,
  0x51,
  0x09,
  0x06
};
struct disp_font_glyph glyph_hp_fixed_5_7_63 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_63,
};

const unsigned char bitmap_hp_fixed_5_7_64 [5] = {
  0x3e,
  0x41,
  0x5d,
  0x55,
  0x1e
};
struct disp_font_glyph glyph_hp_fixed_5_7_64 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_64,
};

const unsigned char bitmap_hp_fixed_5_7_65 [5] = {
  0x7e,
  0x09,
  0x09,
  0x09,
  0x7e
};
struct disp_font_glyph glyph_hp_fixed_5_7_65 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_65,
};

const unsigned char bitmap_hp_fixed_5_7_66 [5] = {
  0x7e,
  0x49,
  0x49,
  0x49,
  0x36
};
struct disp_font_glyph glyph_hp_fixed_5_7_66 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_66,
};

const unsigned char bitmap_hp_fixed_5_7_67 [5] = {
  0x3e,
  0x41,
  0x41,
  0x41,
  0x22
};
struct disp_font_glyph glyph_hp_fixed_5_7_67 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_67,
};

const unsigned char bitmap_hp_fixed_5_7_68 [5] = {
  0x7f,
  0x41,
  0x41,
  0x41,
  0x3e
};
struct disp_font_glyph glyph_hp_fixed_5_7_68 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_68,
};

const unsigned char bitmap_hp_fixed_5_7_69 [5] = {
  0x7f,
  0x49,
  0x49,
  0x49,
  0x41
};
struct disp_font_glyph glyph_hp_fixed_5_7_69 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_69,
};

const unsigned char bitmap_hp_fixed_5_7_70 [5] = {
  0x7f,
  0x09,
  0x09,
  0x09,
  0x01
};
struct disp_font_glyph glyph_hp_fixed_5_7_70 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_70,
};

const unsigned char bitmap_hp_fixed_5_7_71 [5] = {
  0x3e,
  0x41,
  0x41,
  0x51,
  0x32
};
struct disp_font_glyph glyph_hp_fixed_5_7_71 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_71,
};

const unsigned char bitmap_hp_fixed_5_7_72 [5] = {
  0x7f,
  0x08,
  0x08,
  0x08,
  0x7f
};
struct disp_font_glyph glyph_hp_fixed_5_7_72 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_72,
};

const unsigned char bitmap_hp_fixed_5_7_73 [5] = {
  0x00,
  0x41,
  0x7f,
  0x41,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_73 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_73,
};

const unsigned char bitmap_hp_fixed_5_7_74 [5] = {
  0x20,
  0x40,
  0x40,
  0x40,
  0x3f
};
struct disp_font_glyph glyph_hp_fixed_5_7_74 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_74,
};

const unsigned char bitmap_hp_fixed_5_7_75 [5] = {
  0x7f,
  0x08,
  0x14,
  0x22,
  0x41
};
struct disp_font_glyph glyph_hp_fixed_5_7_75 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_75,
};

const unsigned char bitmap_hp_fixed_5_7_76 [5] = {
  0x7f,
  0x40,
  0x40,
  0x40,
  0x40
};
struct disp_font_glyph glyph_hp_fixed_5_7_76 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_76,
};

const unsigned char bitmap_hp_fixed_5_7_77 [5] = {
  0x7f,
  0x02,
  0x0c,
  0x02,
  0x7f
};
struct disp_font_glyph glyph_hp_fixed_5_7_77 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_77,
};

const unsigned char bitmap_hp_fixed_5_7_78 [5] = {
  0x7f,
  0x04,
  0x08,
  0x10,
  0x7f
};
struct disp_font_glyph glyph_hp_fixed_5_7_78 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_78,
};

const unsigned char bitmap_hp_fixed_5_7_79 [5] = {
  0x3e,
  0x41,
  0x41,
  0x41,
  0x3e
};
struct disp_font_glyph glyph_hp_fixed_5_7_79 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_79,
};

const unsigned char bitmap_hp_fixed_5_7_80 [5] = {
  0x7f,
  0x09,
  0x09,
  0x09,
  0x06
};
struct disp_font_glyph glyph_hp_fixed_5_7_80 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_80,
};

const unsigned char bitmap_hp_fixed_5_7_81 [5] = {
  0x3e,
  0x41,
  0x51,
  0x21,
  0x5e
};
struct disp_font_glyph glyph_hp_fixed_5_7_81 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_81,
};

const unsigned char bitmap_hp_fixed_5_7_82 [5] = {
  0x7f,
  0x09,
  0x19,
  0x29,
  0x46
};
struct disp_font_glyph glyph_hp_fixed_5_7_82 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_82,
};

const unsigned char bitmap_hp_fixed_5_7_83 [5] = {
  0x26,
  0x49,
  0x49,
  0x49,
  0x32
};
struct disp_font_glyph glyph_hp_fixed_5_7_83 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_83,
};

const unsigned char bitmap_hp_fixed_5_7_84 [5] = {
  0x01,
  0x01,
  0x7f,
  0x01,
  0x01
};
struct disp_font_glyph glyph_hp_fixed_5_7_84 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_84,
};

const unsigned char bitmap_hp_fixed_5_7_85 [5] = {
  0x3f,
  0x40,
  0x40,
  0x40,
  0x3f
};
struct disp_font_glyph glyph_hp_fixed_5_7_85 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_85,
};

const unsigned char bitmap_hp_fixed_5_7_86 [5] = {
  0x07,
  0x18,
  0x60,
  0x18,
  0x07
};
struct disp_font_glyph glyph_hp_fixed_5_7_86 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_86,
};

const unsigned char bitmap_hp_fixed_5_7_87 [5] = {
  0x7f,
  0x20,
  0x18,
  0x20,
  0x7f
};
struct disp_font_glyph glyph_hp_fixed_5_7_87 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_87,
};

const unsigned char bitmap_hp_fixed_5_7_88 [5] = {
  0x63,
  0x14,
  0x08,
  0x14,
  0x63
};
struct disp_font_glyph glyph_hp_fixed_5_7_88 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_88,
};

const unsigned char bitmap_hp_fixed_5_7_89 [5] = {
  0x03,
  0x04,
  0x78,
  0x04,
  0x03
};
struct disp_font_glyph glyph_hp_fixed_5_7_89 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_89,
};

const unsigned char bitmap_hp_fixed_5_7_90 [5] = {
  0x61,
  0x51,
  0x49,
  0x45,
  0x43
};
struct disp_font_glyph glyph_hp_fixed_5_7_90 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_90,
};

const unsigned char bitmap_hp_fixed_5_7_91 [5] = {
  0x00,
  0x00,
  0x7f,
  0x41,
  0x41
};
struct disp_font_glyph glyph_hp_fixed_5_7_91 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_91,
};

const unsigned char bitmap_hp_fixed_5_7_92 [5] = {
  0x02,
  0x04,
  0x08,
  0x10,
  0x20
};
struct disp_font_glyph glyph_hp_fixed_5_7_92 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_92,
};

const unsigned char bitmap_hp_fixed_5_7_93 [5] = {
  0x41,
  0x41,
  0x7f,
  0x00,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_93 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_93,
};

const unsigned char bitmap_hp_fixed_5_7_94 [5] = {
  0x04,
  0x02,
  0x7f,
  0x02,
  0x04
};
struct disp_font_glyph glyph_hp_fixed_5_7_94 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_94,
};

const unsigned char bitmap_hp_fixed_5_7_95 [5] = {
  0x40,
  0x40,
  0x40,
  0x40,
  0x40
};
struct disp_font_glyph glyph_hp_fixed_5_7_95 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_95,
};

const unsigned char bitmap_hp_fixed_5_7_96 [5] = {
  0x00,
  0x07,
  0x0b,
  0x00,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_96 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_96,
};

const unsigned char bitmap_hp_fixed_5_7_97 [5] = {
  0x38,
  0x44,
  0x44,
  0x3c,
  0x40
};
struct disp_font_glyph glyph_hp_fixed_5_7_97 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_97,
};

const unsigned char bitmap_hp_fixed_5_7_98 [5] = {
  0x7f,
  0x48,
  0x44,
  0x44,
  0x38
};
struct disp_font_glyph glyph_hp_fixed_5_7_98 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_98,
};

const unsigned char bitmap_hp_fixed_5_7_99 [5] = {
  0x38,
  0x44,
  0x44,
  0x44,
  0x44
};
struct disp_font_glyph glyph_hp_fixed_5_7_99 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_99,
};

const unsigned char bitmap_hp_fixed_5_7_100 [5] = {
  0x38,
  0x44,
  0x44,
  0x48,
  0x7f
};
struct disp_font_glyph glyph_hp_fixed_5_7_100 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_100,
};

const unsigned char bitmap_hp_fixed_5_7_101 [5] = {
  0x38,
  0x54,
  0x54,
  0x54,
  0x08
};
struct disp_font_glyph glyph_hp_fixed_5_7_101 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_101,
};

const unsigned char bitmap_hp_fixed_5_7_102 [5] = {
  0x08,
  0x7e,
  0x09,
  0x02,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_102 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_102,
};

const unsigned char bitmap_hp_fixed_5_7_103 [5] = {
  0x08,
  0x14,
  0x54,
  0x54,
  0x3c
};
struct disp_font_glyph glyph_hp_fixed_5_7_103 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_103,
};

const unsigned char bitmap_hp_fixed_5_7_104 [5] = {
  0x7f,
  0x08,
  0x04,
  0x04,
  0x78
};
struct disp_font_glyph glyph_hp_fixed_5_7_104 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_104,
};

const unsigned char bitmap_hp_fixed_5_7_105 [5] = {
  0x00,
  0x44,
  0x7d,
  0x40,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_105 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_105,
};

const unsigned char bitmap_hp_fixed_5_7_106 [5] = {
  0x20,
  0x40,
  0x44,
  0x3d,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_106 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_106,
};

const unsigned char bitmap_hp_fixed_5_7_107 [5] = {
  0x00,
  0x7f,
  0x10,
  0x28,
  0x44
};
struct disp_font_glyph glyph_hp_fixed_5_7_107 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_107,
};

const unsigned char bitmap_hp_fixed_5_7_108 [5] = {
  0x00,
  0x41,
  0x7f,
  0x40,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_108 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_108,
};

const unsigned char bitmap_hp_fixed_5_7_109 [5] = {
  0x78,
  0x04,
  0x18,
  0x04,
  0x78
};
struct disp_font_glyph glyph_hp_fixed_5_7_109 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_109,
};

const unsigned char bitmap_hp_fixed_5_7_110 [5] = {
  0x7c,
  0x08,
  0x04,
  0x04,
  0x78
};
struct disp_font_glyph glyph_hp_fixed_5_7_110 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_110,
};

const unsigned char bitmap_hp_fixed_5_7_111 [5] = {
  0x38,
  0x44,
  0x44,
  0x44,
  0x38
};
struct disp_font_glyph glyph_hp_fixed_5_7_111 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_111,
};

const unsigned char bitmap_hp_fixed_5_7_112 [5] = {
  0x7c,
  0x14,
  0x24,
  0x24,
  0x18
};
struct disp_font_glyph glyph_hp_fixed_5_7_112 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_112,
};

const unsigned char bitmap_hp_fixed_5_7_113 [5] = {
  0x18,
  0x24,
  0x14,
  0x7c,
  0x40
};
struct disp_font_glyph glyph_hp_fixed_5_7_113 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_113,
};

const unsigned char bitmap_hp_fixed_5_7_114 [5] = {
  0x00,
  0x7c,
  0x08,
  0x04,
  0x04
};
struct disp_font_glyph glyph_hp_fixed_5_7_114 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_114,
};

const unsigned char bitmap_hp_fixed_5_7_115 [5] = {
  0x48,
  0x54,
  0x54,
  0x54,
  0x20
};
struct disp_font_glyph glyph_hp_fixed_5_7_115 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_115,
};

const unsigned char bitmap_hp_fixed_5_7_116 [5] = {
  0x04,
  0x3e,
  0x44,
  0x20,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_116 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_116,
};

const unsigned char bitmap_hp_fixed_5_7_117 [5] = {
  0x3c,
  0x40,
  0x40,
  0x20,
  0x7c
};
struct disp_font_glyph glyph_hp_fixed_5_7_117 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_117,
};

const unsigned char bitmap_hp_fixed_5_7_118 [5] = {
  0x1c,
  0x20,
  0x40,
  0x20,
  0x1c
};
struct disp_font_glyph glyph_hp_fixed_5_7_118 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_118,
};

const unsigned char bitmap_hp_fixed_5_7_119 [5] = {
  0x3c,
  0x40,
  0x30,
  0x40,
  0x3c
};
struct disp_font_glyph glyph_hp_fixed_5_7_119 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_119,
};

const unsigned char bitmap_hp_fixed_5_7_120 [5] = {
  0x44,
  0x28,
  0x10,
  0x28,
  0x44
};
struct disp_font_glyph glyph_hp_fixed_5_7_120 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_120,
};

const unsigned char bitmap_hp_fixed_5_7_121 [5] = {
  0x04,
  0x48,
  0x30,
  0x08,
  0x04
};
struct disp_font_glyph glyph_hp_fixed_5_7_121 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_121,
};

const unsigned char bitmap_hp_fixed_5_7_122 [5] = {
  0x44,
  0x64,
  0x54,
  0x4c,
  0x44
};
struct disp_font_glyph glyph_hp_fixed_5_7_122 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_122,
};

const unsigned char bitmap_hp_fixed_5_7_123 [5] = {
  0x00,
  0x08,
  0x36,
  0x41,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_123 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_123,
};

const unsigned char bitmap_hp_fixed_5_7_124 [5] = {
  0x00,
  0x00,
  0x77,
  0x00,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_124 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_124,
};

const unsigned char bitmap_hp_fixed_5_7_125 [5] = {
  0x00,
  0x41,
  0x36,
  0x08,
  0x00
};
struct disp_font_glyph glyph_hp_fixed_5_7_125 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_125,
};

const unsigned char bitmap_hp_fixed_5_7_126 [5] = {
  0x08,
  0x04,
  0x08,
  0x10,
  0x08
};
struct disp_font_glyph glyph_hp_fixed_5_7_126 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_126,
};

const unsigned char bitmap_hp_fixed_5_7_127 [5] = {
  0x2a,
  0x55,
  0x2a,
  0x55,
  0x2a
};
struct disp_font_glyph glyph_hp_fixed_5_7_127 = {
  {5,7,0,0},
  5, 0, bitmap_hp_fixed_5_7_127,
};

const struct disp_font_base font_hcms29xx_base = {
  1,               /* trans */
  {5, 7, 0, -1},  /* fbb w, h, x, y */
  7, 0,            /* ascent, descent */
  0,               /* default_char */
  {&glyph_hp_fixed_5_7_0,
  &glyph_hp_fixed_5_7_1,
  &glyph_hp_fixed_5_7_2,
  &glyph_hp_fixed_5_7_3,
  &glyph_hp_fixed_5_7_4,
  &glyph_hp_fixed_5_7_5,
  &glyph_hp_fixed_5_7_6,
  &glyph_hp_fixed_5_7_7,
  &glyph_hp_fixed_5_7_8,
  &glyph_hp_fixed_5_7_9,
  &glyph_hp_fixed_5_7_10,
  &glyph_hp_fixed_5_7_11,
  &glyph_hp_fixed_5_7_12,
  &glyph_hp_fixed_5_7_13,
  &glyph_hp_fixed_5_7_14,
  &glyph_hp_fixed_5_7_15,
  &glyph_hp_fixed_5_7_16,
  &glyph_hp_fixed_5_7_17,
  &glyph_hp_fixed_5_7_18,
  &glyph_hp_fixed_5_7_19,
  &glyph_hp_fixed_5_7_20,
  &glyph_hp_fixed_5_7_21,
  &glyph_hp_fixed_5_7_22,
  &glyph_hp_fixed_5_7_23,
  &glyph_hp_fixed_5_7_24,
  &glyph_hp_fixed_5_7_25,
  &glyph_hp_fixed_5_7_26,
  &glyph_hp_fixed_5_7_27,
  &glyph_hp_fixed_5_7_28,
  &glyph_hp_fixed_5_7_29,
  &glyph_hp_fixed_5_7_30,
  &glyph_hp_fixed_5_7_31,
  &glyph_hp_fixed_5_7_32,
  &glyph_hp_fixed_5_7_33,
  &glyph_hp_fixed_5_7_34,
  &glyph_hp_fixed_5_7_35,
  &glyph_hp_fixed_5_7_36,
  &glyph_hp_fixed_5_7_37,
  &glyph_hp_fixed_5_7_38,
  &glyph_hp_fixed_5_7_39,
  &glyph_hp_fixed_5_7_40,
  &glyph_hp_fixed_5_7_41,
  &glyph_hp_fixed_5_7_42,
  &glyph_hp_fixed_5_7_43,
  &glyph_hp_fixed_5_7_44,
  &glyph_hp_fixed_5_7_45,
  &glyph_hp_fixed_5_7_46,
  &glyph_hp_fixed_5_7_47,
  &glyph_hp_fixed_5_7_48,
  &glyph_hp_fixed_5_7_49,
  &glyph_hp_fixed_5_7_50,
  &glyph_hp_fixed_5_7_51,
  &glyph_hp_fixed_5_7_52,
  &glyph_hp_fixed_5_7_53,
  &glyph_hp_fixed_5_7_54,
  &glyph_hp_fixed_5_7_55,
  &glyph_hp_fixed_5_7_56,
  &glyph_hp_fixed_5_7_57,
  &glyph_hp_fixed_5_7_58,
  &glyph_hp_fixed_5_7_59,
  &glyph_hp_fixed_5_7_60,
  &glyph_hp_fixed_5_7_61,
  &glyph_hp_fixed_5_7_62,
  &glyph_hp_fixed_5_7_63,
  &glyph_hp_fixed_5_7_64,
  &glyph_hp_fixed_5_7_65,
  &glyph_hp_fixed_5_7_66,
  &glyph_hp_fixed_5_7_67,
  &glyph_hp_fixed_5_7_68,
  &glyph_hp_fixed_5_7_69,
  &glyph_hp_fixed_5_7_70,
  &glyph_hp_fixed_5_7_71,
  &glyph_hp_fixed_5_7_72,
  &glyph_hp_fixed_5_7_73,
  &glyph_hp_fixed_5_7_74,
  &glyph_hp_fixed_5_7_75,
  &glyph_hp_fixed_5_7_76,
  &glyph_hp_fixed_5_7_77,
  &glyph_hp_fixed_5_7_78,
  &glyph_hp_fixed_5_7_79,
  &glyph_hp_fixed_5_7_80,
  &glyph_hp_fixed_5_7_81,
  &glyph_hp_fixed_5_7_82,
  &glyph_hp_fixed_5_7_83,
  &glyph_hp_fixed_5_7_84,
  &glyph_hp_fixed_5_7_85,
  &glyph_hp_fixed_5_7_86,
  &glyph_hp_fixed_5_7_87,
  &glyph_hp_fixed_5_7_88,
  &glyph_hp_fixed_5_7_89,
  &glyph_hp_fixed_5_7_90,
  &glyph_hp_fixed_5_7_91,
  &glyph_hp_fixed_5_7_92,
  &glyph_hp_fixed_5_7_93,
  &glyph_hp_fixed_5_7_94,
  &glyph_hp_fixed_5_7_95,
  &glyph_hp_fixed_5_7_96,
  &glyph_hp_fixed_5_7_97,
  &glyph_hp_fixed_5_7_98,
  &glyph_hp_fixed_5_7_99,
  &glyph_hp_fixed_5_7_100,
  &glyph_hp_fixed_5_7_101,
  &glyph_hp_fixed_5_7_102,
  &glyph_hp_fixed_5_7_103,
  &glyph_hp_fixed_5_7_104,
  &glyph_hp_fixed_5_7_105,
  &glyph_hp_fixed_5_7_106,
  &glyph_hp_fixed_5_7_107,
  &glyph_hp_fixed_5_7_108,
  &glyph_hp_fixed_5_7_109,
  &glyph_hp_fixed_5_7_110,
  &glyph_hp_fixed_5_7_111,
  &glyph_hp_fixed_5_7_112,
  &glyph_hp_fixed_5_7_113,
  &glyph_hp_fixed_5_7_114,
  &glyph_hp_fixed_5_7_115,
  &glyph_hp_fixed_5_7_116,
  &glyph_hp_fixed_5_7_117,
  &glyph_hp_fixed_5_7_118,
  &glyph_hp_fixed_5_7_119,
  &glyph_hp_fixed_5_7_120,
  &glyph_hp_fixed_5_7_121,
  &glyph_hp_fixed_5_7_122,
  &glyph_hp_fixed_5_7_123,
  &glyph_hp_fixed_5_7_124,
  &glyph_hp_fixed_5_7_125,
  &glyph_hp_fixed_5_7_126,
  &glyph_hp_fixed_5_7_127,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
   NULL}
};
