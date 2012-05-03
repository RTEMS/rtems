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


/*! \file timers.h
    \brief Contains defines and macros for ARM7 and ARM9 timer operation.

	The timers are fed with a 33.4 MHz source on the ARM9.  The ARM7
	timing hasn't been tested yet, but is likely to be the same.
*/

//---------------------------------------------------------------------------------
#ifndef NDS_TIMERS_INCLUDE
#define NDS_TIMERS_INCLUDE
//---------------------------------------------------------------------------------


//---------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif
//---------------------------------------------------------------------------------


#include <nds/jtypes.h>

// Timers

//!	A macro that calculates %TIMER_DATA(n) settings for a given frequency of n.
/*!	Use the appropriate macro depending on clock divider:  \n
	<ul>
		<li>	%TIMER_FREQ(f) </li>
		<li>	%TIMER_FREQ_64(f) </li>
		<li>	%TIMER_FREQ_256(f) </li>
		<li>	%TIMER_FREQ_1024(f) </li>
	</ul>
	Clock divider is controlled by %TIMER_CR(n) \n
	\n
	<b>Example Usage:</b>

		%TIMER_DATA(0) = %TIMER_FREQ(freq_in_hz); \n
		%TIMER_CR(0) = %TIMER_DIV1 | %TIMER_ENABLE; \n
	\n
	Max frequency is: 33554432Hz\n
	Min frequency is: 512Hz\n
*/
#define TIMER_FREQ(n)    (-0x2000000/(n))

//!	A macro that calculates %TIMER_DATA(n) settings for a given frequency of n.
/*!	Use the appropriate macro depending on clock divider:  \n
	<ul>
		<li>	%TIMER_FREQ(f) </li>
		<li>	%TIMER_FREQ_64(f) </li>
		<li>	%TIMER_FREQ_256(f) </li>
		<li>	%TIMER_FREQ_1024(f) </li>
	</ul>


	Clock divider is controlled by %TIMER_CR(n) \n
	\n
	<b>Example Usage:</b>
		%TIMER_DATA(x) = %TIMER_FREQ_64(freq_in_hz)\n
	\n
	Max frequency is: 524288Hz\n
	Min frequency is: 8Hz\n
*/
#define TIMER_FREQ_64(n)  (-(0x2000000>>6)/(n))

//!	A macro that calculates %TIMER_DATA(n) settings for a given frequency of n.
/*!	Use the appropriate macro depending on clock divider:  \n
	<ul>
		<li>	%TIMER_FREQ(f) </li>
		<li>	%TIMER_FREQ_64(f) </li>
		<li>	%TIMER_FREQ_256(f) </li>
		<li>	%TIMER_FREQ_1024(f) </li>
	</ul> <p>

	Clock divider is controlled by %TIMER_CR(n) \n
	\n
	<b>Example Usage:</b>
		%TIMER_DATA(x) = %TIMER_FREQ_256(freq_in_hz)\n
	\n
	Max frequency is: 131072Hz\n
	Min frequency is: 2Hz\n
*/
#define TIMER_FREQ_256(n) (-(0x2000000>>8)/(n))

//!	A macro that calculates %TIMER_DATA(n) settings for a given frequency of n.
/*!	Use the appropriate macro depending on clock divider:  \n
	<ul>
		<li>	%TIMER_FREQ(f) </li>
		<li>	%TIMER_FREQ_64(f) </li>
		<li>	%TIMER_FREQ_256(f) </li>
		<li>	TIMER_FREQ_1024(f) </li>
	</ul>

	Clock divider is controlled by %TIMER_CR(n) \n
	\n
	<b>Example Usage:</b>
		%TIMER_DATA(x) = %TIMER_FREQ_1024(freq_in_hz)\n
	\n
	Max frequency is: 32768Hz\n
	Min frequency is: 0.5Hz\n
*/
#define TIMER_FREQ_1024(n) (-(0x2000000>>10)/(n))


//!	Same as %TIMER_DATA(0).
#define TIMER0_DATA    (*(vuint16*)0x04000100)
//!	Same as %TIMER_DATA(1).
#define TIMER1_DATA    (*(vuint16*)0x04000104)
//!	Same as %TIMER_DATA(2).
#define TIMER2_DATA    (*(vuint16*)0x04000108)
//!	Same as %TIMER_DATA(3).
#define TIMER3_DATA    (*(vuint16*)0x0400010C)

//!	Returns a dereferenced pointer to the data register for timer number "n".
/*!	\see TIMER_CR(n)
	\see TIMER_FREQ(n)

	%TIMER_DATA(n) when set will latch that value into the counter.  Everytime the
	counter rolls over %TIMER_DATA(0) will return to the latched value.  This allows
	you to control the frequency of the timer using the following formula:\n
		%TIMER_DATA(x) = -(0x2000000/(freq * divider)); \n
	\n
	<b>Example Usage:</b>
	%TIMER_DATA(0) = value;  were 0 can be 0 through 3 and value is 16 bits.
*/
#define TIMER_DATA(n)  (*(vuint16*)(0x04000100+((n)<<2)))

// Timer control registers
//!	Same as %TIMER_CR(0).
#define TIMER0_CR   (*(vuint16*)0x04000102)
//!	Same as %TIMER_CR(1).
#define TIMER1_CR   (*(vuint16*)0x04000106)
//!	Same as %TIMER_CR(2).
#define TIMER2_CR   (*(vuint16*)0x0400010A)
//!	Same as %TIMER_CR(3).
#define TIMER3_CR   (*(vuint16*)0x0400010E)

//!	Returns a dereferenced pointer to the data register for timer control Register.
/*!	<b>Example Usage:</b> %TIMER_CR(x) = %TIMER_ENABLE | %TIMER_DIV_64; \n
	\n
	Possible bit defines: \n

	\see TIMER_ENABLE
	\see TIMER_IRQ_REQ
	\see TIMER_DIV_1
	\see TIMER_DIV_64
	\see TIMER_DIV_256
	\see TIMER_DIV_1024
*/
#define TIMER_CR(n) (*(vuint16*)(0x04000102+((n)<<2)))

//!	Enables the timer.
#define TIMER_ENABLE    (1<<7)

//!	Causes the timer to request an Interupt on overflow.
#define TIMER_IRQ_REQ   (1<<6)

//!	When set will cause the timer to count when the timer below overflows (unavailable for timer 0).
#define TIMER_CASCADE   (1<<2)

//!	Causes the timer to count at 33.514Mhz.
#define TIMER_DIV_1     (0)
//!	Causes the timer to count at (33.514 / 64) Mhz.
#define TIMER_DIV_64    (1)
//!	Causes the timer to count at (33.514 / 256) Mhz.
#define TIMER_DIV_256   (2)
//!	Causes the timer to count at (33.514 / 1024)Mhz.
#define TIMER_DIV_1024  (3)

//---------------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------------

