/*---------------------------------------------------------------------------------
	Copyright (C) 2005
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)
		Mike Parks (BigRedPimp)
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.
	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you
     must not claim that you wrote the original software. If you use
     this software in a product, an acknowledgment in the product
     documentation would be appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and
     must not be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source
     distribution.

---------------------------------------------------------------------------------*/
#include <nds/jtypes.h>
#include <nds/memory.h>
#include <nds/arm9/rumble.h>

static RUMBLE_TYPE rumbleType;

//---------------------------------------------------------------------------------
bool isRumbleInserted(void) {
//---------------------------------------------------------------------------------
	uint16 four[4];
	sysSetCartOwner(BUS_OWNER_ARM9);
	// First, check for 0x96 to see if it's a GBA game
	if (GBA_HEADER.is96h == 0x96) {

		//if it is a game, we check the game code
		//to see if it is warioware twisted
		if (	(GBA_HEADER.gamecode[0] == 'R') &&
				(GBA_HEADER.gamecode[1] == 'Z') &&
				(GBA_HEADER.gamecode[2] == 'W') &&
				(GBA_HEADER.gamecode[3] == 'E')
			)
		{
			rumbleType = WARIOWARE;
			WARIOWARE_ENABLE = 8;
			return true;
		}
		return false;

	} else {

		rumbleType = RUMBLE;
		// Now check to see if it's true open bus, or if D1 is pulled low
		four[0] = GBA_BUS[0] & 0xFF;
		four[1] = GBA_BUS[1] & 0xFF;
		four[2] = GBA_BUS[2] & 0xFF;
		four[3] = GBA_BUS[3] & 0xFF;
		return (four[0] == 0x00) && (four[2] == 0x00) && (four[1] == 0x01) && (four[3] == 0x01);
	}
}
//---------------------------------------------------------------------------------
void setRumble(bool position) {
//---------------------------------------------------------------------------------

	if( rumbleType == WARIOWARE) {
		WARIOWARE_PAK = (position ? 8 : 0);
	} else {
		RUMBLE_PAK = (position ? 2 : 0);
	}

}
