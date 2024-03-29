/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This header file provides GIO interfaces.
 */

/* The header file is generated by make_header.py from GIO.json */
/* Current script's version can be found at: */
/* https://github.com/AoLaD/rtems-tms570-utils/tree/headers/headers/python */

/*
 * Copyright (c) 2014-2015, Premysl Houdek <kom541000@gmail.com>
 *
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
*/
#ifndef LIBBSP_ARM_TMS570_GIO
#define LIBBSP_ARM_TMS570_GIO

#include <bsp/utility.h>

typedef struct{
  uint32_t DIR;               /*GIO Data Direction Register*/
  uint32_t DIN;               /*GIO Data Input Register*/
  uint32_t DOUT;              /*GIO Data Output Register*/
  uint32_t DSET;              /*GIO Data Set Register*/
  uint32_t DCLR;              /*GIO Data Clear Register*/
  uint32_t PDR;               /*GIO Open Drain Register*/
  uint32_t PULDIS;            /*GIO Pull Disable Register*/
  uint32_t PSL;               /*GIO Pull Select Register*/
} tms570_gio_port_t;

typedef struct{
  uint32_t GCR0;              /*GIO Global Control Register*/
  uint8_t reserved1 [4];
  uint32_t INTDET;            /*GIO Interrupt Detect Register*/
  uint32_t POL;               /*GIO Interrupt Polarity Register*/
  uint32_t ENASET;            /*GIO Interrupt Enable Set Register*/
  uint32_t ENACLR;            /*GIO Interrupt Enable Clear Register*/
  uint32_t LVLSET;            /*GIO Interrupt Priority Set Register*/
  uint32_t LVLCLR;            /*GIO Interrupt Priority Clear Register*/
  uint32_t FLG;               /*GIO Interrupt Flag Register*/
  uint32_t OFF1;              /*GIO Offset 1 Register*/
  uint32_t OFF2;              /*GIO Offset 2 Register*/
  uint32_t EMU1;              /*GIO Emulation 1 Register*/
  uint32_t EMU2;              /*GIO Emulation 2 Register*/
  tms570_gio_port_t ports[8]; /*GIO ports*/
} tms570_gio_t;


/*-----------------------TMS570_GIO_DIR-----------------------*/
/* field: GIODIR - GIO data direction, pins [7:0] */
#define TMS570_GIO_DIR_GIODIR(val) BSP_FLD32(val,0, 7)
#define TMS570_GIO_DIR_GIODIR_GET(reg) BSP_FLD32GET(reg,0, 7)
#define TMS570_GIO_DIR_GIODIR_SET(reg,val) BSP_FLD32SET(reg, val,0, 7)


/*-----------------------TMS570_GIO_DIN-----------------------*/
/* field: GIODIN - GIO data input, pins [7:0] */
#define TMS570_GIO_DIN_GIODIN(val) BSP_FLD32(val,0, 7)
#define TMS570_GIO_DIN_GIODIN_GET(reg) BSP_FLD32GET(reg,0, 7)
#define TMS570_GIO_DIN_GIODIN_SET(reg,val) BSP_FLD32SET(reg, val,0, 7)


/*----------------------TMS570_GIO_DOUT----------------------*/
/* field: GIODOUT - IO data output, pins[7:0]. */
#define TMS570_GIO_DOUT_GIODOUT(val) BSP_FLD32(val,0, 7)
#define TMS570_GIO_DOUT_GIODOUT_GET(reg) BSP_FLD32GET(reg,0, 7)
#define TMS570_GIO_DOUT_GIODOUT_SET(reg,val) BSP_FLD32SET(reg, val,0, 7)


/*----------------------TMS570_GIO_DSET----------------------*/
/* field: GIODSET - GIO data set, pins[7:0]. This bit drives the output of GIO pin high. */
#define TMS570_GIO_DSET_GIODSET(val) BSP_FLD32(val,0, 7)
#define TMS570_GIO_DSET_GIODSET_GET(reg) BSP_FLD32GET(reg,0, 7)
#define TMS570_GIO_DSET_GIODSET_SET(reg,val) BSP_FLD32SET(reg, val,0, 7)


/*----------------------TMS570_GIO_DCLR----------------------*/
/* field: GIODCLR - GIO data clear, pins[7:0]. This bit drives the output of GIO pin low. */
#define TMS570_GIO_DCLR_GIODCLR(val) BSP_FLD32(val,0, 7)
#define TMS570_GIO_DCLR_GIODCLR_GET(reg) BSP_FLD32GET(reg,0, 7)
#define TMS570_GIO_DCLR_GIODCLR_SET(reg,val) BSP_FLD32SET(reg, val,0, 7)


