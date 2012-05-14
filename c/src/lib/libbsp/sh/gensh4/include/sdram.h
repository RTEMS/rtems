/*
 * SDRAM Mode Register
 * Based on Fujitsu MB81F643242B data sheet.
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __SDRAM_H__
#define __SDRAM_H__

/* SDRAM Mode Register */
#define SDRAM_MODE_BL         0x0007  /* Burst Length: */
#define SDRAM_MODE_BL_1       0x0000  /*    0 */
#define SDRAM_MODE_BL_2       0x0001  /*    2 */
#define SDRAM_MODE_BL_4       0x0002  /*    4 */
#define SDRAM_MODE_BL_8       0x0003  /*    8 */
#define SDRAM_MODE_BL_16      0x0004  /*    16 */
#define SDRAM_MODE_BL_32      0x0005  /*    32 */
#define SDRAM_MODE_BL_64      0x0006  /*    64 */
#define SDRAM_MODE_BL_FULL    0x0007  /*    Full column */

#define SDRAM_MODE_BT         0x0008  /* Burst Type: */
#define SDRAM_MODE_BT_SEQ     0x0000  /*   Sequential */
#define SDRAM_MODE_BT_ILV     0x0008  /*   Interleave */

#define SDRAM_MODE_CL         0x0070  /* CAS Latency: */
#define SDRAM_MODE_CL_1       0x0010  /*    1 */
#define SDRAM_MODE_CL_2       0x0020  /*    2 */
#define SDRAM_MODE_CL_3       0x0030  /*    3 */

#define SDRAM_MODE_OPC        0x0200  /* Opcode: */
#define SDRAM_MODE_OPC_BRBW   0x0000  /*     Burst read & Burst write */
#define SDRAM_MODE_OPC_BRSW   0x0200  /*     Burst read & Single write */

#endif
