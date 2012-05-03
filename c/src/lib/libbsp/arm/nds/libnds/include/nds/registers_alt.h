/*---------------------------------------------------------------------------------
	Copyright (C) 2005
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)
		Chris Double (doublec)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.
	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.
	3.	This notice may not be removed or altered from any source
		distribution.

---------------------------------------------------------------------------------*/

/*
	This file should be deprecated.

	All hardware register defines should be replaced with REG_ for consistency and namespacing

	http://forum.gbadev.org/viewtopic.php?t=4993

*/
#ifndef	NDS_REGISTERS_H
#define	NDS_REGISTERS_H


#include <nds/jtypes.h>


#define	REG_DISPCNT		(*(vu32*)0x4000000)

#ifdef ARM9
#define WAIT_CR	REG_EXMEMCNT
#else
#define WAIT_CR	REG_EXMEMSTAT
#endif

#define DISP_SR			REG_DISPSTAT
#define DISP_Y			REG_VCOUNT

#define	REG_BGCTRL		( (vu16*)0x4000008)
#define	REG_BG0CNT		(*(vu16*)0x4000008)
#define	REG_BG1CNT		(*(vu16*)0x400000A)
#define	REG_BG2CNT		(*(vu16*)0x400000C)
#define	REG_BG3CNT		(*(vu16*)0x400000E)

#define	REG_BGOFFSETS	( (vu16*)0x4000010)
#define	REG_BG0HOFS		(*(vu16*)0x4000010)
#define	REG_BG0VOFS		(*(vu16*)0x4000012)
#define	REG_BG1HOFS		(*(vu16*)0x4000014)
#define	REG_BG1VOFS		(*(vu16*)0x4000016)
#define	REG_BG2HOFS		(*(vu16*)0x4000018)
#define	REG_BG2VOFS		(*(vu16*)0x400001A)
#define	REG_BG3HOFS		(*(vu16*)0x400001C)
#define	REG_BG3VOFS		(*(vu16*)0x400001E)

#define	REG_BG2PA		(*(vu16*)0x4000020)
#define	REG_BG2PB		(*(vu16*)0x4000022)
#define	REG_BG2PC		(*(vu16*)0x4000024)
#define	REG_BG2PD		(*(vu16*)0x4000026)

#define	REG_BG2X		(*(vu32*)0x4000028)
#define	REG_BG2X_L		(*(vu16*)0x4000028)
#define	REG_BG2X_H		(*(vu16*)0x400002A)

#define	REG_BG2Y		(*(vu32*)0x400002C)
#define	REG_BG2Y_L		(*(vu16*)0x400002C)
#define	REG_BG2Y_H		(*(vu16*)0x400002E)

#define	REG_BG3PA		(*(vu16*)0x4000030)
#define	REG_BG3PB		(*(vu16*)0x4000032)
#define	REG_BG3PC		(*(vu16*)0x4000034)
#define	REG_BG3PD		(*(vu16*)0x4000036)

#define	REG_BG3X		(*(vu32*)0x4000038)
#define	REG_BG3X_L		(*(vu16*)0x4000038)
#define	REG_BG3X_H		(*(vu16*)0x400003A)
#define	REG_BG3Y		(*(vu32*)0x400003C)
#define	REG_BG3Y_L		(*(vu16*)0x400003C)
#define	REG_BG3Y_H		(*(vu16*)0x400003E)

#define	REG_WIN0H		(*(vu16*)0x4000040)
#define	REG_WIN1H		(*(vu16*)0x4000042)
#define	REG_WIN0V		(*(vu16*)0x4000044)
#define	REG_WIN1V		(*(vu16*)0x4000046)
#define	REG_WININ		(*(vu16*)0x4000048)
#define	REG_WINOUT		(*(vu16*)0x400004A)

#define	REG_MOSAIC		(*(vu32*)0x400004C)
#define	REG_MOSAIC_L	(*(vu32*)0x400004C)
#define	REG_MOSAIC_H	(*(vu32*)0x400004E)

#define	REG_BLDMOD		(*(vu16*)0x4000050)
#define	REG_COLV		(*(vu16*)0x4000052)
#define	REG_COLY		(*(vu16*)0x4000054)

