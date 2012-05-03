/*---------------------------------------------------------------------------------
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
void scanKeys(void);

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
touchPosition touchReadXY(void);

#ifdef __cplusplus
}
#endif

//---------------------------------------------------------------------------------
#endif // INPUT_HEADER_INCLUDE
//---------------------------------------------------------------------------------
