/**
 *  @file gba_registers.h
 *
 *  Game Boy Advance registers.
 *
 *  This include file contains definitions related to the ARM BSP.
 */
/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) 2004  Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/*---------------------------------------------------------------------------+
| THIS CODE WAS NOT MADE IN ASSOCIATION WITH NINTENDO AND DOES NOT MAKE
| USE OF ANY INTELLECTUAL PROPERTY CLAIMED BY NINTENDO.
|
| GAMEBOY ADVANCE IS A TRADEMARK OF NINTENDO.
|
| THIS CODE HAS BEEN PROVIDED "AS-IS" WITHOUT A WARRANTY OF ANY KIND,
| EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO IMPLIED
| WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.
| THE ENTIRE RISK AS TO THE QUALITY OR PERFORMANCE OF THE CODE IS WITH YOU.
|
| IN NO EVENT, UNLESS AGREED TO IN WRITING, WILL ANY COPYRIGHT HOLDER,
| OR ANY OTHER PARTY, BE HELD LIABLE FOR ANY DAMAGES RESULTING FROM
| THE USE OR INABILITY TO USE THIS CODE.
+----------------------------------------------------------------------------*/

#ifndef _GBA_REGISTERS_H
#define _GBA_REGISTERS_H

#ifndef __asm__
extern volatile unsigned int  *Regs;        /**< Chip registers              */
#endif


/*---------------------------------------------------------------------------+
|    General address definitions
+----------------------------------------------------------------------------*/
/* General Internal Memory */
#define GBA_BIOS_ADDR        0x00000000  /**< GBA BIOS start address         */
#define GBA_BIOS_END         0x00004000  /**< GBA BIOS end address           */
#define GBA_EXT_RAM_ADDR     0x02000000  /**< On-board RAM start address     */
#define GBA_EXT_RAM_END      0x02040000  /**< On-board RAM end address       */
#define GBA_INT_RAM_ADDR     0x03000000  /**< In-chip RAM start address      */
#define GBA_INT_RAM_END      0x03008000  /**< In-chip RAM end address        */
#define GBA_IO_REGS_ADDR     0x04000000  /**< IO registers start address     */
#define GBA_IO_REGS_END      0x04000400  /**< IO registers end address       */
/* Internal Display Memory */
#define GBA_PAL_RAM_ADDR     0x05000000  /**< PAL RAM start address          */
#define GBA_PAL_RAM_END      0x05000400  /**< PAL RAM end address            */
#define GBA_VRAM_ADDR        0x06000000  /**< VRAM start address             */
#define GBA_VRAM_END         0x06180000  /**< VRAM end address               */
#define GBA_OAM_ADDR         0x07000000  /**< OAM start address              */
#define GBA_OAM_END          0x07000400  /**< OAM end address                */
/* External Memory (Game Pak) */
#define GBA_ROM0_ADDR        0x08000000  /**< Card ROM0 start address        */
#define GBA_ROM1_ADDR        0x0A000000  /**< Card ROM1 start address        */
#define GBA_ROM2_ADDR        0x0C000000  /**< Card ROM2 start address        */
#define GBA_SRAM_ADDR        0x0E000000  /**< Card SRAM start address        */
#define GBA_SRAM_END         0x0E010000  /**< Card SRAM end address          */
/* Unused Memory Area */
#define GBA_MAX_ADDR         0x10000000  /**< Upper 4bits of address bus unused */
/* Memory pointers */
#ifndef __asm__
#define GBA_BASE_BIOS       (volatile unsigned char *)GBA_BIOS_ADDR    /**< BIOS - System ROM 16KBytes, protected */
#define GBA_BASE_EXT_RAM    (volatile unsigned char *)GBA_EXT_RAM_ADDR /**< WRAM - On-board Work RAM 256KBytes    */
#define GBA_BASE_INT_RAM    (volatile unsigned char *)GBA_INT_RAM_ADDR /**< WRAM - In-chip Work RAM 32KBytes      */
#define GBA_BASE_IO_REGS    (volatile unsigned char *)GBA_IO_REGS_ADDR /**< I/O Registers                         */
#define GBA_BASE_PAL_RAM    (volatile unsigned char *)GBA_PAL_RAM_ADDR /**< BG/OBJ Palette RAM 1KBytes            */
#define GBA_BASE_VRAM       (volatile unsigned char *)GBA_VRAM_ADDR    /**< VRAM - Video RAM 96KBytes             */
#define GBA_BASE_OAM        (volatile unsigned char *)GBA_OAM_ADDR     /**< OAM - OBJ Attribytes                  */
#define GBA_BASE_ROM0       (volatile unsigned char *)GBA_ROM0_ADDR    /**< Card ROM 32MB                         */
#define GBA_BASE_ROM1       (volatile unsigned char *)GBA_ROM1_ADDR    /**< Card ROM 32MB                         */
#define GBA_BASE_ROM2       (volatile unsigned char *)GBA_ROM2_ADDR    /**< Card ROM 32MB                         */
#define GBA_BASE_SRAM       (volatile unsigned char *)GBA_SRAM_ADDR    /**< Card SRAM 64KBytes                    */
#endif
/*---------------------------------------------------------------------------*
 *   Display Control
 *---------------------------------------------------------------------------*/
#define GBA_DISP_BG_MODE_MASK    0x0007      /**< BG Mode       */
#define GBA_DISP_ON_MASK         0x1f00      /**< OBJ BG ON     */
#define GBA_DISP_WIN_MASK        0x6000      /**< Window ON     */

#define GBA_DISP_BG_MODE_SHIFT   0
#define GBA_DISP_ON_SHIFT        8
#define GBA_DISP_WIN_SHIFT       13

#define GBA_DISP_MODE_0          0x0000      /**< BG Mode 0     */
#define GBA_DISP_MODE_1          0x0001      /**< BG Mode 1     */
#define GBA_DISP_MODE_2          0x0002      /**< BG Mode 2     */
#define GBA_DISP_MODE_3          0x0003      /**< BG Mode 3     */
#define GBA_DISP_MODE_4          0x0004      /**< BG Mode 4     */
#define GBA_DISP_MODE_5          0x0005      /**< BG Mode 5     */
#define GBA_DISP_BMP_FRAME_NO    0x0010      /**< Bitmap Mode Display Frame     */
#define GBA_DISP_OBJ_HOFF        0x0020      /**< OBJ Processing in H Blank OFF */
#define GBA_DISP_OBJ_CHAR_2D_MAP 0x0000      /**< OBJ Character Data 2D Mapping */
#define GBA_DISP_OBJ_CHAR_1D_MAP 0x0040      /**< OBJ Character Data 1D Mapping */
#define GBA_DISP_LCDC_OFF        0x0080      /**< LCDC OFF       */
#define GBA_DISP_BG0_ON          0x0100      /**< BG0 ON         */
#define GBA_DISP_BG1_ON          0x0200      /**< BG1 ON         */
#define GBA_DISP_BG2_ON          0x0400      /**< BG2 ON         */
#define GBA_DISP_BG3_ON          0x0800      /**< BG3 ON         */
#define GBA_DISP_BG_ALL_ON       0x0f00      /**< All BG ON      */
#define GBA_DISP_OBJ_ON          0x1000      /**< OBJ ON         */
#define GBA_DISP_OBJ_BG_ALL_ON   0x1f00      /**< All OBJ/BG ON  */
#define GBA_DISP_WIN0_ON         0x2000      /**< Window 0 ON    */
#define GBA_DISP_WIN1_ON         0x4000      /**< Window 1 ON    */
#define GBA_DISP_WIN01_ON        0x6000      /**< Window 0,1 ON  */
#define GBA_DISP_OBJWIN_ON       0x8000      /**< OBJ Window ON  */
#define GBA_DISP_WIN_ALL_ON      0xe000      /**< All Window ON  */
#define GBA_DISP_ALL_ON          0x7f00      /**< All ON         */

#define GBA_BG_MODE_0            0
#define GBA_BG_MODE_1            1
#define GBA_BG_MODE_2            2
#define GBA_BG_MODE_3            3
#define GBA_BG_MODE_4            4
#define GBA_BG_MODE_5            5

#define GBA_BG0_ENABLE           1
#define GBA_BG1_ENABLE           2
#define GBA_BG2_ENABLE           4
#define GBA_BG3_ENABLE           8
#define GBA_OBJ_ENABLE          16

#define GBA_OBJ_1D_MAP           1
#define GBA_OBJ_2D_MAP           0