/*-----------------------TMS570_GIO_PDR-----------------------*/
/* field: 7_0 - GIOPDRH GIO open drain, pins[7:0] */
#define TMS570_GIO_PDR_7_0(val) BSP_FLD32(val,0, 7)
#define TMS570_GIO_PDR_7_0_GET(reg) BSP_FLD32GET(reg,0, 7)
#define TMS570_GIO_PDR_7_0_SET(reg,val) BSP_FLD32SET(reg, val,0, 7)


/*---------------------TMS570_GIO_PULDIS---------------------*/
/* field: GIOPULDIS - GIO pull disable, pins[7:0]. */
#define TMS570_GIO_PULDIS_GIOPULDIS(val) BSP_FLD32(val,0, 7)
#define TMS570_GIO_PULDIS_GIOPULDIS_GET(reg) BSP_FLD32GET(reg,0, 7)
#define TMS570_GIO_PULDIS_GIOPULDIS_SET(reg,val) BSP_FLD32SET(reg, val,0, 7)


/*-----------------------TMS570_GIO_PSL-----------------------*/
/* field: GIOPSL - GIO pull select, pins[7:0] */
#define TMS570_GIO_PSL_GIOPSL(val) BSP_FLD32(val,0, 7)
#define TMS570_GIO_PSL_GIOPSL_GET(reg) BSP_FLD32GET(reg,0, 7)
#define TMS570_GIO_PSL_GIOPSL_SET(reg,val) BSP_FLD32SET(reg, val,0, 7)


/*----------------------TMS570_GIO_GCR0----------------------*/
/* field: RESET - GIO reset. */
#define TMS570_GIO_GCR0_RESET BSP_BIT32(0)


/*---------------------TMS570_GIO_INTDET---------------------*/
/* field: GIOINTDET_3 - Interrupt detection select for pins GIOD[7:0] */
#define TMS570_GIO_INTDET_GIOINTDET_3(val) BSP_FLD32(val,24, 31)
#define TMS570_GIO_INTDET_GIOINTDET_3_GET(reg) BSP_FLD32GET(reg,24, 31)
#define TMS570_GIO_INTDET_GIOINTDET_3_SET(reg,val) BSP_FLD32SET(reg, val,24, 31)

/* field: GIOINTDET_2 - Interrupt detection select for pins GIOC[7:0] */
#define TMS570_GIO_INTDET_GIOINTDET_2(val) BSP_FLD32(val,16, 23)
#define TMS570_GIO_INTDET_GIOINTDET_2_GET(reg) BSP_FLD32GET(reg,16, 23)
#define TMS570_GIO_INTDET_GIOINTDET_2_SET(reg,val) BSP_FLD32SET(reg, val,16, 23)

/* field: GIOINTDET_1 - Interrupt detection select for pins GIOB[7:0] */
#define TMS570_GIO_INTDET_GIOINTDET_1(val) BSP_FLD32(val,8, 15)
#define TMS570_GIO_INTDET_GIOINTDET_1_GET(reg) BSP_FLD32GET(reg,8, 15)
#define TMS570_GIO_INTDET_GIOINTDET_1_SET(reg,val) BSP_FLD32SET(reg, val,8, 15)

/* field: GIOINTDET_0 - Interrupt detection select for pins GIOA[7:0] */
#define TMS570_GIO_INTDET_GIOINTDET_0(val) BSP_FLD32(val,0, 7)
#define TMS570_GIO_INTDET_GIOINTDET_0_GET(reg) BSP_FLD32GET(reg,0, 7)
#define TMS570_GIO_INTDET_GIOINTDET_0_SET(reg,val) BSP_FLD32SET(reg, val,0, 7)


/*-----------------------TMS570_GIO_POL-----------------------*/
/* field: GIOPOL_3 - Interrupt polarity select for pins GIOD[7:0] */
#define TMS570_GIO_POL_GIOPOL_3(val) BSP_FLD32(val,24, 31)
#define TMS570_GIO_POL_GIOPOL_3_GET(reg) BSP_FLD32GET(reg,24, 31)
#define TMS570_GIO_POL_GIOPOL_3_SET(reg,val) BSP_FLD32SET(reg, val,24, 31)

/* field: GIOPOL_2 - Interrupt polarity select for pins GIOC[7:0] */
#define TMS570_GIO_POL_GIOPOL_2(val) BSP_FLD32(val,16, 23)
#define TMS570_GIO_POL_GIOPOL_2_GET(reg) BSP_FLD32GET(reg,16, 23)
#define TMS570_GIO_POL_GIOPOL_2_SET(reg,val) BSP_FLD32SET(reg, val,16, 23)

