//////////////////////////////////////////////////////////////////////
//
// reload.h -- Provides an ability to return to the main menu when used
//				with darkains loader or compatible system.
//
//	Contributed by Darkain
//
// version 0.1, June 5, 2005
//
//  Copyright (C) 2005 Michael Noland (joat) and Jason Rogers (dovoto)
//
//  This software is provided 'as-is', without any express or implied
//  warranty.  In no event will the authors be held liable for any
//  damages arising from the use of this software.
//
//  Permission is granted to anyone to use this software for any
//  purpose, including commercial applications, and to alter it and
//  redistribute it freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you
//     must not claim that you wrote the original software. If you use
//     this software in a product, an acknowledgment in the product
//     documentation would be appreciated but is not required.
//  2. Altered source versions must be plainly marked as such, and
//     must not be misrepresented as being the original software.
//  3. This notice may not be removed or altered from any source
//     distribution.
//
// Changelog:
//   0.1: First version
//
//////////////////////////////////////////////////////////////////////


#ifndef _NDS_LOAD_H_
#define _NDS_LOAD_H_


typedef void __attribute__ ((long_call)) (*COPYFUNC)(const char*);
typedef u32  __attribute__ ((long_call)) (*QUERYFUNC)(u32);

typedef struct {
  u32         VERSION;   // contains version information about loader
  QUERYFUNC   QUERY;     // used to retreive extended information from loader
  COPYFUNC    ARM7FUNC;  // a pointer to the ARM7 load function
  COPYFUNC    ARM9FUNC;  // a pointer to the ARM9 load function
  const char *PATH;      // THIS VALUE IS SET FOR YOU, DONT TOUCH IT
  u32         RESERVED;  // reserved for future expansion
} LOADER_DATA;

#define LOADNDS ((volatile LOADER_DATA*)(0x02800000-48))
#define BOOT_NDS ((const char*)-1)



/*
dump this near the begining of your ARM7's main function

LOADNDS->PATH = 0;


inside of your ARM7's main loop or VBlank IRQ or what-ever, add this code:

if (LOADNDS->PATH != 0) {
  LOADNDS->ARM7FUNC(LOADNDS->PATH);
}


//////////////////////////////////////

in your ARM9 code, to return to the menu, call this function:

WAIT_CR &= ~0x8080;
LOADNDS->ARM9FUNC(BOOT_NDS);

*/


#endif //_NDS_LOAD_H_