/*---------------------------------------------------------------------------+
|    LCD
+----------------------------------------------------------------------------*/
/* LCD I/O Register address offsets */
#define GBA_DISPCNT             0x00000000   /**< LCD Control                */
#define GBA_DISPSTAT            0x00000004   /**< General LCD Status (STAT,LYC) */
#define GBA_VCOUNT              0x00000006   /**< Vertical Counter (LY)      */
#define GBA_BG0CNT              0x00000008   /**< BG0 Control                */
#define GBA_BG1CNT              0x0000000A   /**< BG1 Control                */
#define GBA_BG2CNT              0x0000000C   /**< BG2 Control                */
#define GBA_BG3CNT              0x0000000E   /**< BG3 Control                */
#define GBA_BG0HOFS             0x00000010   /**< BG0 X-Offset               */
#define GBA_BG0VOFS             0x00000012   /**< BG0 Y-Offset               */
#define GBA_BG1HOFS             0x00000014   /**< BG1 X-Offset               */
#define GBA_BG1VOFS             0x00000016   /**< BG1 Y-Offset               */
#define GBA_BG2HOFS             0x00000018   /**< BG2 X-Offset               */
#define GBA_BG2VOFS             0x0000001A   /**< BG2 Y-Offset               */
#define GBA_BG3HOFS             0x0000001C   /**< BG3 X-Offset               */
#define GBA_BG3VOFS             0x0000001E   /**< BG3 Y-Offset               */
#define GBA_BG2PA               0x00000020   /**< BG2 Rotation/Scaling Parameter A (dx)  */
#define GBA_BG2PB               0x00000022   /**< BG2 Rotation/Scaling Parameter B (dmx) */
#define GBA_BG2PC               0x00000024   /**< BG2 Rotation/Scaling Parameter C (dy)  */
#define GBA_BG2PD               0x00000026   /**< BG2 Rotation/Scaling Parameter D (dmy) */
#define GBA_BG2X                0x00000028   /**< BG2 Reference Point X-Coordinate       */
#define GBA_BG2X_L              0x00000028   /**< BG2 Reference Point X-Coordinate low   */
#define GBA_BG2X_H              0x0000002A   /**< BG2 Reference Point X-Coordinate high  */
#define GBA_BG2Y                0x0000002C   /**< BG2 Reference Point Y-Coordinate       */
#define GBA_BG2Y_L              0x0000002C   /**< BG2 Reference Point Y-Coordinate low   */
#define GBA_BG2Y_H              0x0000002E   /**< BG2 Reference Point Y-Coordinate high  */
#define GBA_BG3PA               0x00000030   /**< BG3 Rotation/Scaling Parameter A (dx)  */
#define GBA_BG3PB               0x00000032   /**< BG3 Rotation/Scaling Parameter B (dmx) */
#define GBA_BG3PC               0x00000034   /**< BG3 Rotation/Scaling Parameter C (dy)  */
#define GBA_BG3PD               0x00000036   /**< BG3 Rotation/Scaling Parameter D (dmy) */
#define GBA_BG3X                0x00000038   /**< BG3 Reference Point X-Coordinate       */
#define GBA_BG3X_L              0x00000038   /**< BG3 Reference Point X-Coordinate low   */
#define GBA_BG3X_H              0x0000003A   /**< BG3 Reference Point X-Coordinate high  */
#define GBA_BG3Y                0x0000003C   /**< BG3 Reference Point Y-Coordinate       */
#define GBA_BG3Y_L              0x0000003C   /**< BG3 Reference Point Y-Coordinate low   */
#define GBA_BG3Y_H              0x0000003E   /**< BG3 Reference Point Y-Coordinate hugh  */
#define GBA_WIN0H               0x00000040   /**< Window 0 Horizontal Dimensions         */
#define GBA_WIN1H               0x00000042   /**< Window 1 Horizontal Dimensions         */
#define GBA_WIN0V               0x00000044   /**< Window 0 Vertical Dimensions           */
#define GBA_WIN1V               0x00000046   /**< Window 1 Vertical Dimensions           */
#define GBA_WININ               0x00000048   /**< Control Inside of Window(s)            */
#define GBA_WINOUT              0x0000004A   /**< Control Outside of Windows & Inside of OBJ Window */
#define GBA_MOSAIC              0x0000004C   /**< Mosaic Size                            */
#define GBA_BLDCNT              0x00000050   /**< Color Special Effects Selection        */
#define GBA_BLDMOD              0x00000050   /**< Color Special Effects Selection       X*/
#define GBA_BLDALPHA            0x00000052   /**< Alpha Blending Coefficients            */
#define GBA_COLEV               0x00000052   /**< Alpha Blending Coefficients           X*/
#define GBA_BLDY                0x00000054   /**< Brightness (Fade-In/Out) Coefficient   */
#define GBA_COLY                0x00000054   /**< Brightness (Fade-In/Out) Coefficient  X*/
/* LCD I/O Register addresses */
#define GBA_REG_DISPCNT_ADDR    GBA_IO_REGS_ADDR + GBA_DISPCNT
#define GBA_REG_DISPSTAT_ADDR   GBA_IO_REGS_ADDR + GBA_DISPSTAT
#define GBA_REG_VCOUNT_ADDR     GBA_IO_REGS_ADDR + GBA_VCOUNT
#define GBA_REG_BG0CNT_ADDR     GBA_IO_REGS_ADDR + GBA_BG0CNT
#define GBA_REG_BG1CNT_ADDR     GBA_IO_REGS_ADDR + GBA_BG1CNT
#define GBA_REG_BG2CNT_ADDR     GBA_IO_REGS_ADDR + GBA_BG2CNT
#define GBA_REG_BG3CNT_ADDR     GBA_IO_REGS_ADDR + GBA_BG3CNT
#define GBA_REG_BG0HOFS_ADDR    GBA_IO_REGS_ADDR + GBA_BG0HOFS
#define GBA_REG_BG0VOFS_ADDR    GBA_IO_REGS_ADDR + GBA_BG0VOFS
#define GBA_REG_BG1HOFS_ADDR    GBA_IO_REGS_ADDR + GBA_BG1HOFS
#define GBA_REG_BG1VOFS_ADDR    GBA_IO_REGS_ADDR + GBA_BG1VOFS
#define GBA_REG_BG2HOFS_ADDR    GBA_IO_REGS_ADDR + GBA_BG2HOFS
#define GBA_REG_BG2VOFS_ADDR    GBA_IO_REGS_ADDR + GBA_BG2VOFS
#define GBA_REG_BG3HOFS_ADDR    GBA_IO_REGS_ADDR + GBA_BG3HOFS
#define GBA_REG_BG3VOFS_ADDR    GBA_IO_REGS_ADDR + GBA_BG3VOFS
#define GBA_REG_BG2PA_ADDR      GBA_IO_REGS_ADDR + GBA_BG2PA
#define GBA_REG_BG2PB_ADDR      GBA_IO_REGS_ADDR + GBA_BG2PB
#define GBA_REG_BG2PC_ADDR      GBA_IO_REGS_ADDR + GBA_BG2PC
#define GBA_REG_BG2PD_ADDR      GBA_IO_REGS_ADDR + GBA_BG2PD
#define GBA_REG_BG2X_ADDR       GBA_IO_REGS_ADDR + GBA_BG2X
#define GBA_REG_BG2X_L_ADDR     GBA_IO_REGS_ADDR + GBA_BG2X_L
#define GBA_REG_BG2X_H_ADDR     GBA_IO_REGS_ADDR + GBA_BG2X_H
#define GBA_REG_BG2Y_ADDR       GBA_IO_REGS_ADDR + GBA_BG2Y
#define GBA_REG_BG2Y_L_ADDR     GBA_IO_REGS_ADDR + GBA_BG2Y_L
#define GBA_REG_BG2Y_H_ADDR     GBA_IO_REGS_ADDR + GBA_BG2Y_H
#define GBA_REG_BG3PA_ADDR      GBA_IO_REGS_ADDR + GBA_BG3PA
#define GBA_REG_BG3PB_ADDR      GBA_IO_REGS_ADDR + GBA_BG3PB
#define GBA_REG_BG3PC_ADDR      GBA_IO_REGS_ADDR + GBA_BG3PC
#define GBA_REG_BG3PD_ADDR      GBA_IO_REGS_ADDR + GBA_BG3PD
#define GBA_REG_BG3X_ADDR       GBA_IO_REGS_ADDR + GBA_BG3X
#define GBA_REG_BG3X_L_ADDR     GBA_IO_REGS_ADDR + GBA_BG3X_L
#define GBA_REG_BG3X_H_ADDR     GBA_IO_REGS_ADDR + GBA_BG3X_H
#define GBA_REG_BG3Y_ADDR       GBA_IO_REGS_ADDR + GBA_BG3Y
#define GBA_REG_BG3Y_L_ADDR     GBA_IO_REGS_ADDR + GBA_BG3Y_L
#define GBA_REG_BG3Y_H_ADDR     GBA_IO_REGS_ADDR + GBA_BG3Y_H
#define GBA_REG_WIN0H_ADDR      GBA_IO_REGS_ADDR + GBA_WIN0H
#define GBA_REG_WIN1H_ADDR      GBA_IO_REGS_ADDR + GBA_WIN1H
#define GBA_REG_WIN0V_ADDR      GBA_IO_REGS_ADDR + GBA_WIN0V
#define GBA_REG_WIN1V_ADDR      GBA_IO_REGS_ADDR + GBA_WIN1V
#define GBA_REG_WININ_ADDR      GBA_IO_REGS_ADDR + GBA_WININ
#define GBA_REG_WINOUT_ADDR     GBA_IO_REGS_ADDR + GBA_WINOUT
#define GBA_REG_MOSAIC_ADDR     GBA_IO_REGS_ADDR + GBA_MOSAIC
#define GBA_REG_BLDCNT_ADDR     GBA_IO_REGS_ADDR + GBA_BLDCNT
#define GBA_REG_BLDMOD_ADDR     GBA_IO_REGS_ADDR + GBA_BLDMOD
#define GBA_REG_BLDALPHA_ADDR   GBA_IO_REGS_ADDR + GBA_BLDALPHA
#define GBA_REG_COLEV_ADDR      GBA_IO_REGS_ADDR + GBA_COLEV
#define GBA_REG_BLDY_ADDR       GBA_IO_REGS_ADDR + GBA_BLDY
#define GBA_REG_COLY_ADDR       GBA_IO_REGS_ADDR + GBA_COLY
/* LCD I/O Registers */
#ifndef __asm__
#define GBA_REG_DISPCNT         (*(volatile unsigned short *)(GBA_REG_DISPCNT_ADDR))
#define GBA_REG_DISPSTAT        (*(volatile unsigned short *)(GBA_REG_DISPSTAT_ADDR))
#define GBA_REG_VCOUNT          (*(volatile unsigned short *)(GBA_REG_VCOUNT_ADDR))
#define GBA_REG_BG0CNT          (*(volatile unsigned short *)(GBA_REG_BG0CNT_ADDR))
#define GBA_REG_BG1CNT          (*(volatile unsigned short *)(GBA_REG_BG1CNT_ADDR))
#define GBA_REG_BG2CNT          (*(volatile unsigned short *)(GBA_REG_BG2CNT_ADDR))
#define GBA_REG_BG3CNT          (*(volatile unsigned short *)(GBA_REG_BG3CNT_ADDR))
#define GBA_REG_BG0HOFS         (*(volatile unsigned short *)(GBA_REG_BG0HOFS_ADDR))
#define GBA_REG_BG0VOFS         (*(volatile unsigned short *)(GBA_REG_BG0VOFS_ADDR))
#define GBA_REG_BG1HOFS         (*(volatile unsigned short *)(GBA_REG_BG1HOFS_ADDR))
#define GBA_REG_BG1VOFS         (*(volatile unsigned short *)(GBA_REG_BG1VOFS_ADDR))
#define GBA_REG_BG2HOFS         (*(volatile unsigned short *)(GBA_REG_BG2HOFS_ADDR))
#define GBA_REG_BG2VOFS         (*(volatile unsigned short *)(GBA_REG_BG2VOFS_ADDR))
#define GBA_REG_BG3HOFS         (*(volatile unsigned short *)(GBA_REG_BG3HOFS_ADDR))
#define GBA_REG_BG3VOFS         (*(volatile unsigned short *)(GBA_REG_BG3VOFS_ADDR))
#define GBA_REG_BG2PA           (*(volatile unsigned short *)(GBA_REG_BG2PA_ADDR))
#define GBA_REG_BG2PB           (*(volatile unsigned short *)(GBA_REG_BG2PB_ADDR))
#define GBA_REG_BG2PC           (*(volatile unsigned short *)(GBA_REG_BG2PC_ADDR))
#define GBA_REG_BG2PD           (*(volatile unsigned short *)(GBA_REG_BG2PD_ADDR))
#define GBA_REG_BG2X            (*(volatile unsigned int   *)(GBA_REG_BG2X_ADDR))
#define GBA_REG_BG2X_L          (*(volatile unsigned short *)(GBA_REG_BG2X_L_ADDR))
#define GBA_REG_BG2X_H          (*(volatile unsigned short *)(GBA_REG_BG2X_H_ADDR))
#define GBA_REG_BG2Y            (*(volatile unsigned int   *)(GBA_REG_BG2Y_ADDR))
#define GBA_REG_BG2Y_L          (*(volatile unsigned short *)(GBA_REG_BG2Y_L_ADDR))
#define GBA_REG_BG2Y_H          (*(volatile unsigned short *)(GBA_REG_BG2Y_H_ADDR))
#define GBA_REG_BG3PA           (*(volatile unsigned short *)(GBA_REG_BG3PA_ADDR))
#define GBA_REG_BG3PB           (*(volatile unsigned short *)(GBA_REG_BG3PB_ADDR))
#define GBA_REG_BG3PC           (*(volatile unsigned short *)(GBA_REG_BG3PC_ADDR))
#define GBA_REG_BG3PD           (*(volatile unsigned short *)(GBA_REG_BG3PD_ADDR))
#define GBA_REG_BG3X            (*(volatile unsigned int   *)(GBA_REG_BG3X_ADDR))
#define GBA_REG_BG3X_L          (*(volatile unsigned short *)(GBA_REG_BG3X_L_ADDR))
#define GBA_REG_BG3X_H          (*(volatile unsigned short *)(GBA_REG_BG3X_H_ADDR))
#define GBA_REG_BG3Y            (*(volatile unsigned int   *)(GBA_REG_BG3Y_ADDR))
#define GBA_REG_BG3Y_L          (*(volatile unsigned short *)(GBA_REG_BG3Y_L_ADDR))
#define GBA_REG_BG3Y_H          (*(volatile unsigned short *)(GBA_REG_BG3Y_H_ADDR))
#define GBA_REG_WIN0H           (*(volatile unsigned short *)(GBA_REG_WIN0H_ADDR))
#define GBA_REG_WIN1H           (*(volatile unsigned short *)(GBA_REG_WIN1H_ADDR))
#define GBA_REG_WIN0V           (*(volatile unsigned short *)(GBA_REG_WIN0V_ADDR))
#define GBA_REG_WIN1V           (*(volatile unsigned short *)(GBA_REG_WIN1V_ADDR))
#define GBA_REG_WININ           (*(volatile unsigned short *)(GBA_REG_WININ_ADDR))
#define GBA_REG_WINOUT          (*(volatile unsigned short *)(GBA_REG_WINOUT_ADDR))
#define GBA_REG_MOSAIC          (*(volatile unsigned short *)(GBA_REG_MOSAIC_ADDR))
#define GBA_REG_BLDCNT          (*(volatile unsigned short *)(GBA_REG_BLDCNT_ADDR))
#define GBA_REG_BLDMOD          (*(volatile unsigned short *)(GBA_REG_BLDMOD_ADDR))
#define GBA_REG_BLDALPHA        (*(volatile unsigned short *)(GBA_REG_BLDALPHA_ADDR))
#define GBA_REG_COLEV           (*(volatile unsigned short *)(GBA_REG_COLEV_ADDR))
#define GBA_REG_BLDY            (*(volatile unsigned short *)(GBA_REG_BLDY_ADDR))
#define GBA_REG_COLY            (*(volatile unsigned short *)(GBA_REG_COLY_ADDR))
#endif
/*---------------------------------------------------------------------------+
|    SOUND
+----------------------------------------------------------------------------*/
#define GBA_SOUND_INIT            0x8000  /**< makes the sound restart       */
#define GBA_SOUND_DUTY87          0x0000  /**< 87.5% wave duty               */
#define GBA_SOUND_DUTY75          0x0040  /**< 75% wave duty                 */
#define GBA_SOUND_DUTY50          0x0080  /**< 50% wave duty                 */
#define GBA_SOUND_DUTY25          0x00C0  /**< 25% wave duty                 */

