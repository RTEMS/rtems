/*---------------------------------------------------------------------------------

	ARM7 realtime clock

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

#ifndef ARM7_CLOCK_INCLUDE
#define ARM7_CLOCK_INCLUDE


#ifndef ARM7
#error The clock is only available on the ARM7
#endif

#ifdef __cplusplus
extern "C" {
#endif


#include <nds/arm7/serial.h>

// RTC registers
#define WRITE_STATUS_REG1  0x60
#define READ_STATUS_REG1   0x61

/*
  Status Register 1
    0   W   Reset                (0=Normal, 1=Reset)
    1   R/W 12/24 hour mode      (0=12 hour, 1=24 hour)
    2-3 R/W General purpose bits
    4   R   Interrupt 1 Flag (1=Yes)                      ;auto-cleared on read
    5   R   Interrupt 2 Flag (1=Yes)                      ;auto-cleared on read
    6   R   Power Low Flag (0=Normal, 1=Power is/was low) ;auto-cleared on read
    7   R   Power Off Flag (0=Normal, 1=Power was off)    ;auto-cleared on read
    Power off indicates that the battery was removed or fully discharged,
    all registers are reset to 00h (or 01h), and must be re-initialized.
*/
#define STATUS_POC       (1<<7)  // read-only, cleared by reading (1 if just powered on)
#define STATUS_BLD       (1<<6)  // read-only, cleared by reading (1 if power dropped below the safety threshold)
#define STATUS_INT2      (1<<5)  // read-only, INT2 has occured
#define STATUS_INT1      (1<<4)  // read-only, INT1 has occured
#define STATUS_SC1       (1<<3)  // R/W scratch bit
#define STATUS_SC0       (1<<2)  // R/W scratch bit
#define STATUS_24HRS     (1<<1)  // 24 hour mode when 1, 12 hour mode when 0
#define STATUS_RESET     (1<<0)  // write-only, reset when 1 written

#define WRITE_STATUS_REG2  0x62
#define READ_STATUS_REG2   0x63
/*
  Status Register 2
    0-3 R/W INT1 Mode/Enable
            0000b Disable
            0x01b Selected Frequency steady interrupt
            0x10b Per-minute edge interrupt
            0011b Per-minute steady interrupt 1 (duty 30.0 secomds)
            0100b Alarm 1 interrupt
            0111b Per-minute steady interrupt 2 (duty 0.0079 secomds)
            1xxxb 32kHz output
    4-5 R/W General purpose bits
    6   R/W INT2 Enable
            0b    Disable
            1b    Alarm 2 interrupt
    7   R/W Test Mode (0=Normal, 1=Test, don't use) (cleared on Reset)
*/
#define STATUS_TEST      (1<<7)  //
#define STATUS_INT2AE    (1<<6)  //
#define STATUS_SC3       (1<<5)  // R/W scratch bit
#define STATUS_SC2       (1<<4)  // R/W scratch bit

#define STATUS_32kE      (1<<3)  // Interrupt mode bits
#define STATUS_INT1AE    (1<<2)  //
#define STATUS_INT1ME    (1<<1)  //
#define STATUS_INT1FE    (1<<0)  //

// full 7 bytes for time and date
#define WRITE_TIME_AND_DATE	0x64
#define READ_TIME_AND_DATE	0x65

// last 3 bytes of current time
#define WRITE_TIME    0x66
#define READ_TIME	  0x67

#define WRITE_INT_REG1     0x68
#define READ_INT_REG1      0x69

#define READ_INT_REG2      0x6A
#define WRITE_INT_REG2     0x6B

#define READ_CLOCK_ADJUST_REG  0x6C
#define WRITE_CLOCK_ADJUST_REG 0x6D
// clock-adjustment register

#define READ_FREE_REG      0x6E
#define WRITE_FREE_REG     0x6F


void rtcReset(void);
void rtcTransaction(uint8 * command, uint32 commandLength, uint8 * result, uint32 resultLength);

void rtcGetTime(uint8 * time);
void rtcSetTime(uint8 * time);

void rtcGetTimeAndDate(uint8 * time);
void rtcSetTimeAndDate(uint8 * time);

void rtcGetData(uint8 * data, uint32 size);

void BCDToInteger(uint8 * data, uint32 length);
void integerToBCD(uint8 * data, uint32 length);

void initClockIRQ(void);

#ifdef __cplusplus
}
#endif

#endif

