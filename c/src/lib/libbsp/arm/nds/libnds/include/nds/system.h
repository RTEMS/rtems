/*---------------------------------------------------------------------------------
	Power control, keys, and HV clock registers

	Copyright (C) 2005
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

//!	NDS hardware definitions.
/*!	\file system.h

	These definitions are usually only touched during
	the initialization of the program.
*/

#ifndef NDS_SYSTEM_INCLUDE
#define NDS_SYSTEM_INCLUDE

//!	LCD status register.
#define	REG_DISPSTAT	(*(vu16*)0x04000004)

//!	The display currently in a vertical blank.
#define DISP_IN_VBLANK     BIT(0)

//!	The display currently in a horizontal blank.
#define DISP_IN_HBLANK     BIT(1)

//!	Current scanline and %DISP_Y match.
#define DISP_YTRIGGERED    BIT(2)

//!	Interrupt on vertical blank.
#define DISP_VBLANK_IRQ    BIT(3)

//!	Interrupt on horizontal blank.
#define DISP_HBLANK_IRQ    BIT(4)

//!	Interrupt when current scanline and %DISP_Y match.
#define DISP_YTRIGGER_IRQ  BIT(5)

static inline
void SetYtrigger(int Yvalue) {
	REG_DISPSTAT = (REG_DISPSTAT & 0x007F ) | (Yvalue << 8) | (( Yvalue & 0x100 ) >> 2) ;
}

//!	Current display scanline.
#define	REG_VCOUNT		(*(vu16*)0x4000006)


//!	Halt control register.
/*!	Writing 0x40 to HALT_CR activates GBA mode.
	%HALT_CR can only be accessed via the BIOS.
*/
#define HALT_CR       (*(vuint16*)0x04000300)

//!	Power control register.
/*!	This register controls what hardware should
	be turned on or off.
*/
#define	REG_POWERCNT	*(vu16*)0x4000304

//!	Turns on specified hardware.
/*!	This function should only be called after %powerSET.

	\param on What to power on.
*/
static inline
void powerON(int on) { REG_POWERCNT |= on;}

//!	Turns on only the specified hardware.
/*!	Use this function to power on basic hardware types you
	wish to use throughout your program.

	\param on What to power on.
*/
static inline void powerSET(int on) { REG_POWERCNT = on;}

//!	Turns off the specified hardware.
/*!	\param off What to power off.
*/
static inline void powerOFF(int off) { REG_POWERCNT &= ~off;}

#ifdef ARM9
#ifdef DOXYGEN
//!	Power-controlled hardware devices accessable to the ARM9.
/*!	Used with %powerON, %powerSET, and %powerOFF functions.
	Note that these should only be used when programming for
	the ARM9.  Trying to boot up these hardware devices via
	the ARM7 would lead to unexpected results.
*/
enum ARM9_power
{
	POWER_LCD,			//!<	Controls the power for both LCD screens.
	POWER_2D_A,			//!<	Controls the power for the main 2D core.
	POWER_MATRIX,		//!<	Controls the power for the 3D matrix.
	POWER_3D_CORE,		//!<	Controls the power for the main 3D core.
	POWER_2D_B,			//!<	Controls the power for the sub 2D core.
	POWER_SWAP_LCDS,	//!<	Controls which screen should use the main core.
};
#else
#define POWER_LCD			BIT(0)
#define POWER_2D_A			BIT(1)
#define POWER_MATRIX		BIT(2)
#define POWER_3D_CORE		BIT(3)
#define POWER_2D_B			BIT(9)
#define POWER_SWAP_LCDS		BIT(15)
#endif /* DOXYGEN */

//!	Enables power to all hardware required for 2D video.
#define POWER_ALL_2D     (POWER_LCD |POWER_2D_A |POWER_2D_B)

//!	Enables power to all hardware required for 3D video.
#define POWER_ALL		 (POWER_ALL_2D | POWER_3D_CORE | POWER_MATRIX)

//!	Switches the screens.
static inline void lcdSwap(void) { REG_POWERCNT ^= POWER_SWAP_LCDS; }

//!	Forces the main core to display on the top.
static inline void lcdMainOnTop(void) { REG_POWERCNT |= POWER_SWAP_LCDS; }