#define GBA_SOUND1_PLAYONCE       0x4000  /**< play sound once               */
#define GBA_SOUND1_PLAYLOOP       0x0000  /**< play sound looped             */
#define GBA_SOUND1_INIT           0x8000  /**< makes the sound restart       */
#define GBA_SOUND1_SWEEPSHIFTS(n)(n)      /**< number of sweep shifts (0-7)  */
#define GBA_SOUND1_SWEEPINC       0x0000  /**< sweep add (freq increase)     */
#define GBA_SOUND1_SWEEPDEC       0x0008  /**< sweep dec (freq decrese)      */
#define GBA_SOUND1_SWEEPTIME(n)  (n<<4)   /**< time of sweep (0-7)           */
#define GBA_SOUND1_ENVSTEPS(n)   (n<<8)   /**< envelope steps (0-7)          */
#define GBA_SOUND1_ENVINC         0x0800  /**< envelope increase             */
#define GBA_SOUND1_ENVDEC         0x0000  /**< envelope decrease             */
#define GBA_SOUND1_ENVINIT(n)    (n<<12)  /**< initial envelope volume (0-15)*/

#define GBA_SOUND2_PLAYONCE       0x4000  /**< play sound once               */
#define GBA_SOUND2_PLAYLOOP       0x0000  /**< play sound looped             */
#define GBA_SOUND2_INIT           0x8000  /**< makes the sound restart       */
#define GBA_SOUND2_ENVSTEPS(n)   (n<<8)   /**< envelope steps (0-7)          */
#define GBA_SOUND2_ENVINC         0x0800  /**< envelope increase             */
#define GBA_SOUND2_ENVDEC         0x0000  /**< envelope decrease             */
#define GBA_SOUND2_ENVINIT(n)    (n<<12)  /**< initial envelope volume (0-15)*/

#define GBA_SOUND3_BANK32         0x0000  /**< Use two banks of 32 steps each*/
#define GBA_SOUND3_BANK64         0x0020  /**< Use one bank of 64 steps      */
#define GBA_SOUND3_SETBANK0       0x0000  /**< Bank to play 0 or 1 (non set bank is written to) */
#define GBA_SOUND3_SETBANK1       0x0040
#define GBA_SOUND3_PLAY           0x0080  /**< Output sound                  */

#define GBA_SOUND3_OUTPUT0        0x0000  /**< Mute output                   */
#define GBA_SOUND3_OUTPUT1        0x2000  /**< Output unmodified             */
#define GBA_SOUND3_OUTPUT12       0x4000  /**< Output 1/2                    */
#define GBA_SOUND3_OUTPUT14       0x6000  /**< Output 1/4                    */
#define GBA_SOUND3_OUTPUT34       0x8000  /**< Output 3/4                    */

#define GBA_SOUND3_PLAYONCE       0x4000  /**< Play sound once               */
#define GBA_SOUND3_PLAYLOOP       0x0000  /**< Play sound looped             */
#define GBA_SOUND3_INIT           0x8000  /**< Makes the sound restart       */

#define GBA_SOUND4_PLAYONCE       0x4000  /**< play sound once               */
#define GBA_SOUND4_PLAYLOOP       0x0000  /**< play sound looped             */
#define GBA_SOUND4_INIT           0x8000  /**< makes the sound restart       */
#define GBA_SOUND4_ENVSTEPS(n)   (n<<8)   /**< envelope steps (0-7)          */
#define GBA_SOUND4_ENVINC         0x0800  /**< envelope increase             */
#define GBA_SOUND4_ENVDEC         0x0000  /**< envelope decrease             */
#define GBA_SOUND4_ENVINIT(n)    (n<<12)  /**< initial envelope volume (0-15)*/

#define GBA_SOUND4_STEPS7         0x0004
#define GBA_SOUND4_STEPS15        0x0000
#define GBA_SOUND4_PLAYONCE       0x4000
#define GBA_SOUND4_PLAYLOOP       0x0000
#define GBA_SOUND4_INIT           0x8000