#define SERIAL_CR       REG_SPICNT
#define SERIAL_DATA     REG_SPIDATA
#define SIO_CR          REG_SIOCNT
#define R_CR            REG_RCNT

#define	DISP_CAPTURE	REG_DISPCAPCNT


/*	secondary screen */
#define	REG_DISPCNT_SUB		(*(vu32*)0x4001000)
#define	REG_BGCTRL_SUB		( (vu16*)0x4001008)
#define	REG_BG0CNT_SUB		(*(vu16*)0x4001008)
#define	REG_BG1CNT_SUB		(*(vu16*)0x400100A)
#define	REG_BG2CNT_SUB		(*(vu16*)0x400100C)
#define	REG_BG3CNT_SUB		(*(vu16*)0x400100E)

#define	REG_BGOFFSETS_SUB	( (vu16*)0x4001010)
#define	REG_BG0HOFS_SUB		(*(vu16*)0x4001010)
#define	REG_BG0VOFS_SUB		(*(vu16*)0x4001012)
#define	REG_BG1HOFS_SUB		(*(vu16*)0x4001014)
#define	REG_BG1VOFS_SUB		(*(vu16*)0x4001016)
#define	REG_BG2HOFS_SUB		(*(vu16*)0x4001018)
#define	REG_BG2VOFS_SUB		(*(vu16*)0x400101A)
#define	REG_BG3HOFS_SUB		(*(vu16*)0x400101C)
#define	REG_BG3VOFS_SUB		(*(vu16*)0x400101E)

#define	REG_BG2PA_SUB		(*(vu16*)0x4001020)
#define	REG_BG2PB_SUB		(*(vu16*)0x4001022)
#define	REG_BG2PC_SUB		(*(vu16*)0x4001024)
#define	REG_BG2PD_SUB		(*(vu16*)0x4001026)

#define	REG_BG2X_SUB		(*(vu32*)0x4001028)
#define	REG_BG2Y_SUB		(*(vu32*)0x400102C)

#define	REG_BG3PA_SUB		(*(vu16*)0x4001030)
#define	REG_BG3PB_SUB		(*(vu16*)0x4001032)
#define	REG_BG3PC_SUB		(*(vu16*)0x4001034)
#define	REG_BG3PD_SUB		(*(vu16*)0x4001036)

#define	REG_BG3X_SUB		(*(vu32*)0x4001038)
#define	REG_BG3X_L_SUB		(*(vu16*)0x4001038)
#define	REG_BG3X_H_SUB		(*(vu16*)0x400103A)
#define	REG_BG3Y_SUB		(*(vu32*)0x400103C)
#define	REG_BG3Y_L_SUB		(*(vu16*)0x400103C)
#define	REG_BG3Y_H_SUB		(*(vu16*)0x400103E)

#define	REG_WIN0H_SUB		(*(vu16*)0x4001040)
#define	REG_WIN1H_SUB		(*(vu16*)0x4001042)
#define	REG_WIN0V_SUB		(*(vu16*)0x4001044)
#define	REG_WIN1V_SUB		(*(vu16*)0x4001046)
#define	REG_WININ_SUB		(*(vu16*)0x4001048)
#define	REG_WINOUT_SUB		(*(vu16*)0x400104A)

#define	REG_MOSAIC_SUB		(*(vu32*)0x400104C)
#define	REG_MOSAIC_L_SUB	(*(vu32*)0x400104C)
#define	REG_MOSAIC_H_SUB	(*(vu32*)0x400104E)

#define	REG_BLDMOD_SUB	(*(vu16*)0x4001050)
#define	REG_COLV_SUB	(*(vu16*)0x4001052)
#define	REG_COLY_SUB	(*(vu16*)0x4001054)

/*common*/
#define	REG_DMA			( (vu32*)0x40000B0)

