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

#define MIN_YEAR 1996

#include <stdlib.h>
#include "bsp.h"
#include "cmos.h"
#include "../nvram/mk48t18.h"

#define Bin2BCD(Value) (((Value / 10) << 4) | (Value % 10))
#define BCD2Bin(BcdValue) ((BcdValue >> 4) * 10 + (BcdValue & 0x0f))

/*
 * Private types
 */
typedef
void
(*PTIMESET)
(
	rtems_time_of_day *pTOD
);

typedef
boolean
(*PTIMEGET)
(
	rtems_time_of_day *pTOD
);

typedef struct _TIME_ENTRY_TABLE
{
    PTIMESET	SetTime;
    PTIMEGET	GetTime;
} TIME_ENTRY_TABLE, *PTIME_ENTRY_TABLE;

/*
 * Private routines
 */

/*
 * DS1385 specific routines
 */
static void timeDsSet(rtems_time_of_day *pTOD);
static boolean timeDsGet(rtems_time_of_day *pTOD);

/*
 * MK48T18 specific routines
 */
static void timeMkSet(rtems_time_of_day *pTOD);
static boolean timeMkGet(rtems_time_of_day *pTOD);

TIME_ENTRY_TABLE timeDsTable =
{
	timeDsSet,
	timeDsGet
};

TIME_ENTRY_TABLE timeMkTable =
{
	timeMkSet,
	timeMkGet
};

/*
 * Private variables
 */
static PTIME_ENTRY_TABLE pTimeFunc;

/*
 * Mutual-exclusion semaphore
 */
static rtems_id	semRTC;

/*
 * This only works for the Gregorian calendar - i.e. after 1752 (in the UK)
 */
rtems_unsigned8 
GregorianDay(rtems_time_of_day *pTOD)
{
	boolean isLeap;
	unsigned long leapsToDate;
	unsigned long lastYear;
	unsigned long day;
	unsigned long MonthOffset[] = { 0, 31, 59, 90, 120, 151,
					181, 212, 243, 273, 304, 334 };

	lastYear=pTOD->year-1;

	/*
	 * Number of leap corrections to apply up to end of last year
	 */
	leapsToDate = lastYear/4 - lastYear/100 + lastYear/400;

	/*
	 * This year is a leap year if it is divisible by 4 except when it is
	 * divisible by 100 unless it is divisible by 400
	 *
	 * e.g. 1904 was a leap year, 1900 was not, 1996 is, and 2000 will be
	 */
	isLeap = (pTOD->year%4==0) &&
			 ((pTOD->year%100!=0) || (pTOD->year%400==0));

	if(isLeap && (pTOD->month>2))
	{
		day=1;
	}
	else
	{
		day=0;
	}

	day += lastYear*365 + leapsToDate + MonthOffset[pTOD->month-1] +
		   pTOD->day;

	return((rtems_unsigned8)(day%7));
}

void
DsWriteRawClockRegister (
	rtems_unsigned8 Register,
	rtems_unsigned8 Value
)

/*++

Routine Description:

    This routine reads the specified realtime clock register.

    This function was added to bridge the BCD format of the IBM roms
    and the binary formate of NT


Arguments:

    Register - Supplies the number of the register whose value is read.

Return Value:

    The value of the register is returned as the function value.

--*/

{
	outport_byte((rtems_unsigned8 *)RTC_PORT, Register & 0x7f);

	/* Read the realtime clock register value. */

	outport_byte((rtems_unsigned8 *)(RTC_PORT + 1), Value);
	return;
}

rtems_unsigned8
DsReadRawClockRegister (
	rtems_unsigned8 Register
)

/*++

Routine Description:

    This routine reads the specified realtime clock register.

    This function was added to bridge the BCD format of the IBM roms
    and the binary formate of NT


Arguments:

    Register - Supplies the number of the register whose value is read.

Return Value:

    The value of the register is returned as the function value.

--*/