/* Sound Register address offsets */
#define GBA_SOUND1CNT_L         0x00000060    /**< Channel 1 sweep           */
#define GBA_SG10                0x00000060    /**< Channel 1 sweep low      X*/
#define GBA_SG10_L              0x00000060    /**< Channel 1 sweep high     X*/
#define GBA_SOUND1CNT_H         0x00000062    /**< Channel 1 Duty/Len/Env    */
#define GBA_SG10_H              0x00000062    /**< Channel 1 Duty/Len/Env   X*/
#define GBA_SOUND1CNT_X         0x00000064    /**< Channel 1 Freq/Control    */
#define GBA_SG11                0x00000064    /**< Channel 1 Freq/Control   X*/
#define GBA_SOUND2CNT_L         0x00000068    /**< Channel 2 Duty/Len/Env    */
#define GBA_SG20                0x00000068    /**< Channel 2 Duty/Len/Env   X*/
#define GBA_SOUND2CNT_H         0x0000006C    /**< Channel 2 Freq/Control    */
#define GBA_SG21                0x0000006C    /**< Channel 2 Freq/Control   X*/
#define GBA_SOUND3CNT_L         0x00000070    /**< Channel 3 Stop/Wave RAM   */
#define GBA_SG30                0x00000070    /**< Channel 3 Stop/Wave RAM  X*/
#define GBA_SG30_L              0x00000070    /**< Channel 3 Stop/Wave RAM  X*/
#define GBA_SOUND3CNT_H         0x00000072    /**< Channel 3 Len/Vol         */
#define GBA_SG30_H              0x00000072    /**< Channel 3 Len/Vol        X*/
#define GBA_SOUND3CNT_X         0x00000074    /**< Channel 3 Freq/Control    */
#define GBA_SG31                0x00000074    /**< Channel 3 Freq/Control   X*/
#define GBA_SOUND4CNT_L         0x00000078    /**< Channel 4 Len/Env         */
#define GBA_SG40                0x00000078    /**< Channel 4 Len/Env        X*/
#define GBA_SOUND4CNT_H         0x0000007C    /**< Channel 4 Freq/Control    */
#define GBA_SG41                0x0000007C    /**< Channel 4 Freq/Control   X*/
#define GBA_SOUNDCNT_L          0x00000080    /**< Control LR/Vol/Enable     */
#define GBA_SGCNT0_L            0x00000080    /**< Control LR/Vol/Enable    X*/
#define GBA_SOUNDCNT_H          0x00000082    /**< Control Mixing/DMA        */
#define GBA_SGCNT0_H            0x00000082    /**< Control Mixing/DMA       X*/
#define GBA_SOUNDCNT_X          0x00000084    /**< Control Sound on/off      */
#define GBA_SGCNT1              0x00000084    /**< Control Sound on/off     X*/
#define GBA_SOUNDBIAS           0x00000088    /**< Sound PWM control         */
#define GBA_SGBIAS              0x00000088    /**< Sound PWM control        X*/
#define GBA_SGWR0               0x00000090    /**< Ch3 Wave Pattern RAM     X*/
#define GBA_WAVE_RAM0_L         0x00000090    /**< Ch3 Wave Pattern RAM      */
#define GBA_SGWR0_L             0x00000090    /**< Ch3 Wave Pattern RAM     X*/
#define GBA_WAVE_RAM0_H         0x00000092    /**< Ch3 Wave Pattern RAM      */
#define GBA_SGWR0_H             0x00000092    /**< Ch3 Wave Pattern RAM     X*/
#define GBA_SGWR1               0x00000094    /**< Ch3 Wave Pattern RAM     X*/
#define GBA_WAVE_RAM1_L         0x00000094    /**< Ch3 Wave Pattern RAM      */
#define GBA_SGWR1_L             0x00000094    /**< Ch3 Wave Pattern RAM     X*/
#define GBA_WAVE_RAM1_H         0x00000096    /**< Ch3 Wave Pattern RAM      */
#define GBA_SGWR1_H             0x00000096    /**< Ch3 Wave Pattern RAM     X*/
#define GBA_SGWR2               0x00000098    /**< Ch3 Wave Pattern RAM     X*/
#define GBA_WAVE_RAM2_L         0x00000098    /**< Ch3 Wave Pattern RAM      */
#define GBA_SGWR2_L             0x00000098    /**< Ch3 Wave Pattern RAM     X*/
#define GBA_WAVE_RAM2_H         0x0000009A    /**< Ch3 Wave Pattern RAM      */
#define GBA_SGWR2_H             0x0000009A    /**< Ch3 Wave Pattern RAM     X*/
#define GBA_SGWR3               0x0000009C    /**< Ch3 Wave Pattern RAM     X*/
#define GBA_WAVE_RAM3_L         0x0000009C    /**< Ch3 Wave Pattern RAM      */
#define GBA_SGWR3_L             0x0000009C    /**< Ch3 Wave Pattern RAM     X*/
#define GBA_WAVE_RAM3_H         0x0000009E    /**< Ch3 Wave Pattern RAM      */
#define GBA_SGWR3_H             0x0000009E    /**< Ch3 Wave Pattern RAM     X*/
#define GBA_SGFIF0A             0x000000A0    /**< Sound A FIFO              X*/
#define GBA_FIFO_A_L            0x000000A0    /**< Sound A FIFO               */
#define GBA_SGFIFOA_L           0x000000A0    /**< Sound A FIFO              X*/
#define GBA_FIFO_A_H            0x000000A2    /**< Sound A FIFO               */
#define GBA_SGFIFOA_H           0x000000A2    /**< Sound A FIFO              X*/
#define GBA_SGFIFOB             0x000000A4    /**< Sound B FIFO              X*/
#define GBA_FIFO_B_L            0x000000A4    /**< Sound B FIFO               */
#define GBA_SGFIFOB_L           0x000000A4    /**< Sound B FIFO              X*/
#define GBA_FIFO_B_H            0x000000A6    /**< Sound B FIFO               */
#define GBA_SGFIFOB_H           0x000000A6    /**< Sound B FIFO              X*/
/* Sound Registers addresses */
#define GBA_REG_SOUND1CNT_L_ADDR  GBA_IO_REGS_ADDR + GBA_SOUND1CNT_L
#define GBA_REG_SG10_ADDR         GBA_IO_REGS_ADDR + GBA_SG10
#define GBA_REG_SG10_L_ADDR       GBA_IO_REGS_ADDR + GBA_SG10_L
#define GBA_REG_SOUND1CNT_H_ADDR  GBA_IO_REGS_ADDR + GBA_SOUND1CNT_H
#define GBA_REG_SG10_H_ADDR       GBA_IO_REGS_ADDR + GBA_SG10_H
#define GBA_REG_SOUND1CNT_X_ADDR  GBA_IO_REGS_ADDR + GBA_SOUND1CNT_X
#define GBA_REG_SG11_ADDR         GBA_IO_REGS_ADDR + GBA_SG11
#define GBA_REG_SOUND2CNT_L_ADDR  GBA_IO_REGS_ADDR + GBA_SOUND2CNT_L
#define GBA_REG_SG20_ADDR         GBA_IO_REGS_ADDR + GBA_SG20
#define GBA_REG_SOUND2CNT_H_ADDR  GBA_IO_REGS_ADDR + GBA_SOUND2CNT_H
#define GBA_REG_SG21_ADDR         GBA_IO_REGS_ADDR + GBA_SG21
#define GBA_REG_SOUND3CNT_L_ADDR  GBA_IO_REGS_ADDR + GBA_SOUND3CNT_L
#define GBA_REG_SG30_ADDR         GBA_IO_REGS_ADDR + GBA_SG30
#define GBA_REG_SG30_L_ADDR       GBA_IO_REGS_ADDR + GBA_SG30_L
#define GBA_REG_SOUND3CNT_H_ADDR  GBA_IO_REGS_ADDR + GBA_SOUND3CNT_H
#define GBA_REG_SG30_H_ADDR       GBA_IO_REGS_ADDR + GBA_SG30_H
#define GBA_REG_SOUND3CNT_X_ADDR  GBA_IO_REGS_ADDR + GBA_SOUND3CNT_X
#define GBA_REG_SG31_ADDR         GBA_IO_REGS_ADDR + GBA_SG31
#define GBA_REG_SOUND4CNT_L_ADDR  GBA_IO_REGS_ADDR + GBA_SOUND4CNT_L
#define GBA_REG_SG40_ADDR         GBA_IO_REGS_ADDR + GBA_SG40
#define GBA_REG_SOUND4CNT_H_ADDR  GBA_IO_REGS_ADDR + GBA_SOUND4CNT_H
#define GBA_REG_SG41_ADDR         GBA_IO_REGS_ADDR + GBA_SG41
#define GBA_REG_SOUNDCNT_L_ADDR   GBA_IO_REGS_ADDR + GBA_SOUNDCNT_L
#define GBA_REG_SGCNT0_L_ADDR     GBA_IO_REGS_ADDR + GBA_SGCNT0_L
#define GBA_REG_SOUNDCNT_H_ADDR   GBA_IO_REGS_ADDR + GBA_SOUNDCNT_H
#define GBA_REG_SGCNT0_H_ADDR     GBA_IO_REGS_ADDR + GBA_SGCNT0_H
#define GBA_REG_SOUNDCNT_X_ADDR   GBA_IO_REGS_ADDR + GBA_SOUNDCNT_X
#define GBA_REG_SGCNT1_ADDR       GBA_IO_REGS_ADDR + GBA_SGCNT1
#define GBA_REG_SGBIAS_ADDR       GBA_IO_REGS_ADDR + GBA_SGBIAS
#define GBA_REG_SOUNDBIAS_ADDR    GBA_IO_REGS_ADDR + GBA_SOUNDBIAS
#define GBA_REG_SGWR0_ADDR        GBA_IO_REGS_ADDR + GBA_SGWR0
#define GBA_REG_WAVE_RAM0_L_ADDR  GBA_IO_REGS_ADDR + GBA_WAVE_RAM0_L
#define GBA_REG_SGWR0_L_ADDR      GBA_IO_REGS_ADDR + GBA_SGWR0_L
#define GBA_REG_WAVE_RAM0_H_ADDR  GBA_IO_REGS_ADDR + GBA_WAVE_RAM0_H
#define GBA_REG_SGWR0_H_ADDR      GBA_IO_REGS_ADDR + GBA_SGWR0_H
#define GBA_REG_SGWR1_ADDR        GBA_IO_REGS_ADDR + GBA_SGWR1
#define GBA_REG_WAVE_RAM1_L_ADDR  GBA_IO_REGS_ADDR + GBA_WAVE_RAM1_L
#define GBA_REG_SGWR1_L_ADDR      GBA_IO_REGS_ADDR + GBA_SGWR1_L
#define GBA_REG_WAVE_RAM1_H_ADDR  GBA_IO_REGS_ADDR + GBA_WAVE_RAM1_H
#define GBA_REG_SGWR1_H_ADDR      GBA_IO_REGS_ADDR + GBA_SGWR1_H
#define GBA_REG_SGWR2_ADDR        GBA_IO_REGS_ADDR + GBA_SGWR2
#define GBA_REG_WAVE_RAM2_L_ADDR  GBA_IO_REGS_ADDR + GBA_WAVE_RAM2_L
#define GBA_REG_SGWR2_L_ADDR      GBA_IO_REGS_ADDR + GBA_SGWR2_L
#define GBA_REG_WAVE_RAM2_H_ADDR  GBA_IO_REGS_ADDR + GBA_WAVE_RAM2_H
#define GBA_REG_SGWR2_H_ADDR      GBA_IO_REGS_ADDR + GBA_SGWR2_H
#define GBA_REG_SGWR3_ADDR        GBA_IO_REGS_ADDR + GBA_SGWR3
#define GBA_REG_WAVE_RAM3_L_ADDR  GBA_IO_REGS_ADDR + GBA_WAVE_RAM3_L
#define GBA_REG_SGWR3_L_ADDR      GBA_IO_REGS_ADDR + GBA_SGWR3_L
#define GBA_REG_WAVE_RAM3_H_ADDR  GBA_IO_REGS_ADDR + GBA_WAVE_RAM3_H
#define GBA_REG_SGWR3_H_ADDR      GBA_IO_REGS_ADDR + GBA_SGWR3_H
#define GBA_REG_SGFIF0A_ADDR      GBA_IO_REGS_ADDR + GBA_SGFIF0A
#define GBA_REG_FIFO_A_L_ADDR     GBA_IO_REGS_ADDR + GBA_FIFO_A_L
#define GBA_REG_SGFIFOA_L_ADDR    GBA_IO_REGS_ADDR + GBA_SGFIFOA_L
#define GBA_REG_FIFO_A_H_ADDR     GBA_IO_REGS_ADDR + GBA_FIFO_A_H
#define GBA_REG_SGFIFOA_H_ADDR    GBA_IO_REGS_ADDR + GBA_SGFIFOA_H
#define GBA_REG_SGFIFOB_ADDR      GBA_IO_REGS_ADDR + GBA_SGFIFOB
#define GBA_REG_FIFO_B_L_ADDR     GBA_IO_REGS_ADDR + GBA_FIFO_B_L
#define GBA_REG_SGFIFOB_L_ADDR    GBA_IO_REGS_ADDR + GBA_SGFIFOB_L
#define GBA_REG_FIFO_B_H_ADDR     GBA_IO_REGS_ADDR + GBA_FIFO_B_H
#define GBA_REG_SGFIFOB_H_ADDR    GBA_IO_REGS_ADDR + GBA_SGFIFOB_H
/* Sound Registers */
#ifndef __asm__
#define GBA_REG_SOUND1CNT_L     (*(volatile unsigned int   *)(GBA_REG_SOUND1CNT_L_ADDR))
#define GBA_REG_SG10            (*(volatile unsigned int   *)(GBA_REG_SG10_ADDR))
#define GBA_REG_SG10_L          (*(volatile unsigned short *)(GBA_REG_SG10_L_ADDR))
#define GBA_REG_SOUND1CNT_H     (*(volatile unsigned short *)(GBA_REG_SOUND1CNT_H_ADDR))
#define GBA_REG_SG10_H          (*(volatile unsigned short *)(GBA_REG_SG10_H_ADDR))
#define GBA_REG_SOUND1CNT_X     (*(volatile unsigned short *)(GGBA_REG_SOUND1CNT_X_ADDR))
#define GBA_REG_SG11            (*(volatile unsigned short *)(GBA_REG_SG11_ADDR))
#define GBA_REG_SOUND2CNT_L     (*(volatile unsigned short *)(GBA_REG_SOUND2CNT_L_ADDR))
#define GBA_REG_SG20            (*(volatile unsigned short *)(GBA_REG_SG20_ADDR))
#define GBA_REG_SOUND2CNT_H     (*(volatile unsigned short *)(GBA_REG_SOUND2CNT_H_ADDR))
#define GBA_REG_SG21            (*(volatile unsigned short *)(GBA_REG_SG21_ADDR))
#define GBA_REG_SOUND3CNT_L     (*(volatile unsigned int   *)(GBA_REG_SOUND3CNT_L_ADDR))
#define GBA_REG_SG30            (*(volatile unsigned int   *)(GBA_REG_SG30_ADDR))
#define GBA_REG_SG30_L          (*(volatile unsigned short *)(GBA_REG_SG30_L_ADDR))
#define GBA_REG_SOUND3CNT_H     (*(volatile unsigned short *)(GBA_REG_SOUND3CNT_H_ADDR))
#define GBA_REG_SG30_H          (*(volatile unsigned short *)(GBA_REG_SG30_H_ADDR))
#define GBA_REG_SOUND3CNT_X     (*(volatile unsigned short *)(GBA_REG_SOUND3CNT_X_ADDR))
#define GBA_REG_SG31            (*(volatile unsigned short *)(GBA_REG_SG31_ADDR))
#define GBA_REG_SOUND4CNT_L     (*(volatile unsigned short *)(GBA_REG_SOUND4CNT_L_ADDR))
#define GBA_REG_SG40            (*(volatile unsigned short *)(GBA_REG_SG40_ADDR))
#define GBA_REG_SOUND4CNT_H     (*(volatile unsigned short *)(GBA_REG_SOUND4CNT_H_ADDR))
#define GBA_REG_SG41            (*(volatile unsigned short *)(GBA_REG_SG41_ADDR))
#define GBA_REG_SGCNT0          (*(volatile unsigned int   *)(GBA_REG_SGCNT0_ADDR))
#define GBA_REG_SOUNDCNT_L      (*(volatile unsigned short *)(GBA_REG_SOUNDCNT_L_ADDR))
#define GBA_REG_SGCNT0_L        (*(volatile unsigned short *)(GBA_REG_SGCNT0_L_ADDR))
#define GBA_REG_SOUNDCNT_H      (*(volatile unsigned short *)(GBA_REG_SOUNDCNT_H_ADDR))
#define GBA_REG_SGCNT0_H        (*(volatile unsigned short *)(GBA_REG_SGCNT0_H_ADDR))
#define GBA_REG_SOUNDCNT_X      (*(volatile unsigned short *)(GBA_REG_SOUNDCNT_X_ADDR))
#define GBA_REG_SGCNT1          (*(volatile unsigned short *)(GBA_REG_SGCNT1_ADDR))
#define GBA_REG_SOUNDBIAS       (*(volatile unsigned short *)(GBA_REG_SOUNDBIAS_ADDR))
#define GBA_REG_SGBIAS          (*(volatile unsigned short *)(GBA_REG_SGBIAS_ADDR))
#define GBA_REG_SGWR0           (*(volatile unsigned int   *)(GBA_REG_SGWR0_ADDR))
#define GBA_REG_WAVE_RAM0_L     (*(volatile unsigned short *)(GBA_REG_WAVE_RAM0_L_ADDR))
#define GBA_REG_SGWR0_L         (*(volatile unsigned short *)(GBA_REG_SGWR0_L_ADDR))
#define GBA_REG_WAVE_RAM0_H     (*(volatile unsigned short *)(GBA_REG_WAVE_RAM0_H_ADDR))
#define GBA_REG_SGWR0_H         (*(volatile unsigned short *)(GBA_REG_SGWR0_H_ADDR))
#define GBA_REG_SGWR1           (*(volatile unsigned int   *)(GBA_REG_SGWR1_ADDR))
#define GBA_REG_WAVE_RAM1_L     (*(volatile unsigned short *)(GBA_REG_WAVE_RAM1_L_ADDR))
#define GBA_REG_SGWR1_L         (*(volatile unsigned short *)(GBA_REG_SGWR1_L_ADDR))
#define GBA_REG_WAVE_RAM1_H     (*(volatile unsigned short *)(GBA_REG_WAVE_RAM1_H_ADDR))
#define GBA_REG_SGWR1_H         (*(volatile unsigned short *)(GBA_REG_SGWR1_H_ADDR))
#define GBA_REG_SGWR2           (*(volatile unsigned int   *)(GBA_REG_SGWR2_ADDR))
#define GBA_REG_WAVE_RAM2_L     (*(volatile unsigned short *)(GBA_REG_WAVE_RAM2_L_ADDR))
#define GBA_REG_SGWR2_L         (*(volatile unsigned short *)(GBA_REG_SGWR2_L_ADDR))
#define GBA_REG_WAVE_RAM2_H     (*(volatile unsigned short *)(GGBA_REG_WAVE_RAM2_H_ADDR))
#define GBA_REG_SGWR2_H         (*(volatile unsigned short *)(GGBA_REG_SGWR2_H_ADDR))
#define GBA_REG_SGWR3           (*(volatile unsigned int   *)(GBA_REG_SGWR3_ADDR))
#define GBA_REG_WAVE_RAM3_L     (*(volatile unsigned short *)(GBA_REG_WAVE_RAM3_L)
#define GBA_REG_SGWR3_L         (*(volatile unsigned short *)(GBA_REG_SGWR3_L)
#define GBA_REG_WAVE_RAM3_H     (*(volatile unsigned short *)(GBA_REG_WAVE_RAM3_H_ADDR))
#define GBA_REG_SGWR3_H         (*(volatile unsigned short *)(GBA_REG_SGWR3_H_ADDR))
#define GBA_REG_SGFIF0A         (*(volatile unsigned int   *)(GBA_REG_SGFIF0A_ADDR))
#define GBA_REG_FIFO_A_L        (*(volatile unsigned short *)(GBA_REG_FIFO_A_L_ADDR))
#define GBA_REG_SGFIFOA_L       (*(volatile unsigned short *)(GBA_REG_SGFIFOA_L_ADDR))
#define GBA_REG_FIFO_A_H        (*(volatile unsigned short *)(GBA_REG_FIFO_A_H_ADDR))
#define GBA_REG_SGFIFOA_H       (*(volatile unsigned short *)(GBA_REG_SGFIFOA_H_ADDR))
#define GBA_REG_SGFIFOB         (*(volatile unsigned int   *)(GBA_REG_SGFIFOB_ADDR))
#define GBA_REG_FIFO_B_L        (*(volatile unsigned short *)(GBA_REG_FIFO_B_L_ADDR))
#define GBA_REG_SGFIFOB_L       (*(volatile unsigned short *)(GBA_REG_SGFIFOB_L_ADDR))
#define GBA_REG_FIFO_B_H        (*(volatile unsigned short *)(GBA_REG_FIFO_B_H_ADDR))
#define GBA_REG_SGFIFOB_H       (*(volatile unsigned short *)(GBA_REG_SGFIFOB_H_ADDR))
#endif