/* field: GIOPOL_1 - Interrupt polarity select for pins GIOB[7:0] */
#define TMS570_GIO_POL_GIOPOL_1(val) BSP_FLD32(val,8, 15)
#define TMS570_GIO_POL_GIOPOL_1_GET(reg) BSP_FLD32GET(reg,8, 15)
#define TMS570_GIO_POL_GIOPOL_1_SET(reg,val) BSP_FLD32SET(reg, val,8, 15)

/* field: GIOPOL_0 - Interrupt polarity select for pins GIOA[7:0] */
#define TMS570_GIO_POL_GIOPOL_0(val) BSP_FLD32(val,0, 7)
#define TMS570_GIO_POL_GIOPOL_0_GET(reg) BSP_FLD32GET(reg,0, 7)
#define TMS570_GIO_POL_GIOPOL_0_SET(reg,val) BSP_FLD32SET(reg, val,0, 7)


/*---------------------TMS570_GIO_ENASET---------------------*/
/* field: GIOENASET_3 - nterrupt enable for pins GIOD[7:0] */
#define TMS570_GIO_ENASET_GIOENASET_3(val) BSP_FLD32(val,24, 31)
#define TMS570_GIO_ENASET_GIOENASET_3_GET(reg) BSP_FLD32GET(reg,24, 31)
#define TMS570_GIO_ENASET_GIOENASET_3_SET(reg,val) BSP_FLD32SET(reg, val,24, 31)

/* field: GIOENASET_2 - Interrupt enable for pins GIOC[7:0] */
#define TMS570_GIO_ENASET_GIOENASET_2(val) BSP_FLD32(val,16, 23)
#define TMS570_GIO_ENASET_GIOENASET_2_GET(reg) BSP_FLD32GET(reg,16, 23)
#define TMS570_GIO_ENASET_GIOENASET_2_SET(reg,val) BSP_FLD32SET(reg, val,16, 23)

/* field: GIOENASET_1 - Interrupt enable for pins GIOB[7:0] */
#define TMS570_GIO_ENASET_GIOENASET_1(val) BSP_FLD32(val,8, 15)
#define TMS570_GIO_ENASET_GIOENASET_1_GET(reg) BSP_FLD32GET(reg,8, 15)
#define TMS570_GIO_ENASET_GIOENASET_1_SET(reg,val) BSP_FLD32SET(reg, val,8, 15)

/* field: GIOENASET_0 - Interrupt enable for pins GIOA[7:0] */
#define TMS570_GIO_ENASET_GIOENASET_0(val) BSP_FLD32(val,0, 7)
#define TMS570_GIO_ENASET_GIOENASET_0_GET(reg) BSP_FLD32GET(reg,0, 7)
#define TMS570_GIO_ENASET_GIOENASET_0_SET(reg,val) BSP_FLD32SET(reg, val,0, 7)


/*---------------------TMS570_GIO_ENACLR---------------------*/
/* field: GIOENACLR_3 - Interrupt enable for pins GIOD[7:0] */
#define TMS570_GIO_ENACLR_GIOENACLR_3(val) BSP_FLD32(val,24, 31)
#define TMS570_GIO_ENACLR_GIOENACLR_3_GET(reg) BSP_FLD32GET(reg,24, 31)
#define TMS570_GIO_ENACLR_GIOENACLR_3_SET(reg,val) BSP_FLD32SET(reg, val,24, 31)

/* field: GIOENACLR_2 - Interrupt enable for pins GIOC[7:0] */
#define TMS570_GIO_ENACLR_GIOENACLR_2(val) BSP_FLD32(val,16, 23)
#define TMS570_GIO_ENACLR_GIOENACLR_2_GET(reg) BSP_FLD32GET(reg,16, 23)
#define TMS570_GIO_ENACLR_GIOENACLR_2_SET(reg,val) BSP_FLD32SET(reg, val,16, 23)

/* field: GIOENACLR_1 - Interrupt enable for pins GIOB[7:0] */
#define TMS570_GIO_ENACLR_GIOENACLR_1(val) BSP_FLD32(val,8, 15)
#define TMS570_GIO_ENACLR_GIOENACLR_1_GET(reg) BSP_FLD32GET(reg,8, 15)
#define TMS570_GIO_ENACLR_GIOENACLR_1_SET(reg,val) BSP_FLD32SET(reg, val,8, 15)

