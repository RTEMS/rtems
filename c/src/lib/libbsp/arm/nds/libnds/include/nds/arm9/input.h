/*---------------------------------------------------------------------------------
	$Id$

	key input code -- provides slightly higher level input forming

	Copyright (C) 2005
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Christian Auby (DesktopMan)
		Dave Murphy (WinterMute)

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

	$Log$
	Revision 1.14  2006/02/25 02:18:53  wntrmute
	doxygenation updates
	
	Revision 1.13  2006/01/12 09:10:47  wntrmute
	Added key repeat as suggested by pepsiman
	
	Revision 1.12  2006/01/10 05:50:24  dovoto
	uhmm...about that KEY_R vs KEY_L thing...lets pretend that never happened
	
	Revision 1.11  2006/01/10 05:45:24  dovoto
	KEY_L and KEY_R were swapped
	
	Revision 1.10  2005/11/27 07:55:14  joatski
	Fixed my mistake in the changelogs
	
	Revision 1.9  2005/11/27 07:48:45  joatski
	Changed function returns to uint32
	
	Revision 1.8  2005/11/14 11:59:49  wntrmute
	reformat for consistency
	
	Revision 1.7  2005/11/03 23:38:49  wntrmute
	don't use enum for key function returns
	
	Revision 1.6  2005/10/18 04:17:04  wntrmute
	doxygenation
	
	Revision 1.5  2005/10/13 16:30:11  dovoto
	Changed KEYPAD_BITS to a typedef enum, this resolved some issues with multiple redefinition of KEYPAD_BITS (although this error did not allways occur).
	
	Revision 1.4  2005/10/03 21:21:21  wntrmute
	doxygenation
	
	Revision 1.3  2005/09/07 18:03:36  wntrmute
	renamed key input registers
	moved key bit definitions to input.h
	
	Revision 1.2  2005/08/23 17:06:10  wntrmute
	converted all endings to unix

	Revision 1.1  2005/08/03 17:37:28  wntrmute
	moved to input.h

	Revision 1.5  2005/08/01 23:18:22  wntrmute
	adjusted headers for logging

	Revision 1.4  2005/07/25 02:31:07  wntrmute
	made local variables static
	added proper header to keys.h

	Revision 1.3  2005/07/25 02:19:01  desktopman
	Added support for KEY_LID in keys.c.
	Moved KEYS_CUR from header to source file.
	Changed from the custom abs() to stdlib.h's abs().

	Revision 1.2  2005/07/14 08:00:57  wntrmute
	resynchronise with ndslib


---------------------------------------------------------------------------------*/
//!	NDS input support.
/*! \file input.h
*/

//---------------------------------------------------------------------------------
#ifndef	INPUT_HEADER_INCLUDE
#define	INPUT_HEADER_INCLUDE
//---------------------------------------------------------------------------------

#include <nds/jtypes.h>

// Keyboard

//!	Bit values for the keypad buttons.
typedef enum KEYPAD_BITS {
  KEY_A      = BIT(0),  //!< Keypad A button.
  KEY_B      = BIT(1),  //!< Keypad B button.
  KEY_SELECT = BIT(2),  //!< Keypad SELECT button.
  KEY_START  = BIT(3),  //!< Keypad START button.
  KEY_RIGHT  = BIT(4),  //!< Keypad RIGHT button.
  KEY_LEFT   = BIT(5),  //!< Keypad LEFT button.
  KEY_UP     = BIT(6),  //!< Keypad UP button.
  KEY_DOWN   = BIT(7),  //!< Keypad DOWN button.
  KEY_R      = BIT(8),  //!< Right shoulder button.
  KEY_L      = BIT(9),  //!< Left shoulder button.
  KEY_X      = BIT(10), //!< Keypad X button.
  KEY_Y      = BIT(11), //!< Keypad Y button.
  KEY_TOUCH  = BIT(12), //!< Touchscreen pendown.
  KEY_LID    = BIT(13)  //!< Lid state.
} KEYPAD_BITS;

#ifdef __cplusplus
extern "C" {
#endif

//!	Obtains the current keypad state.
/*!	Call this function once per main loop in order to use the keypad functions.
*/
void scanKeys();

//!	Obtains the current keypad held state.
uint32 keysHeld(void);

//!	Obtains the current keypad pressed state.
uint32 keysDown(void);

//!	Obtains the current keypad pressed or repeating state.
uint32 keysDownRepeat(void);

//!	Sets the key repeat parameters.
/*!	\param setDelay Number of %scanKeys calls before keys start to repeat.
	\param setRepeat Number of %scanKeys calls before keys repeat.
*/
void keysSetRepeat( u8 setDelay, u8 setRepeat );

//! Obtains the current keypad released state.
uint32 keysUp(void);

//!	Obtains the current touchscreen co-ordinates.
touchPosition touchReadXY();

#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------------
#endif // INPUT_HEADER_INCLUDE
//---------------------------------------------------------------------------------