/*---------------------------------------------------------------------------+
|    DMA
+----------------------------------------------------------------------------*/
#define GBA_DMA_ENABLE            0x80000000  /**<  DMA Enable               */
#define GBA_DMA_IF_ENABLE         0x40000000  /**<  Interrupt Request Enable */
#define GBA_DMA_TIMMING_IMM       0x00000000  /**<  Run Immediately          */
#define GBA_DMA_TIMMING_V_BLANK   0x10000000  /**<  Run V Blank              */
#define GBA_DMA_TIMMING_H_BLANK   0x20000000  /**<  Run H Blank              */
#define GBA_DMA_TIMMING_DISP      0x30000000  /**<  Run Display              */
#define GBA_DMA_TIMMING_SOUND     0x30000000  /**<  Run Sound FIFO Request   */
#define GBA_DMA_DREQ_ON           0x08000000  /**<  Data Request Synchronize Mode ON */
#define GBA_DMA_16BIT_BUS         0x00000000  /**<  Select Bus Size 16Bit    */
#define GBA_DMA_32BIT_BUS         0x04000000  /**<  Select Bus Size 32Bit    */
#define GBA_DMA_CONTINUOUS_ON     0x02000000  /**<  Continuous Mode ON       */
#define GBA_DMA_SRC_INC           0x00000000  /**<  Select Source Increment  */
#define GBA_DMA_SRC_DEC           0x00800000  /**<  Select Source Decrement  */
#define GBA_DMA_SRC_FIX           0x01000000  /**<  Select Source Fixed      */
#define GBA_DMA_DEST_INC          0x00000000  /**<  Select Destination Increment */
#define GBA_DMA_DEST_DEC          0x00200000  /**<  Select Destination Decrement */
#define GBA_DMA_DEST_FIX          0x00400000  /**<  Select Destination Fixed */
#define GBA_DMA_DEST_RELOAD       0x00600000  /**<  Select Destination       */