/* field: GIOENACLR_0 - Interrupt enable for pins GIOA[7:0] */
#define TMS570_GIO_ENACLR_GIOENACLR_0(val) BSP_FLD32(val,0, 7)
#define TMS570_GIO_ENACLR_GIOENACLR_0_GET(reg) BSP_FLD32GET(reg,0, 7)
#define TMS570_GIO_ENACLR_GIOENACLR_0_SET(reg,val) BSP_FLD32SET(reg, val,0, 7)


/*---------------------TMS570_GIO_LVLSET---------------------*/
/* field: GIOLVLSET_3 - GIO high priority interrupt for pins GIOD[7:0]. */
#define TMS570_GIO_LVLSET_GIOLVLSET_3(val) BSP_FLD32(val,24, 31)
#define TMS570_GIO_LVLSET_GIOLVLSET_3_GET(reg) BSP_FLD32GET(reg,24, 31)
#define TMS570_GIO_LVLSET_GIOLVLSET_3_SET(reg,val) BSP_FLD32SET(reg, val,24, 31)

/* field: GIOLVLSET_2 - GIO high priority interrupt for pins GIOC[7:0]. */
#define TMS570_GIO_LVLSET_GIOLVLSET_2(val) BSP_FLD32(val,16, 23)
#define TMS570_GIO_LVLSET_GIOLVLSET_2_GET(reg) BSP_FLD32GET(reg,16, 23)
#define TMS570_GIO_LVLSET_GIOLVLSET_2_SET(reg,val) BSP_FLD32SET(reg, val,16, 23)

/* field: GIOLVLSET_1 - GIO high priority interrupt for pins GIOB[7:0]. */
#define TMS570_GIO_LVLSET_GIOLVLSET_1(val) BSP_FLD32(val,8, 15)
#define TMS570_GIO_LVLSET_GIOLVLSET_1_GET(reg) BSP_FLD32GET(reg,8, 15)
#define TMS570_GIO_LVLSET_GIOLVLSET_1_SET(reg,val) BSP_FLD32SET(reg, val,8, 15)

/* field: GIOLVLSET_0 - GIO high priority interrupt for pins GIOA[7:0]. */
#define TMS570_GIO_LVLSET_GIOLVLSET_0(val) BSP_FLD32(val,0, 7)
#define TMS570_GIO_LVLSET_GIOLVLSET_0_GET(reg) BSP_FLD32GET(reg,0, 7)
#define TMS570_GIO_LVLSET_GIOLVLSET_0_SET(reg,val) BSP_FLD32SET(reg, val,0, 7)


/*---------------------TMS570_GIO_LVLCLR---------------------*/
/* field: GIOLVLCLR_3 - GIO low priority interrupt for pins GIOD[7:0] */
#define TMS570_GIO_LVLCLR_GIOLVLCLR_3(val) BSP_FLD32(val,24, 31)
#define TMS570_GIO_LVLCLR_GIOLVLCLR_3_GET(reg) BSP_FLD32GET(reg,24, 31)
#define TMS570_GIO_LVLCLR_GIOLVLCLR_3_SET(reg,val) BSP_FLD32SET(reg, val,24, 31)

/* field: GIOLVLCLR_2 - GIO low priority interrupt for pins GIOC[7:0] */
#define TMS570_GIO_LVLCLR_GIOLVLCLR_2(val) BSP_FLD32(val,16, 23)
#define TMS570_GIO_LVLCLR_GIOLVLCLR_2_GET(reg) BSP_FLD32GET(reg,16, 23)
#define TMS570_GIO_LVLCLR_GIOLVLCLR_2_SET(reg,val) BSP_FLD32SET(reg, val,16, 23)

/* field: GIOLVLCLR_1 - GIO low priority interrupt for pins GIOB[7:0] */
#define TMS570_GIO_LVLCLR_GIOLVLCLR_1(val) BSP_FLD32(val,8, 15)
#define TMS570_GIO_LVLCLR_GIOLVLCLR_1_GET(reg) BSP_FLD32GET(reg,8, 15)
#define TMS570_GIO_LVLCLR_GIOLVLCLR_1_SET(reg,val) BSP_FLD32SET(reg, val,8, 15)

/* field: GIOLVLCLR_0 - GIO low priority interrupt for pins GIOA[7:0] */
#define TMS570_GIO_LVLCLR_GIOLVLCLR_0(val) BSP_FLD32(val,0, 7)
#define TMS570_GIO_LVLCLR_GIOLVLCLR_0_GET(reg) BSP_FLD32GET(reg,0, 7)
#define TMS570_GIO_LVLCLR_GIOLVLCLR_0_SET(reg,val) BSP_FLD32SET(reg, val,0, 7)


