/*
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
 *
 */

#include <rtems.h>

/*
 *  JRS - February 20, 1998
 *
 *  There is a swap32 in each port.  So this should be removed.
 *
 *  Adding a swap16 to the port would be useful.
 *
 *  The end of all this would be to remove this file.
 */

inline unsigned int Swap32(
        unsigned32 ulValue
)
{
        unsigned32 ulSwapped;

        asm volatile(
		"rlwimi %0,%1,8,24,31;"
		"rlwimi %0,%1,24,16,23;"
		"rlwimi %0,%1,8,8,15;"
		"rlwimi %0,%1,24,0,7;" :
		
		"=&r" ((ulSwapped)) :
		"r" ((ulValue))
	);

        return( ulSwapped );
}

inline unsigned int Swap16(
        unsigned16 usValue
)
{
        unsigned16 usSwapped;

        asm volatile(
		"rlwimi %0,%1,24,24,31;"
		"rlwimi %0,%1,8,16,23;" :

		"=&r" ((usSwapped)) :
		"r" ((usValue))
	);

        return( usSwapped );
}