#define	REG_DMA0SAD		(*(vu32*)0x40000B0)
#define	REG_DMA0SAD_L	(*(vu16*)0x40000B0)
#define	REG_DMA0SAD_H	(*(vu16*)0x40000B2)
#define	REG_DMA0DAD		(*(vu32*)0x40000B4)
#define	REG_DMA0DAD_L	(*(vu16*)0x40000B4)
#define	REG_DMA0DAD_H	(*(vu16*)0x40000B6)
#define	REG_DMA0CNT		(*(vu32*)0x40000B8)
#define	REG_DMA0CNT_L	(*(vu16*)0x40000B8)
#define	REG_DMA0CNT_H	(*(vu16*)0x40000BA)

#define	REG_DMA1SAD		(*(vu32*)0x40000BC)
#define	REG_DMA1SAD_L	(*(vu16*)0x40000BC)
#define	REG_DMA1SAD_H	(*(vu16*)0x40000BE)
#define	REG_DMA1DAD		(*(vu32*)0x40000C0)
#define	REG_DMA1DAD_L	(*(vu16*)0x40000C0)
#define	REG_DMA1DAD_H	(*(vu16*)0x40000C2)
#define	REG_DMA1CNT		(*(vu32*)0x40000C4)
#define	REG_DMA1CNT_L	(*(vu16*)0x40000C4)
#define	REG_DMA1CNT_H	(*(vu16*)0x40000C6)

#define	REG_DMA2SAD		(*(vu32*)0x40000C8)
#define	REG_DMA2SAD_L	(*(vu16*)0x40000C8)
#define	REG_DMA2SAD_H	(*(vu16*)0x40000CA)
#define	REG_DMA2DAD		(*(vu32*)0x40000CC)
#define	REG_DMA2DAD_L	(*(vu16*)0x40000CC)
#define	REG_DMA2DAD_H	(*(vu16*)0x40000CE)
#define	REG_DMA2CNT		(*(vu32*)0x40000D0)
#define	REG_DMA2CNT_L	(*(vu16*)0x40000D0)
#define	REG_DMA2CNT_H	(*(vu16*)0x40000D2)

#define	REG_DMA3SAD		(*(vu32*)0x40000D4)
#define	REG_DMA3SAD_L	(*(vu16*)0x40000D4)
#define	REG_DMA3SAD_H	(*(vu16*)0x40000D6)
#define	REG_DMA3DAD		(*(vu32*)0x40000D8)
#define	REG_DMA3DAD_L	(*(vu16*)0x40000D8)
#define	REG_DMA3DAD_H	(*(vu16*)0x40000DA)
#define	REG_DMA3CNT		(*(vu32*)0x40000DC)
#define	REG_DMA3CNT_L	(*(vu16*)0x40000DC)
#define	REG_DMA3CNT_H	(*(vu16*)0x40000DE)

#define	REG_TIME		( (vu16*)0x4000100)
#define	REG_TM0D		(*(vu16*)0x4000100)
#define	REG_TM0CNT		(*(vu16*)0x4000102)
#define	REG_TM1D		(*(vu16*)0x4000106)
#define	REG_TM2D		(*(vu16*)0x4000108)
#define	REG_TM2CNT		(*(vu16*)0x400010A)
#define	REG_TM3D		(*(vu16*)0x400010C)
#define	REG_TM3CNT		(*(vu16*)0x400010E)


#define	REG_SIOCNT		(*(vu16*)0x4000128)
#define	REG_SIOMLT_SEND	(*(vu16*)0x400012A)

#define	KEYS		REG_KEYINPUT
#define	KEYS_CR		REG_KEYCNT
//???
#define	REG_RCNT		(*(vu16*)0x4000134)
#define	REG_HS_CTRL		(*(vu16*)0x4000140)

/* Interupt	enable registers */
#define	IE			REG_IE
#define	IF			REG_IF
#define	IME			REG_IME

/*controls power  0x30f	is all on */
#define POWER_CR	REG_POWERCNT