/*-----------------------TMS570_GIO_FLG-----------------------*/
/* field: GIOFLG_3 - GIO flag for pins GIOD[7:0]. */
#define TMS570_GIO_FLG_GIOFLG_3(val) BSP_FLD32(val,24, 31)
#define TMS570_GIO_FLG_GIOFLG_3_GET(reg) BSP_FLD32GET(reg,24, 31)
#define TMS570_GIO_FLG_GIOFLG_3_SET(reg,val) BSP_FLD32SET(reg, val,24, 31)

/* field: GIOFLG_2 - GIO flag for pins GIOC[7:0]. */
#define TMS570_GIO_FLG_GIOFLG_2(val) BSP_FLD32(val,16, 23)
#define TMS570_GIO_FLG_GIOFLG_2_GET(reg) BSP_FLD32GET(reg,16, 23)
#define TMS570_GIO_FLG_GIOFLG_2_SET(reg,val) BSP_FLD32SET(reg, val,16, 23)

/* field: GIOFLG_1 - GIO flag for pins GIOB[7:0]. */
#define TMS570_GIO_FLG_GIOFLG_1(val) BSP_FLD32(val,8, 15)
#define TMS570_GIO_FLG_GIOFLG_1_GET(reg) BSP_FLD32GET(reg,8, 15)
#define TMS570_GIO_FLG_GIOFLG_1_SET(reg,val) BSP_FLD32SET(reg, val,8, 15)

/* field: GIOFLG_0 - GIO flag for pins GIOA[7:0]. */
#define TMS570_GIO_FLG_GIOFLG_0(val) BSP_FLD32(val,0, 7)
#define TMS570_GIO_FLG_GIOFLG_0_GET(reg) BSP_FLD32GET(reg,0, 7)
#define TMS570_GIO_FLG_GIOFLG_0_SET(reg,val) BSP_FLD32SET(reg, val,0, 7)


/*----------------------TMS570_GIO_OFF1----------------------*/
/* field: GIOOFF1 - GIO offset 1. These bits index the currently pending high-priority interrupt. */
#define TMS570_GIO_OFF1_GIOOFF1(val) BSP_FLD32(val,0, 5)
#define TMS570_GIO_OFF1_GIOOFF1_GET(reg) BSP_FLD32GET(reg,0, 5)
#define TMS570_GIO_OFF1_GIOOFF1_SET(reg,val) BSP_FLD32SET(reg, val,0, 5)


/*----------------------TMS570_GIO_OFF2----------------------*/
/* field: GIOOFF2 - GIO offset 2. These bits index the currently pending low-priority interrupt. */
#define TMS570_GIO_OFF2_GIOOFF2(val) BSP_FLD32(val,0, 5)
#define TMS570_GIO_OFF2_GIOOFF2_GET(reg) BSP_FLD32GET(reg,0, 5)
#define TMS570_GIO_OFF2_GIOOFF2_SET(reg,val) BSP_FLD32SET(reg, val,0, 5)


/*----------------------TMS570_GIO_EMU1----------------------*/
/* field: GIOEMU1 - GIO offset emulation 1. These bits index the currently pending high-priority interrupt. */
#define TMS570_GIO_EMU1_GIOEMU1(val) BSP_FLD32(val,0, 5)
#define TMS570_GIO_EMU1_GIOEMU1_GET(reg) BSP_FLD32GET(reg,0, 5)
#define TMS570_GIO_EMU1_GIOEMU1_SET(reg,val) BSP_FLD32SET(reg, val,0, 5)


/*----------------------TMS570_GIO_EMU2----------------------*/
/* field: GIOEMU2 - GIO offset emulation 2. These bits index the currently pending low-priority interrupt. */
#define TMS570_GIO_EMU2_GIOEMU2(val) BSP_FLD32(val,0, 5)
#define TMS570_GIO_EMU2_GIOEMU2_GET(reg) BSP_FLD32GET(reg,0, 5)
#define TMS570_GIO_EMU2_GIOEMU2_SET(reg,val) BSP_FLD32SET(reg, val,0, 5)


/*----------------------TMS570_GIO_ports----------------------*/
/* field: GIOEMU2 - GIO offset emulation 2. These bits index the currently pending low-priority interrupt. */
#define TMS570_GIO_ports_GIOEMU2(val) BSP_FLD32(val,0, 5)
#define TMS570_GIO_ports_GIOEMU2_GET(reg) BSP_FLD32GET(reg,0, 5)
#define TMS570_GIO_ports_GIOEMU2_SET(reg,val) BSP_FLD32SET(reg, val,0, 5)



#endif /* LIBBSP_ARM_TMS570_GIO */
