/*---------------------------------------------------------------------------------

	background.h -- definitions for DS backgrounds

	Copyright (C) 2007
		Dave Murphy (WinterMute)

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
#ifndef _libnds_background_h_
#define _libnds_background_h_

#include <nds/jtypes.h>

// Background control defines

// BGxCNT defines ///
#define BG_MOSAIC_ENABLE	0x40
#define BG_COLOR_256		0x80
#define BG_COLOR_16			0x00

#define CHAR_BASE_BLOCK(n)			(((n)*0x4000)+ 0x06000000)
#define CHAR_BASE_BLOCK_SUB(n)		(((n)*0x4000)+ 0x06200000)
#define SCREEN_BASE_BLOCK(n)		(((n)*0x800) + 0x06000000)
#define SCREEN_BASE_BLOCK_SUB(n)	(((n)*0x800) + 0x06200000)

#define CHAR_SHIFT        2
#define SCREEN_SHIFT      8
#define TEXTBG_SIZE_256x256    0x0
#define TEXTBG_SIZE_256x512    0x8000
#define TEXTBG_SIZE_512x256    0x4000
#define TEXTBG_SIZE_512x512    0xC000

#define ROTBG_SIZE_128x128    0x0
#define ROTBG_SIZE_256x256    0x4000
#define ROTBG_SIZE_512x512    0x8000
#define ROTBG_SIZE_1024x1024  0xC000

#define WRAPAROUND              0x1

#endif
