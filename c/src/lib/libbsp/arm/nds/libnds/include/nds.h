/*---------------------------------------------------------------------------------
	Copyright (C) 2005
		Michael Noland (joat)
		Jason Rogers (dovoto)
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

#ifndef NDS_INCLUDE
#define NDS_INCLUDE

#ifndef ARM7
#ifndef ARM9
#error Either ARM7 or ARM9 must be defined
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <nds/jtypes.h>
#include <nds/bios.h>
#include <nds/card.h>
#include <nds/dma.h>
#include <nds/interrupts.h>
#include <nds/ipc.h>
#include <nds/memory.h>
#include <nds/system.h>
#include <nds/timers.h>

//---------------------------------------------------------------------------------
#ifdef ARM9
//---------------------------------------------------------------------------------

#include <nds/arm9/background.h>
#include <nds/arm9/boxtest.h>
#include <nds/arm9/cache.h>
#include <nds/arm9/console.h>
#include <nds/arm9/exceptions.h>
#include <nds/arm9/image.h>
#include <nds/arm9/input.h>
#include <nds/arm9/math.h>
#include <nds/arm9/pcx.h>
#include <nds/arm9/rumble.h>
#include <nds/arm9/sound.h>
#include <nds/arm9/trig_lut.h>
#include <nds/arm9/video.h>
#include <nds/arm9/videoGL.h>
#include <nds/arm9/sprite.h>

#endif // #ifdef ARM9

//---------------------------------------------------------------------------------
#ifdef ARM7
//---------------------------------------------------------------------------------

#include <nds/arm7/audio.h>
#include <nds/arm7/clock.h>
#include <nds/arm7/serial.h>
#include <nds/arm7/touch.h>

#endif // #ifdef ARM7


#ifdef __cplusplus
}
#endif


#endif // NDS_INCLUDE


