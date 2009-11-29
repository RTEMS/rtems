/*
 filetime.c
 Conversion of file time and date values to various other types

 Copyright (c) 2006 Michael "Chishm" Chisholm

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.
  3. The name of the author may not be used to endorse or promote products derived
     from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	2006-07-11 - Chishm
		* Original release

	2006-09-30 - Chishm
		* Validity checks performed on the time supplied by the IPC
		* Cleaned up magic numbers

	2006-10-01 - Chishm
		* Fixed incorrect use of bitwise-or instead of logical-or
*/


#include "filetime.h"

#ifdef NDS
#include <nds/ipc.h>
#endif

#define HOUR_PM_INDICATOR 40

#define MAX_HOUR 23
#define MAX_MINUTE 59
#define MAX_SECOND 59

#define MAX_YEAR 99
#define MIN_YEAR 6		// The date is invalid if it's before this year
#define MAX_MONTH 12
#define MIN_MONTH 1
#define MAX_DAY 31
#define MIN_DAY 1

// Second values are averages, so time value won't be 100% accurate,
// but should be within the correct month.
#define SECONDS_PER_MINUTE 60
#define SECONDS_PER_HOUR 3600
#define SECONDS_PER_DAY 86400
#define SECONDS_PER_MONTH 2629743
#define SECONDS_PER_YEAR 31556926

u16 _FAT_filetime_getTimeFromRTC (void) {
#ifdef NDS
	int hour, minute, second;
	hour = (IPC->time.rtc.hours >= HOUR_PM_INDICATOR ? IPC->time.rtc.hours - HOUR_PM_INDICATOR : IPC->time.rtc.hours);
	minute = IPC->time.rtc.minutes;
	second = IPC->time.rtc.seconds;

	// Check that the values are all in range.
	// If they are not, return 0 (no timestamp)
	if ((hour < 0) || (hour > MAX_HOUR))	return 0;
	if ((minute < 0) || (minute > MAX_MINUTE)) return 0;
	if ((second < 0) || (second > MAX_SECOND)) return 0;

	return (
		((hour & 0x1F) << 11) |
		((minute & 0x3F) << 5) |
		((second >> 1) & 0x1F)
	);
#else
	return 0;
#endif
}


u16 _FAT_filetime_getDateFromRTC (void) {
#ifdef NDS
	int year, month, day;

	year = IPC->time.rtc.year;
	month = IPC->time.rtc.month;
	day = IPC->time.rtc.day;

	if ((year < MIN_YEAR) || (year > MAX_YEAR)) return 0;
	if ((month < MIN_MONTH) || (month > MAX_MONTH)) return 0;
	if ((day < MIN_DAY) || (day > MAX_DAY)) return 0;

	return (
		(((year + 20) & 0x7F) <<9) |	// Adjust for MS-FAT base year (1980 vs 2000 for DS clock)
		((month & 0xF) << 5) |
		(day & 0x1F)
	);
#else
	return 0;
#endif
}

time_t _FAT_filetime_to_time_t (u16 time, u16 date) {
	int hour, minute, second;
	int day, month, year;

	time_t result;

	hour = time >> 11;
	minute = (time >> 5) & 0x3F;
	second = (time & 0x1F) << 1;

	day = date & 0x1F;
	month = (date >> 5) & 0x0F;
	year = date >> 9;

	// Second values are averages, so time value won't be 100% accurate,
	// but should be within the correct month.
	result 	= second
			+ minute * SECONDS_PER_MINUTE
			+ hour * SECONDS_PER_HOUR
			+ day * SECONDS_PER_DAY
			+ month * SECONDS_PER_MONTH
			+ year * SECONDS_PER_YEAR
			;

	return result;
}