/* ram	controllers	0x8	is enabled,	other bits have	to do with mapping */
#define	REG_VRAM_A_CR	(*(vu8*) 0x4000240)
#define	REG_VRAM_B_CR	(*(vu8*) 0x4000241)
#define	REG_VRAM_C_CR	(*(vu8*) 0x4000242)
#define	REG_VRAM_D_CR	(*(vu8*) 0x4000243)
#define	REG_VRAM_E_CR	(*(vu8*) 0x4000244)
#define	REG_VRAM_F_CR	(*(vu8*) 0x4000245)
#define	REG_VRAM_G_CR	(*(vu8*) 0x4000246)
#define	REG_VRAM_H_CR	(*(vu8*) 0x4000248)
#define	REG_VRAM_I_CR	(*(vu8*) 0x4000249)
#define	REG_WRAM_CNT	(*(vu8*) 0x4000247)




/*3D graphics suff*/
#define	REG_GFX_FIFO		(*(vu32*) 0x4000400)
#define	REG_GFX_STATUS		(*(vu32*) 0x4000600)
#define	REG_GFX_CONTROL		(*(vu16*) 0x4000060)
#define	REG_COLOR			(*(vu32*) 0x4000480)
#define	REG_VERTEX16		(*(vu32*) 0x400048C)
#define	REG_TEXT_COORD		(*(vu32*) 0x4000488)
#define	REG_TEXT_FORMAT		(*(vu32*) 0x40004A8)


#define	REG_CLEAR_COLOR		(*(vu32*) 0x4000350)
#define	REG_CLEAR_DEPTH		(*(vu16*) 0x4000354)

#define	REG_LIGHT_VECTOR	(*(vu32*) 0x40004C8)
#define	REG_LIGHT_COLOR		(*(vu32*) 0x40004CC)
#define	REG_NORMAL			(*(vu32*) 0x4000484)

#define	REG_DIFFUSE_AMBIENT		(*(vu32*) 0x40004C0)
#define	REG_SPECULAR_EMISSION	(*(vu32*) 0x40004C4)
#define	REG_SHININESS			(*(vu32*) 0x40004D0)

#define	REG_POLY_FORMAT		(*(vu32*) 0x40004A4)

#define	REG_GFX_BEGIN		(*(vu32*) 0x4000500)
#define	REG_GFX_END			(*(vu32*) 0x4000504)
#define	REG_GFX_FLUSH		(*(vu32*) 0x4000540)
#define	REG_GFX_VIEWPORT	(*(vu32*) 0x4000580)

#define	REG_MTX_CONTROL		(*(vu32*) 0x4000440)
#define	REG_MTX_PUSH		(*(vu32*) 0x4000444)
#define	REG_MTX_POP			(*(vu32*) 0x4000448)
#define	REG_MTX_SCALE		(*(vint32*) 0x400046C)
#define	REG_MTX_TRANSLATE	(*(vint32*) 0x4000470)
#define	REG_MTX_RESTORE		(*(vu32*) 0x4000450)
#define	REG_MTX_STORE		(*(vu32*) 0x400044C)
#define	REG_MTX_IDENTITY	(*(vu32*) 0x4000454)
#define	REG_MTX_LOAD4x4		(*(volatile	f32*) 0x4000458)
#define	REG_MTX_LOAD4x3		(*(volatile	f32*) 0x400045C)
#define	REG_MTX_MULT4x4		(*(volatile	f32*) 0x4000460)
#define	REG_MTX_MULT4x3		(*(volatile	f32*) 0x4000464)
#define	REG_MTX_MULT3x3		(*(volatile	f32*) 0x4000468)

// Card	bus

#define	REG_CARD_CR1		(*(vu32*)0x040001A0)
#define	REG_CARD_CR1H		(*(vu8*)0x040001A1)
#define	REG_CARD_CR2		(*(vu32*)0x040001A4)
#define	REG_CARD_COMMAND	((vu8*)0x040001A8)

#define	REG_CARD_DATA		(*(vu32*)0x04100000)
#define	REG_CARD_DATA_RD	(*(vu32*)0x04100010)

#define	REG_CARD_1B0		(*(vu32*)0x040001B0)
#define	REG_CARD_1B4		(*(vu32*)0x040001B4)
#define	REG_CARD_1B8		(*(vu16*)0x040001B8)
#define	REG_CARD_1BA		(*(vu16*)0x040001BA)
#endif
