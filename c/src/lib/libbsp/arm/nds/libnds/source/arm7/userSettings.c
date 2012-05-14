/*---------------------------------------------------------------------------------
	Copyright (C) 2005
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

#include <nds/arm7/serial.h>
#include <nds/system.h>
#include <string.h>

//---------------------------------------------------------------------------------
void readUserSettings() {
//---------------------------------------------------------------------------------

	PERSONAL_DATA slot1;
	PERSONAL_DATA slot2;

	short slot1count, slot2count;
	short slot1CRC, slot2CRC;

	uint32 userSettingsBase;
	readFirmware( 0x20, &userSettingsBase,2);

	uint32 slot1Address = userSettingsBase * 8;
	uint32 slot2Address = userSettingsBase * 8 + 0x100;

	readFirmware( slot1Address , &slot1, sizeof(PERSONAL_DATA));
	readFirmware( slot2Address , &slot2, sizeof(PERSONAL_DATA));
	readFirmware( slot1Address + 0x70, &slot1count, 2);
	readFirmware( slot2Address + 0x70, &slot2count, 2);
	readFirmware( slot1Address + 0x72, &slot1CRC, 2);
	readFirmware( slot2Address + 0x72, &slot2CRC, 2);

	// default to slot 1 user Settings
	void *currentSettings = &slot1;

	short calc1CRC = swiCRC16( 0xffff, &slot1, sizeof(PERSONAL_DATA));
	short calc2CRC = swiCRC16( 0xffff, &slot2, sizeof(PERSONAL_DATA));

	// bail out if neither slot is valid
	if ( calc1CRC != slot1CRC && calc2CRC != slot2CRC) return;

	// if both slots are valid pick the most recent
	if ( calc1CRC == slot1CRC && calc2CRC == slot2CRC ) {
		currentSettings = (slot2count == (( slot2count + 1 ) & 0x7f) ? &slot2 : &slot1);
	} else {
		if ( calc2CRC == slot2CRC )
			currentSettings = &slot2;
	}
	memcpy ( PersonalData, currentSettings, sizeof(PERSONAL_DATA));

}