{
	rtems_unsigned8 ucDataByte;

	outport_byte((rtems_unsigned8 *)RTC_PORT, Register & 0x7f);

	/* Read the realtime clock register value. */

	inport_byte((rtems_unsigned8 *)(RTC_PORT + 1), ucDataByte);
	return ucDataByte;
}

void
DsWriteClockRegister (
	rtems_unsigned8 Register,
	rtems_unsigned8 Value
)

/*++

Routine Description:

    This routine writes the specified value to the specified realtime
    clock register.

Arguments:

    Register - Supplies the number of the register whose value is written.

    Value - Supplies the value that is written to the specified register.

Return Value:

    The value of the register is returned as the function value.

--*/

{
	rtems_unsigned8 BcdValue;

	BcdValue = Bin2BCD(Value);
	DsWriteRawClockRegister(Register, BcdValue);
	return;
}

rtems_unsigned8
DsReadClockRegister (
	rtems_unsigned8 Register
)

/*++

Routine Description:

    This routine reads the specified realtime clock register.

Arguments:

    Register - Supplies the number of the register whose value is read.

Return Value:

    The value of the register is returned as the function value.

--*/

{
	rtems_unsigned8 BcdValue;

	BcdValue =  DsReadRawClockRegister(Register);
	return BCD2Bin(BcdValue);
}

void
timeDsSet (
    rtems_time_of_day *pTOD
    )

/*++

Routine Description:

    This routine sets the realtime clock.

    N.B. This routine assumes that the caller has provided any required
        synchronization to set the realtime clock information.

Arguments:

    pTOD - Supplies a pointer to a time structure that specifies the
        realtime clock information.

Return Value:

    If the power to the realtime clock has not failed, then the time
    values are written to the realtime clock and a value of TRUE is
    returned. Otherwise, a value of FALSE is returned.

--*/

{
	rtems_unsigned8 ucDataByte;
	PCMOS_MAP pCMOS = (PCMOS_MAP)0;

	/* If the realtime clock battery is still functioning, then write */
	/* the realtime clock values, and return a function value of TRUE. */
	/* Otherwise, return a function value of FALSE. */

	ucDataByte = DsReadRawClockRegister(RTC_CONTROL_REGISTERD);
	if (ucDataByte&DS1385_REGD_VALID)
	{
		/* Set the realtime clock control to set the time. */

		ucDataByte = DS1385_REGB_HOURS_FMT | DS1385_REGB_SET_TIME;
		DsWriteRawClockRegister(RTC_CONTROL_REGISTERB, ucDataByte);

		/* Write the realtime clock values. */

		DsWriteClockRegister(RTC_YEAR,
				     (rtems_unsigned8)(pTOD->year%100));
		if(pTOD->year>=100)
		{
			DsWriteClockRegister((rtems_unsigned8)
					     ((unsigned long)&pCMOS->Century),
					     pTOD->year/100);
		}
		DsWriteClockRegister(RTC_MONTH,
				     (rtems_unsigned8)pTOD->month);
		DsWriteClockRegister(RTC_DAY_OF_MONTH,
				     (rtems_unsigned8)pTOD->day);
		DsWriteClockRegister(RTC_DAY_OF_WEEK,
				     (rtems_unsigned8)
				     (GregorianDay(pTOD) + 1));
		DsWriteClockRegister(RTC_HOUR,
				     (rtems_unsigned8)pTOD->hour);
		DsWriteClockRegister(RTC_MINUTE,
				     (rtems_unsigned8)pTOD->minute);
		DsWriteClockRegister(RTC_SECOND,
				     (rtems_unsigned8)pTOD->second);

		/* Set the realtime clock control to update the time. */

		ucDataByte &= ~DS1385_REGB_SET_TIME;
		DsWriteRawClockRegister(RTC_CONTROL_REGISTERB, ucDataByte);
		return;

	}
	else
	{
		return;
	}
}