/* DMA Transfer Channel address offsets */
#define GBA_DMA0SAD             0x000000B0   /**< DMA0 Source Address             */
#define GBA_DMA0SAD_L           0x000000B0   /**< DMA0 Source Address Low Value   */
#define GBA_DMA0SAD_H           0x000000B2   /**< DMA0 Source Address High Value  */
#define GBA_DMA0DAD             0x000000B4   /**< DMA0 Destination Address        */
#define GBA_DMA0DAD_L           0x000000B4   /**< DMA0 Destination Address Low Value  */
#define GBA_DMA0DAD_H           0x000000B6   /**< DMA0 Destination Address High Value */
#define GBA_DMA0CNT             0x000000B8   /**< DMA0 Control Word Count          */
#define GBA_DMA0CNT_L           0x000000B8   /**< DMA0 Control Low Value           */
#define GBA_DMA0CNT_H           0x000000BA   /**< DMA0 Control High Value          */
#define GBA_DMA1SAD             0x000000BC   /**< DMA1 Source Address              */
#define GBA_DMA1SAD_L           0x000000BC   /**< DMA1 Source Address Low Value    */
#define GBA_DMA1SAD_H           0x000000BE   /**< DMA1 Source Address High Value   */
#define GBA_DMA1DAD             0x000000C0   /**< DMA1 Destination Address         */
#define GBA_DMA1DAD_L           0x000000C0   /**< DMA1 Destination Address Low Value  */
#define GBA_DMA1DAD_H           0x000000C2   /**< DMA1 Destination Address High Value */
#define GBA_DMA1CNT             0x000000C4   /**< DMA1 Control Word Count          */
#define GBA_DMA1CNT_L           0x000000C4   /**< DMA1 Control Low Value           */
#define GBA_DMA1CNT_H           0x000000C6   /**< DMA1 Control High Value          */
#define GBA_DMA2SAD             0x000000C8   /**< DMA2 Source Address              */
#define GBA_DMA2SAD_L           0x000000C8   /**< DMA2 Source Address Low Value    */
#define GBA_DMA2SAD_H           0x000000CA   /**< DMA2 Source Address High Value   */
#define GBA_DMA2DAD             0x000000CC   /**< DMA2 Destination Address         */
#define GBA_DMA2DAD_L           0x000000CC   /**< DMA2 Destination Address Low Value  */
#define GBA_DMA2DAD_H           0x000000CE   /**< DMA2 Destination Address High Value */
#define GBA_DMA2CNT             0x000000D0   /**< DMA2 Control Word Count          */
#define GBA_DMA2CNT_L           0x000000D0   /**< DMA2 Control Low Value           */
#define GBA_DMA2CNT_H           0x000000D2   /**< DMA2 Control High Value          */
#define GBA_DMA3SAD             0x000000D4   /**< DMA3 Source Address              */
#define GBA_DMA3SAD_L           0x000000D4   /**< DMA3 Source Address Low Value    */
#define GBA_DMA3SAD_H           0x000000D6   /**< DMA3 Source Address High Value   */
#define GBA_DMA3DAD             0x000000D8   /**< DMA3 Destination Address         */
#define GBA_DMA3DAD_L           0x000000D8   /**< DMA3 Destination Address Low Value  */
#define GBA_DMA3DAD_H           0x000000DA   /**< DMA3 Destination Address High Value */
#define GBA_DMA3CNT             0x000000DC   /**< DMA3 Control Word Count          */
#define GBA_DMA3CNT_L           0x000000DC   /**< DMA3 Control Low Value           */
#define GBA_DMA3CNT_H           0x000000DE   /**< DMA3 Control High Value          */
/* DMA Transfer Channel address  */
#define GBA_REG_DMA0SAD_ADDR    GBA_IO_REGS_ADDR + GBA_DMA0SAD
#define GBA_REG_DMA0SAD_L_ADDR  GBA_IO_REGS_ADDR + GBA_DMA0SAD_L
#define GBA_REG_DMA0SAD_H_ADDR  GBA_IO_REGS_ADDR + GBA_DMA0SAD_H
#define GBA_REG_DMA0DAD_ADDR    GBA_IO_REGS_ADDR + GBA_DMA0DAD
#define GBA_REG_DMA0DAD_L_ADDR  GBA_IO_REGS_ADDR + GBA_DMA0DAD_L
#define GBA_REG_DMA0DAD_H_ADDR  GBA_IO_REGS_ADDR + GBA_DMA0DAD_H
#define GBA_REG_DMA0CNT_ADDR    GBA_IO_REGS_ADDR + GBA_DMA0CNT
#define GBA_REG_DMA0CNT_L_ADDR  GBA_IO_REGS_ADDR + GBA_DMA0CNT_L
#define GBA_REG_DMA0CNT_H_ADDR  GBA_IO_REGS_ADDR + GBA_DMA0CNT_H
#define GBA_REG_DMA1SAD_ADDR    GBA_IO_REGS_ADDR + GBA_DMA1SAD
#define GBA_REG_DMA1SAD_L_ADDR  GBA_IO_REGS_ADDR + GBA_DMA1SAD_L
#define GBA_REG_DMA1SAD_H_ADDR  GBA_IO_REGS_ADDR + GBA_DMA1SAD_H
#define GBA_REG_DMA1DAD_ADDR    GBA_IO_REGS_ADDR + GBA_DMA1DAD)
#define GBA_REG_DMA1DAD_L_ADDR  GBA_IO_REGS_ADDR + GBA_DMA1DAD_L
#define GBA_REG_DMA1DAD_H_ADDR  GBA_IO_REGS_ADDR + GBA_DMA1DAD_H
#define GBA_REG_DMA1CNT_ADDR    GBA_IO_REGS_ADDR + GBA_DMA1CNT
#define GBA_REG_DMA1CNT_L_ADDR  GBA_IO_REGS_ADDR + GBA_DMA1CNT_L
#define GBA_REG_DMA1CNT_H_ADDR  GBA_IO_REGS_ADDR + GBA_DMA1CNT_H
#define GBA_REG_DMA2SAD_ADDR    GBA_IO_REGS_ADDR + GBA_DMA2SAD
#define GBA_REG_DMA2SAD_L_ADDR  GBA_IO_REGS_ADDR + GBA_DMA2SAD_L
#define GBA_REG_DMA2SAD_H_ADDR  GBA_IO_REGS_ADDR + GBA_DMA2SAD_H
#define GBA_REG_DMA2DAD_ADDR    GBA_IO_REGS_ADDR + GBA_DMA2DAD
#define GBA_REG_DMA2DAD_L_ADDR  GBA_IO_REGS_ADDR + GBA_DMA2DAD_L
#define GBA_REG_DMA2DAD_H_ADDR  GBA_IO_REGS_ADDR + GBA_DMA2DAD_H
#define GBA_REG_DMA2CNT_ADDR    GBA_IO_REGS_ADDR + GBA_DMA2CNT
#define GBA_REG_DMA2CNT_L_ADDR  GBA_IO_REGS_ADDR + GBA_DMA2CNT_L
#define GBA_REG_DMA2CNT_H_ADDR  GBA_IO_REGS_ADDR + GBA_DMA2CNT_H
#define GBA_REG_DMA3SAD_ADDR    GBA_IO_REGS_ADDR + GBA_DMA3SAD
#define GBA_REG_DMA3SAD_L_ADDR  GBA_IO_REGS_ADDR + GBA_DMA3SAD_L
#define GBA_REG_DMA3SAD_H_ADDR  GBA_IO_REGS_ADDR + GBA_DMA3SAD_H
#define GBA_REG_DMA3DAD_ADDR    GBA_IO_REGS_ADDR + GBA_DMA3DAD
#define GBA_REG_DMA3DAD_L_ADDR  GBA_IO_REGS_ADDR + GBA_DMA3DAD_L
#define GBA_REG_DMA3DAD_H_ADDR  GBA_IO_REGS_ADDR + GBA_DMA3DAD_H
#define GBA_REG_DMA3CNT_ADDR    GBA_IO_REGS_ADDR + GBA_DMA3CNT
#define GBA_REG_DMA3CNT_L_ADDR  GBA_IO_REGS_ADDR + GBA_DMA3CNT_L
#define GBA_REG_DMA3CNT_H_ADDR  GBA_IO_REGS_ADDR + GBA_DMA3CNT_H
/* DMA Transfer Channel registers*/
#ifndef __asm__
#define GBA_REG_DMA0SAD         (*(volatile unsigned int   *)(GBA_REG_DMA0SAD_ADDR))
#define GBA_REG_DMA0SAD_L       (*(volatile unsigned short *)(GBA_REG_DMA0SAD_L_ADDR))
#define GBA_REG_DMA0SAD_H       (*(volatile unsigned short *)(GBA_REG_DMA0SAD_H_ADDR))
#define GBA_REG_DMA0DAD         (*(volatile unsigned int   *)(GBA_REG_DMA0DAD_ADDR))
#define GBA_REG_DMA0DAD_L       (*(volatile unsigned short *)(GBA_REG_DMA0DAD_L_ADDR))
#define GBA_REG_DMA0DAD_H       (*(volatile unsigned short *)(GBA_REG_DMA0DAD_H_ADDR))
#define GBA_REG_DMA0CNT         (*(volatile unsigned int   *)(GBA_REG_DMA0CNT_ADDR))
#define GBA_REG_DMA0CNT_L       (*(volatile unsigned short *)(GBA_REG_DMA0CNT_L_ADDR))
#define GBA_REG_DMA0CNT_H       (*(volatile unsigned short *)(GBA_REG_DMA0CNT_H_ADDR))
#define GBA_REG_DMA1SAD         (*(volatile unsigned int   *)(GBA_REG_DMA1SAD_ADDR))
#define GBA_REG_DMA1SAD_L       (*(volatile unsigned short *)(GBA_REG_DMA1SAD_L_ADDR))
#define GBA_REG_DMA1SAD_H       (*(volatile unsigned short *)(GBA_REG_DMA1SAD_H_ADDR))
#define GBA_REG_DMA1DAD         (*(volatile unsigned int   *)(GBA_REG_DMA1DAD_ADDR))
#define GBA_REG_DMA1DAD_L       (*(volatile unsigned short *)(GBA_REG_DMA1DAD_L_ADDR))
#define GBA_REG_DMA1DAD_H       (*(volatile unsigned short *)(GBA_REG_DMA1DAD_H_ADDR))
#define GBA_REG_DMA1CNT         (*(volatile unsigned int   *)(GBA_REG_DMA1CNT_ADDR))
#define GBA_REG_DMA1CNT_L       (*(volatile unsigned short *)(GBA_REG_DMA1CNT_L_ADDR))
#define GBA_REG_DMA1CNT_H       (*(volatile unsigned short *)(GBA_REG_DMA1CNT_H_ADDR))
#define GBA_REG_DMA2SAD         (*(volatile unsigned int   *)(GBA_REG_DMA2SAD_ADDR))
#define GBA_REG_DMA2SAD_L       (*(volatile unsigned short *)(GBA_REG_DMA2SAD_L_ADDR))
#define GBA_REG_DMA2SAD_H       (*(volatile unsigned short *)(GBA_REG_DMA2SAD_H_ADDR))
#define GBA_REG_DMA2DAD         (*(volatile unsigned int   *)(GBA_REG_DMA2DAD_ADDR))
#define GBA_REG_DMA2DAD_L       (*(volatile unsigned short *)(GBA_REG_DMA2DAD_L_ADDR))
#define GBA_REG_DMA2DAD_H       (*(volatile unsigned short *)(GBA_REG_DMA2DAD_H_ADDR))
#define GBA_REG_DMA2CNT         (*(volatile unsigned int   *)(GBA_REG_DMA2CNT_ADDR))
#define GBA_REG_DMA2CNT_L       (*(volatile unsigned short *)(GBA_REG_DMA2CNT_L_ADDR))
#define GBA_REG_DMA2CNT_H       (*(volatile unsigned short *)(GBA_REG_DMA2CNT_H_ADDR))
#define GBA_REG_DMA3SAD         (*(volatile unsigned int   *)(GBA_REG_DMA3SAD_ADDR))
#define GBA_REG_DMA3SAD_L       (*(volatile unsigned short *)(GBA_REG_DMA3SAD_L_ADDR))
#define GBA_REG_DMA3SAD_H       (*(volatile unsigned short *)(GBA_REG_DMA3SAD_H_ADDR))
#define GBA_REG_DMA3DAD         (*(volatile unsigned int   *)(GBA_REG_DMA3DAD_ADDR))
#define GBA_REG_DMA3DAD_L       (*(volatile unsigned short *)(GBA_REG_DMA3DAD_L_ADDR))
#define GBA_REG_DMA3DAD_H       (*(volatile unsigned short *)(GBA_REG_DMA3DAD_H_ADDR))
#define GBA_REG_DMA3CNT         (*(volatile unsigned int *)(GBA_REG_DMA3CNT_ADDR))
#define GBA_REG_DMA3CNT_L       (*(volatile unsigned short *)(GBA_REG_DMA3CNT_L_ADDR))
#define GBA_REG_DMA3CNT_H       (*(volatile unsigned short *)(GBA_REG_DMA3CNT_H_ADDR))
#endif

/*---------------------------------------------------------------------------+
|    TIMER
+----------------------------------------------------------------------------*/
#define GBA_TMR_PRESCALER_1CK     0x0000    /**< Prescaler 1 clock           */
#define GBA_TMR_PRESCALER_64CK    0x0001    /**<          64 clocks          */
#define GBA_TMR_PRESCALER_256CK   0x0002    /**<         256 clocks          */
#define GBA_TMR_PRESCALER_1024CK  0x0003    /**<        1024 clocks          */
#define GBA_TMR_IF_ENABLE         0x0040    /**< Interrupt Request Enable    */
#define GBA_TMR_ENABLE            0x0080    /**< Run Timer                   */