//!	Forces the main core to display on the bottom.
static inline void lcdMainOnBottom(void) { REG_POWERCNT &= ~POWER_SWAP_LCDS; }
#endif

#ifdef ARM7
#ifdef DOXYGEN
//!	Power-controlled hardware devices accessable to the ARM7.
/*!	Note that these should only be used when programming for
	the ARM7.  Trying to boot up these hardware devices via
	the ARM9 would lead to unexpected results.
*/
enum ARM7_power
{
	POWER_SOUND,	//!<	Controls the power for the sound controller.
	POWER_UNKNOWN,	//!<	Controls the power for an unknown device.
};
#else
#define POWER_SOUND       BIT(0)
#define POWER_UNKNOWN     BIT(1)
#endif /* DOXYGEN */

void readUserSettings(void);

#endif /* ARM7 */

//!	User's DS settings.
/*!	\struct tPERSONAL_DATA

	Defines the structure the DS firmware uses for transfer
	of the user's settings to the booted program.
*/
typedef struct tPERSONAL_DATA {
  u8 RESERVED0[2];			//!<	??? (0x05 0x00).

  u8 theme;					//!<	The user's theme color (0-15).
  u8 birthMonth;			//!<	The user's birth month (1-12).
  u8 birthDay;				//!<	The user's birth day (1-31).

  u8 RESERVED1[1];			//!<	???

  s16 name[10];				//!<	The user's name in UTF-16 format.
  u16 nameLen;				//!<	The length of the user's name in characters.

  s16 message[26];			//!<	The user's message.
  u16 messageLen;			//!<	The length of the user's message in characters.

  u8 alarmHour;				//!<	What hour the alarm clock is set to (0-23).
  u8 alarmMinute;			//!<	What minute the alarm clock is set to (0-59).
            //0x027FFCD3  alarm minute

  u8 RESERVED2[4];			//!<	???
           //0x027FFCD4  ??

  u16 calX1;				//!<	Touchscreen calibration: first X touch
  u16 calY1;				//!<	Touchscreen calibration: first Y touch
  u8 calX1px;				//!<	Touchscreen calibration: first X touch pixel
  u8 calY1px;				//!<	Touchscreen calibration: first X touch pixel

  u16 calX2;				//!<	Touchscreen calibration: second X touch
  u16 calY2;				//!<	Touchscreen calibration: second Y touch
  u8 calX2px;				//!<	Touchscreen calibration: second X touch pixel
  u8 calY2px;				//!<	Touchscreen calibration: second Y touch pixel

  packed_struct {
    unsigned language			: 3;	//!<	User's language.
    unsigned gbaScreen			: 1;	//!<	GBA screen selection (lower screen if set, otherwise upper screen).
    unsigned defaultBrightness	: 2;	//!<	Brightness level at power on, dslite.
    unsigned autoMode			: 1;	//!<	The DS should boot from the DS cart or GBA cart automatically if one is inserted.
    unsigned RESERVED1			: 2;	//!<	???
	unsigned settingsLost		: 1;	//!<	User Settings Lost (0=Normal, 1=Prompt/Settings Lost)
	unsigned RESERVED2			: 6;	//!<	???
  } _user_data;

  u16	RESERVED3;
  u32	rtcOffset;
  u32	RESERVED4;
} PACKED PERSONAL_DATA ;

//!	Key input register.
/*!	On the ARM9, the hinge "button," the touch status, and the
	X and Y buttons cannot be accessed directly.
*/
#define	REG_KEYINPUT	(*(vuint16*)0x04000130)

//!	Key input control register.
#define	REG_KEYCNT		(*(vuint16*)0x04000132)

//!	Default location for the user's personal data (see %PERSONAL_DATA).
#define PersonalData ((PERSONAL_DATA*)0x27FFC80)

//!	argv structure
/*!	\struct __argv

	structure used to set up argc/argv on the DS

*/
struct __argv {
	int argvMagic;		//!< argv magic number, set to 0x5f617267 ('_arg') if valid
	char *commandLine;	//!< base address of command line, set of null terminated strings
	int length;			//!< total length of command line
};

//!	Default location for the libnds argv structure.
#define libnds_argv		((struct __argv *)0x027FFF70)

// argv struct magic number
#define argvMagic 0x5f617267

#endif