boolean
timeDsGet (
    rtems_time_of_day *pTOD
    )

/*++

Routine Description:

    This routine queries the realtime clock.

Arguments:

    pTOD - Supplies a pointer to a time structure that receives
        the realtime clock information.

Return Value:

    If the power to the realtime clock has not failed, then the time
    values are read from the realtime clock and a value of TRUE is
    returned. Otherwise, a value of FALSE is returned.

--*/

{
	rtems_unsigned8 ucDataByte;
	PCMOS_MAP pCMOS = (PCMOS_MAP)0;

	/* If the realtime clock battery is still functioning, then read */
	/* the realtime clock values, and return a function value of TRUE. */
	/* Otherwise, return a function value of FALSE. */

	ucDataByte = DsReadRawClockRegister(RTC_CONTROL_REGISTERD);
	if(ucDataByte&DS1385_REGD_VALID)
	{
		/* Wait until the realtime clock is not being updated. */

		do
		{
			ucDataByte=DsReadRawClockRegister(RTC_CONTROL_REGISTERA);
		} while (ucDataByte&DS1385_REGA_UIP);

		/* Read the realtime clock values. */

		pTOD->year=(rtems_unsigned16)
				  (DsReadClockRegister(
				   (rtems_unsigned8)
				   (unsigned long)&pCMOS->Century)
				  *100 + DsReadClockRegister(RTC_YEAR));
		pTOD->month=DsReadClockRegister(RTC_MONTH);
		pTOD->day=DsReadClockRegister(RTC_DAY_OF_MONTH);
		pTOD->hour=DsReadClockRegister(RTC_HOUR);
		pTOD->minute=DsReadClockRegister(RTC_MINUTE);
		pTOD->second=DsReadClockRegister(RTC_SECOND);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void
timeMkSet (
    rtems_time_of_day *pTOD
    )

/*++

Routine Description:

    This routine sets the realtime clock.

    N.B. This routine assumes that the caller has provided any required
        synchronization to set the realtime clock information.

Arguments:

    pTOD - Supplies a pointer to a time structure that specifies the
        realtime clock information.

Return Value:

    If the power to the realtime clock has not failed, then the time
    values are written to the realtime clock and a value of TRUE is
    returned. Otherwise, a value of FALSE is returned.

--*/

{
	PMK48T18_NVRAM_MAP pNvRAM = MK48T18_BASE;

	/*
	 * Set the RTC into write mode
	 */
	pNvRAM->CMOS.Control|=MK48T18_CTRL_WRITE;
	EIEIO;

	/*
	 * Write the realtime clock values.
	 */

	pNvRAM->CMOS.Year = (rtems_unsigned8)Bin2BCD(pTOD->year%100);
	if(pTOD->year>=100)
	{
	    pNvRAM->CMOS.Century=(rtems_unsigned8)
	    			 Bin2BCD(pTOD->year/100);
	}
	pNvRAM->CMOS.Month  = (rtems_unsigned8)Bin2BCD(pTOD->month);
	pNvRAM->CMOS.Date   = (rtems_unsigned8)Bin2BCD(pTOD->day);
	pNvRAM->CMOS.Day    = (rtems_unsigned8)(GregorianDay(pTOD) + 1);
	pNvRAM->CMOS.Hour   = (rtems_unsigned8)Bin2BCD(pTOD->hour);
	pNvRAM->CMOS.Minute = (rtems_unsigned8)Bin2BCD(pTOD->minute);
	pNvRAM->CMOS.Second = (rtems_unsigned8)Bin2BCD(pTOD->second);

	/*
	 * Set the realtime clock control to update the time.
	 */

	EIEIO;
	pNvRAM->CMOS.Control&=~MK48T18_CTRL_WRITE;
}

boolean
timeMkGet (
    rtems_time_of_day *pTOD
    )

/*++

Routine Description:

    This routine queries the realtime clock.

    N.B. This routine is required to provide any synchronization necessary
         to query the realtime clock information.

Arguments:

    pTOD - Supplies a pointer to a time structure that receives
        the realtime clock information.

Return Value:

    If the power to the realtime clock has not failed, then the time
    values are read from the realtime clock and a value of TRUE is
    returned. Otherwise, a value of FALSE is returned.

--*/

{
	PMK48T18_NVRAM_MAP pNvRAM = MK48T18_BASE;

	/*
	 * Set the RTC into read mode
	 */
	pNvRAM->CMOS.Control|=MK48T18_CTRL_READ;
	EIEIO;

	/*
	 * Read the realtime clock values.
	 */

	pTOD->year = (rtems_unsigned16)(100*BCD2Bin(pNvRAM->CMOS.Century)+
					      BCD2Bin(pNvRAM->CMOS.Year));
	pTOD->month = (rtems_unsigned8)BCD2Bin(pNvRAM->CMOS.Month);
	pTOD->day = (rtems_unsigned8)BCD2Bin(pNvRAM->CMOS.Date);
	pTOD->hour = (rtems_unsigned8)BCD2Bin(pNvRAM->CMOS.Hour);
	pTOD->minute = (rtems_unsigned8)BCD2Bin(pNvRAM->CMOS.Minute);
	pTOD->second = (rtems_unsigned8)BCD2Bin(pNvRAM->CMOS.Second);

	/*
	 * Set the realtime clock control to normal mode.
	 */
	EIEIO;
	pNvRAM->CMOS.Control&=~MK48T18_CTRL_READ;

	return TRUE;
}

/*
 * Set up entry table
 */
void
InitializeRTC(void)
{
	rtems_status_code sc;

	switch(ucSystemType)
	{
		case SYS_TYPE_PPC1:
		{
			if(ucBoardRevMaj<5)
			{
				pTimeFunc=&timeDsTable;
				break;
			}
			/*
			 * For the 005 and later drop through to the PPC1a support
			 */
		}

		case SYS_TYPE_PPC1a:
		{
			pTimeFunc=&timeMkTable;
			break;
		}

		default:
		{
			pTimeFunc=&timeDsTable;
			break;
		}
	}

	/*
	 * Set up mutex semaphore
	 */
	sc = rtems_semaphore_create (
		rtems_build_name ('R', 'T', 'C', 's'),
		1,
		RTEMS_BINARY_SEMAPHORE |
		RTEMS_INHERIT_PRIORITY |
		RTEMS_PRIORITY,
		RTEMS_NO_PRIORITY,
		&semRTC);
	if (sc != RTEMS_SUCCESSFUL)
	{
		rtems_fatal_error_occurred (sc);
	}
}

void setRealTimeToRTEMS()
{
	rtems_time_of_day rtc_tod;

	rtems_semaphore_obtain(semRTC, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	(pTimeFunc->GetTime)(&rtc_tod);
	rtems_semaphore_release(semRTC);

	/*
	 * Millenium fix...
	 *
	 * If year is earlier than MIN_YEAR then assume the clock has wrapped from
	 * 1999 to 1900 so advance by a century
	 */
	if(rtc_tod.year<MIN_YEAR)
	{
		rtc_tod.year+=100;
		rtems_semaphore_obtain(semRTC, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		(pTimeFunc->SetTime)(&rtc_tod);
		rtems_semaphore_release(semRTC);
	}

	rtc_tod.ticks=0;

	rtems_clock_set( &rtc_tod );
}

void setRealTimeFromRTEMS()
{
	rtems_time_of_day rtems_tod;

	rtems_clock_get( RTEMS_CLOCK_GET_TOD, &rtems_tod );
	rtems_semaphore_obtain(semRTC, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	(pTimeFunc->SetTime)(&rtems_tod);
	rtems_semaphore_release(semRTC);
}


int checkRealTime()
{
  return 0;
}