/* Timer Register address offsets */
#define GBA_TM0D                0x00000100  /**< Timer 0 counter value      X*/
#define GBA_TM0CNT_L            0x00000100  /**< Timer 0 counter value       */
#define GBA_TM0CNT              0x00000102  /**< Timer 0 Control            X*/
#define GBA_TM0CNT_H            0x00000102  /**< Timer 0 Control             */
#define GBA_TM1D                0x00000104  /**< Timer 1 counter value      X*/
#define GBA_TM1CNT_L            0x00000104  /**< Timer 1 counter value       */
#define GBA_TM1CNT              0x00000106  /**< Timer 1 control            X*/
#define GBA_TM1CNT_H            0x00000106  /**< Timer 1 control             */
#define GBA_TM2D                0x00000108  /**< Timer 2 counter value      X*/
#define GBA_TM2CNT_L            0x00000108  /**< Timer 2 counter value       */
#define GBA_TM2CNT              0x0000010A  /**< Timer 2 control            X*/
#define GBA_TM2CNT_H            0x0000010A  /**< Timer 2 control             */
#define GBA_TM3D                0x0000010C  /**< Timer 3 counter value      X*/
#define GBA_TM3CNT_L            0x0000010C  /**< Timer 3 counter value       */
#define GBA_TM3CNT              0x0000010E  /**< Timer 4 control            X*/
#define GBA_TM3CNT_H            0x0000010E  /**< Timer 4 control             */
/* Timer Register addresses */
#define GBA_REG_TM0D_ADDR       GBA_IO_REGS_ADDR + GBA_TM0D
#define GBA_REG_TM0CNT_L_ADDR   GBA_IO_REGS_ADDR + GBA_TM0CNT_L
#define GBA_REG_TM0CNT_ADDR     GBA_IO_REGS_ADDR + GBA_TM0CNT
#define GBA_REG_TM0CNT_H_ADDR   GBA_IO_REGS_ADDR + GBA_TM0CNT_H
#define GBA_REG_TM1D_ADDR       GBA_IO_REGS_ADDR + GBA_TM1D
#define GBA_REG_TM1CNT_L_ADDR   GBA_IO_REGS_ADDR + GBA_TM1CNT_L
#define GBA_REG_TM1CNT_ADDR     GBA_IO_REGS_ADDR + GBA_TM1CNT
#define GBA_REG_TM1CNT_H_ADDR   GBA_IO_REGS_ADDR + GBA_TM1CNT_H
#define GBA_REG_TM2D_ADDR       GBA_IO_REGS_ADDR + GBA_TM2D
#define GBA_REG_TM2CNT_L_ADDR   GBA_IO_REGS_ADDR + GBA_TM2CNT_L
#define GBA_REG_TM2CNT_ADDR     GBA_IO_REGS_ADDR + GBA_TM2CNT
#define GBA_REG_TM2CNT_H_ADDR   GBA_IO_REGS_ADDR + GBA_TM2CNT_H
#define GBA_REG_TM3D_ADDR       GBA_IO_REGS_ADDR + GBA_TM3D
#define GBA_REG_TM3CNT_L_ADDR   GBA_IO_REGS_ADDR + GBA_TM3CNT_L
#define GBA_REG_TM3CNT_ADDR     GBA_IO_REGS_ADDR + GBA_TM3CNT
#define GBA_REG_TM3CNT_H_ADDR   GBA_IO_REGS_ADDR + GBA_TM3CNT_H
/* Timer Registers */
#ifndef __asm__
#define GBA_REG_TM0D            (*(volatile unsigned short *)(GBA_REG_TM0D_ADDR))
#define GBA_REG_TM0CNT_L        (*(volatile unsigned short *)(GBA_REG_TM0CNT_L_ADDR))
#define GBA_REG_TM0CNT          (*(volatile unsigned short *)(GBA_REG_TM0CNT_ADDR))
#define GBA_REG_TM0CNT_H        (*(volatile unsigned short *)(GBA_REG_TM0CNT_H_ADDR))
#define GBA_REG_TM1D            (*(volatile unsigned short *)(GBA_REG_TM1D_ADDR))
#define GBA_REG_TM1CNT_L        (*(volatile unsigned short *)(GBA_REG_TM1CNT_L_ADDR))
#define GBA_REG_TM1CNT          (*(volatile unsigned short *)(GBA_REG_TM1CNT_ADDR))
#define GBA_REG_TM1CNT_H        (*(volatile unsigned short *)(GBA_REG_TM1CNT_H_ADDR))
#define GBA_REG_TM2D            (*(volatile unsigned short *)(GBA_REG_TM2D_ADDR))
#define GBA_REG_TM2CMT_L        (*(volatile unsigned short *)(GBA_REG_TM2CNT_L_ADDR))
#define GBA_REG_TM2CNT          (*(volatile unsigned short *)(GBA_REG_TM2CNT_ADDR))
#define GBA_REG_TM2CNT_H        (*(volatile unsigned short *)(GBA_REG_TM2CNT_H_ADDR))
#define GBA_REG_TM3D            (*(volatile unsigned short *)(GBA_REG_TM3D_ADDR))
#define GBA_REG_TM3CNT_L        (*(volatile unsigned short *)(GBA_REG_TM3CNT_L_ADDR))
#define GBA_REG_TM3CNT          (*(volatile unsigned short *)(GBA_REG_TM3CNT_ADDR))
#define GBA_REG_TM3CNT_H        (*(volatile unsigned short *)(GBA_REG_TM3CNT_H_ADDR))
#endif

/*---------------------------------------------------------------------------+
|    SERIAL set1
+----------------------------------------------------------------------------*/
#define GBA_SER_BAUD_MASK       0x0003
#define GBA_SER_BAUD_9600       0x0000
#define GBA_SER_BAUD_38400      0x0001
#define GBA_SER_BAUD_57600      0x0002
#define GBA_SER_BAUD_115200     0x0003
#define GBA_SER_CTS             0x0004

/* Serial Communication address offsets */
#define GBA_SIOMULTI0           0x00000120 /**< SIO Multi-Player Data 0      */
#define GBA_SCD0                0x00000120 /**< SIO Multi-Player Data 0     X*/
#define GBA_SIOMULTI1           0x00000122 /**< SIO Multi-Player Data 1      */
#define GBA_SCD1                0x00000122 /**< SIO Multi-Player Data 1     X*/
#define GBA_SIOMULTI2           0x00000124 /**< SIO Multi-Player Data 2      */
#define GBA_SCD2                0x00000124 /**< SIO Multi-Player Data 2     X*/
#define GBA_SIOMULTI3           0x00000126 /**< SIO Multi-Player Data 3      */
#define GBA_SCD3                0x00000126 /**< SIO Multi-Player Data 3     X*/
#define GBA_SIOCNT              0x00000128 /**< SIO Control                  */
#define GBA_SCCNT               0x00000128 /**< SIO Control                 X*/
#define GBA_SCCNT_L             0x00000128 /**< SIO Control                 X*/
#define GBA_SIOMLT_SEND         0x0000012A /**< Data Send Register           */
#define GBA_SCCNT_H             0x0000012A /**< Data Send Register          X*/
/* Serial Communication addresses */
#define GBA_REG_SIOMULTI0_ADDR   GBA_IO_REGS_ADDR + GBA_SIOMULTI0
#define GBA_REG_SCD0_ADDR        GBA_IO_REGS_ADDR + GBA_SCD0
#define GBA_REG_SIOMULTI1_ADDR   GBA_IO_REGS_ADDR + GBA_SIOMULTI1
#define GBA_REG_SCD1_ADDR        GBA_IO_REGS_ADDR + GBA_SCD1
#define GBA_REG_SIOMULTI2_ADDR   GBA_IO_REGS_ADDR + GBA_SIOMULTI2
#define GBA_REG_SCD2_ADDR        GBA_IO_REGS_ADDR + GBA_SCD2
#define GBA_REG_SIOMULTI3_ADDR   GBA_IO_REGS_ADDR + GBA_SIOMULTI3
#define GBA_REG_SCD3_ADDR        GBA_IO_REGS_ADDR + GBA_SCD3
#define GBA_REG_SCCNT_ADDR       GBA_IO_REGS_ADDR + GBA_SCCNT
#define GBA_REG_SIOCNT_ADDR      GBA_IO_REGS_ADDR + GBA_SIOCNT
#define GBA_REG_SCCNT_L_ADDR     GBA_IO_REGS_ADDR + GBA_SCCNT_L
#define GBA_REG_SIOMLT_SEND_ADDR GBA_IO_REGS_ADDR + GBA_SIOMLT_SEND
#define GBA_REG_SCCNT_H_ADDR     GBA_IO_REGS_ADDR + GBA_SCCNT_H
/* Serial Communication registers */
#ifndef __asm__
#define GBA_REG_SIOMULTI0       (*(volatile unsigned short *)(GBA_REG_SIOMULTI0_ADDR))
#define GBA_REG_SCD0            (*(volatile unsigned short *)(GBA_REG_SCD0_ADDR))
#define GBA_REG_SIOMULTI1       (*(volatile unsigned short *)(GBA_REG_SIOMULTI1_ADDR))
#define GBA_REG_SCD1            (*(volatile unsigned short *)(GBA_REG_SCD1_ADDR))
#define GBA_REG_SIOMULTI2       (*(volatile unsigned short *)(GBA_REG_SIOMULTI2_ADDR))
#define GBA_REG_SCD2            (*(volatile unsigned short *)(GBA_REG_SCD2_ADDR))
#define GBA_REG_SIOMULTI3       (*(volatile unsigned short *)(GBA_REG_SIOMULTI3_ADDR))
#define GBA_REG_SCD3            (*(volatile unsigned short *)(GBA_REG_SCD3_ADDR))
#define GBA_REG_SCCNT           (*(volatile unsigned int   *)(GBA_REG_SCCNT_ADDR))
#define GBA_REG_SIOCNT          (*(volatile unsigned short *)(GBA_REG_SIOCNT_ADDR))
#define GBA_REG_SCCNT_L         (*(volatile unsigned short *)(GBA_REG_SCCNT_L_ADDR))
#define GBA_REG_SIOMLT_SEND     (*(volatile unsigned short *)(GBA_REG_SIOMLT_SEND_ADDR))
#define GBA_REG_SCCNT_H         (*(volatile unsigned short *)(GBA_REG_SCCNT_H_ADDR))
#endif

