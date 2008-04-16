/*---------------------------------------------------------------------------------
	$Id$

	Trig_lut.h provides access to external precompiled trig look up tables

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

	$Log$
	Revision 1.7  2005/08/23 17:06:10  wntrmute
	converted all endings to unix
	
	Revision 1.6  2005/08/01 23:18:22  wntrmute
	adjusted headers for logging
	
	Revision 1.5  2005/07/29 00:35:52  wntrmute
	update file header
	reference externs directly

---------------------------------------------------------------------------------*/
#ifndef TRIG_LUT_H
#define TRIG_LUT_H


extern short COS_bin[];
extern short SIN_bin[];
extern short TAN_bin[];


#define COS COS_bin
#define SIN SIN_bin
#define TAN TAN_bin


#endif 