/*---------------------------------------------------------------------------+
|    KEYPAD
+----------------------------------------------------------------------------*/
/* GBA Keys */
#define GBA_KEY_A               0x0001
#define GBA_KEY_B               0x0002
#define GBA_KEY_SELECT          0x0004
#define GBA_KEY_START           0x0008
#define GBA_KEY_RIGHT           0x0010
#define GBA_KEY_LEFT            0x0020
#define GBA_KEY_UP              0x0040
#define GBA_KEY_DOWN            0x0080
#define GBA_KEY_R               0x0100
#define GBA_KEY_L               0x0200
#define GBA_KEY_ALL             0x03FF
/* Keypad registers address offsets */
#define GBA_P1                  0x00000130 /**< Key Status                  X*/
#define GBA_KEYINPUT            0x00000130 /**< Key Status                   */
#define GBA_P1CNT               0x00000132 /**< Key Interrupt Control       X*/
#define GBA_KEYCNT              0x00000132 /**< Key Interrupt Control        */
/* Keypad registers addresses */
#define GBA_REG_P1_ADDR         GBA_IO_REGS_ADDR + GBA_P1
#define GBA_REG_KEYINPUT_ADDR   GBA_IO_REGS_ADDR + GBA_KEYINPUT
#define GBA_REG_P1CNT_ADDR      GBA_IO_REGS_ADDR + GBA_P1CNT
#define GBA_REG_KEYCNT_ADDR     GBA_IO_REGS_ADDR + GBA_KEYCNT
/* Keypad registers */
#ifndef __asm__
#define GBA_REG_P1              (*(volatile unsigned short *)(GBA_REG_P1_ADDR))
#define GBA_REG_KEYINPUT        (*(volatile unsigned short *)(GBA_REG_KEYINPUT_ADDR))
#define GBA_REG_P1CNT           (*(volatile unsigned short *)(GBA_REG_P1CNT_ADDR))
#define GBA_REG_KEYCNT          (*(volatile unsigned short *)(GBA_REG_KEYCNT_ADDR))
#endif

/*---------------------------------------------------------------------------+
|    SERIAL set2
+----------------------------------------------------------------------------*/
/* Serial Communication address offsets */
#define GBA_R                    0x00000134 /**< Mode Selection             X*/
#define GBA_RCNT                 0x00000134 /**< Mode Selection              */
#define GBA_HS_CTRL              0x00000140 /**< JOY BUS Control Register   X*/
#define GBA_JOYCNT               0x00000140 /**< JOY BUS Control Register    */
#define GBA_JOYRE                0x00000150 /**< Receive Data Register      X*/
#define GBA_JOYRE_L              0x00000150 /**< Receive Data Register low  X*/
#define GBA_JOY_RECV_L           0x00000150 /**< Receive Data Register low   */
#define GBA_JOYRE_H              0x00000152 /**< Receive Data Register high X*/
#define GBA_JOY_RECV_H           0x00000152 /**< Receive Data Register high  */
#define GBA_JOYTR                0x00000154 /**< Send Data Register         X*/
#define GBA_JOYTR_L              0x00000154 /**< Send Data Register low     X*/
#define GBA_JOY_TRANS_L          0x00000154 /**< Send Data Register low      */
#define GBA_JOYTR_H              0x00000156 /**< Send Data Register high    X*/
#define GBA_JOY_TRANS_H          0x00000156 /**< Send Data Register high     */
#define GBA_JSTAT                0x00000158 /**< Receive Status Register    X*/
#define GBA_JOYSTAT              0x00000158 /**< Receive Status Register     */
/* Serial Communication register addresses */
#define GBA_REG_R_ADDR           GBA_IO_REGS_ADDR + GBA_R
#define GBA_REG_RCNT_ADDR        GBA_IO_REGS_ADDR + GBA_RCNT
#define GBA_REG_HS_CTRL_ADDR     GBA_IO_REGS_ADDR + GBA_HS_CTRL
#define GBA_REG_JOYCNT_ADDR      GBA_IO_REGS_ADDR + GBA_JOYCNT
#define GBA_REG_JOYRE_ADDR       GBA_IO_REGS_ADDR + GBA_JOYRE
#define GBA_REG_JOYRE_L_ADDR     GBA_IO_REGS_ADDR + GBA_JOYRE_L
#define GBA_REG_JOY_RECV_L_ADDR  GBA_IO_REGS_ADDR + GBA_JOY_RECV_L
#define GBA_REG_JOYRE_H_ADDR     GBA_IO_REGS_ADDR + GBA_JOYRE_H
#define GBA_REG_JOY_RECV_H_ADDR  GBA_IO_REGS_ADDR + GBA_JOY_RECV_H
#define GBA_REG_JOYTR_ADDR       GBA_IO_REGS_ADDR + GBA_JOYTR
#define GBA_REG_JOYTR_L_ADDR     GBA_IO_REGS_ADDR + GBA_JOYTR_L
#define GBA_REG_JOY_TRANS_L_ADDR GBA_IO_REGS_ADDR + GBA_JOY_TRANS_L
#define GBA_REG_JOYTR_H_ADDR     GBA_IO_REGS_ADDR + GBA_JOYTR_H
#define GBA_REG_JOY_TRANS_H_ADDR GBA_IO_REGS_ADDR + GBA_JOY_TRANS_H
#define GBA_REG_JSTAT_ADDR       GBA_IO_REGS_ADDR + GBA_JSTAT
#define GBA_REG_JOYSTAT_ADDR     GBA_IO_REGS_ADDR + GBA_JOYSTAT
/* Serial Communication registers */
#ifndef __asm__
#define GBA_REG_R               (*(volatile unsigned short *)(GBA_REG_R_ADDR))
#define GBA_REG_RCNT            (*(volatile unsigned short *)(GBA_REG_RCNT_ADDR))
#define GBA_REG_HS_CTRL         (*(volatile unsigned short *)(GBA_REG_HS_CTRL_ADDR))
#define GBA_REG_JOYCNT          (*(volatile unsigned short *)(GBA_REG_JOYCNT_ADDR))
#define GBA_REG_JOYRE           (*(volatile unsigned int   *)(GBA_REG_JOYRE_ADDR))
#define GBA_REG_JOYRE_L         (*(volatile unsigned short *)(GBA_REG_JOYRE_L_ADDR))
#define GBA_REG_JOY_RECV_L      (*(volatile unsigned short *)(GBA_REG_JOY_RECV_L_ADDR))
#define GBA_REG_JOYRE_H         (*(volatile unsigned short *)(GBA_REG_JOYRE_H_ADDR))
#define GBA_REG_JOY_RECV_H      (*(volatile unsigned short *)(GBA_REG_JOY_RECV_H_ADDR))
#define GBA_REG_JOYTR           (*(volatile unsigned int   *)(GBA_REG_JOYTR_ADDR))
#define GBA_REG_JOYTR_L         (*(volatile unsigned short *)(GBA_REG_JOYTR_L_ADDR))
#define GBA_REG_JOY_TRANS_L     (*(volatile unsigned short *)(GBA_REG_JOY_TRANS_L_ADDR))
#define GBA_REG_JOYTR_H         (*(volatile unsigned short *)(GBA_REG_JOYTR_H_ADDR))
#define GBA_REG_JOY_TRANS_H     (*(volatile unsigned short *)(GBA_REG_JOY_TRANS_H_ADDR))
#define GBA_REG_JSTAT           (*(volatile unsigned int   *)(GBA_REG_JSTAT_ADDR))
#define GBA_REG_JOYSTAT         (*(volatile unsigned short *)(GBA_REG_JOYSTAT_ADDR))
#endif

/*---------------------------------------------------------------------------+
|    INTERRUPT
+----------------------------------------------------------------------------*/
/* Interrupt sources */
#define GBA_INT_VBLANK          0x0001
#define GBA_INT_HBLANK          0x0002
#define GBA_INT_VCOUNT          0x0004
#define GBA_INT_TIMER0          0x0008
#define GBA_INT_TIMER1          0x0010
#define GBA_INT_TIMER2          0x0020
#define GBA_INT_TIMER3          0x0040
#define GBA_INT_SERIAL          0x0080
#define GBA_INT_DMA0            0x0100
#define GBA_INT_DMA1            0x0200
#define GBA_INT_DMA2            0x0400
#define GBA_INT_DMA3            0x0800
#define GBA_INT_KEY             0x1000
#define GBA_INT_CART            0x2000
/* Interrupt address offsets */
#define GBA_IE                  0x00000200 /**< Interrupt Enable        */
#define GBA_IF                  0x00000202 /**< Interrupt Flags         */
#define GBA_IME                 0x00000208 /**< Interrupt Master Enable */
/* Interrupt addresses */
#define GBA_REG_IE_ADDR         GBA_IO_REGS_ADDR + GBA_IE
#define GBA_REG_IF_ADDR         GBA_IO_REGS_ADDR + GBA_IF
#define GBA_REG_IME_ADDR        GBA_IO_REGS_ADDR + GBA_IME
/* Interrupt registers */
#ifndef __asm__
#define GBA_REG_IE              (*(volatile unsigned short *)(GBA_REG_IE_ADDR))
#define GBA_REG_IF              (*(volatile unsigned short *)(GBA_REG_IF_ADDR))
#define GBA_REG_IME             (*(volatile unsigned short *)(GBA_REG_IME_ADDR))
#endif

/*---------------------------------------------------------------------------+
|    Waitstate and Power-Down Control registers
+----------------------------------------------------------------------------*/
#define GBA_PHI_MASK            0x1800
#define GBA_PHI_NONE            0x0000
#define GBA_PHI_4_19MHZ         0x0800
#define GBA_PHI_8_38MHZ         0x1000
#define GBA_PHI_16_76MHZ        0x1800
/* Waitstate and Power-Down Control address offsets */
#define GBA_WSCNT               0x00000204 /**< Waitstate Control           X*/
#define GBA_WAITCNT             0x00000204 /**< Waitstate Control            */
#define GBA_PAUSE               0x00000300 /**< Power Down Control          X*/
#define GBA_HALTCNT             0x00000300 /**< Power Down Control           */
/* Waitstate and Power-Down Control addresses */
#define GBA_REG_WSCNT_ADDR      GBA_IO_REGS_ADDR + GBA_WSCNT
#define GBA_REG_WAITCNT_ADDR    GBA_IO_REGS_ADDR + GBA_WAITCNT
#define GBA_REG_PAUSE_ADDR      GBA_IO_REGS_ADDR + GBA_PAUSE
#define GBA_REG_HALTCNT_ADDR    GBA_IO_REGS_ADDR + GBA_HALTCNT
/* Waitstate and Power-Down Control registers */
#ifndef __asm__
#define GBA_REG_WSCNT           (*(volatile unsigned short *)(GBA_REG_WSCNT_ADDR))
#define GBA_REG_WAITCNT         (*(volatile unsigned short *)(GBA_REG_WAITCNT_ADDR))
#define GBA_REG_PAUSE           (*(volatile unsigned short *)(GBA_REG_PAUSE_ADDR))
#define GBA_REG_HALTCNT         (*(volatile unsigned short *)(GBA_REG_HALTCNT_ADDR))
#endif


/*
 * @TODO remove UART register defines
 * define for RTEMS UART registers to be able to compile
 */
#define RSRBR   0
#define RSTHR   1
#define RSIER   2
#define RSIIR   3
#define RSFCR   4
#define RSLCR   5
#define RSLSR   6
#define RSDLL   7
#define RSDLH   8
#define RSCNT   9


#endif /* _GBA_REGISTERS_H */
