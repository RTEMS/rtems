/*
 *  This file contains the RTC driver table for the DY-4 DMV177.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994, 1997.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  $Id$
 */

#include <bsp.h>

#include <libchip/rtc.h>
#include <libchip/icm7170.h>

/*
 * Configuration specific probe routines
 *
 * NOTE:  There are no DMV177 specific configuration routines.  These
 *        routines could be written to dynamically determine what type
 *        of real-time clock is on this board.  This would be useful for
 *        a BSP supporting multiple board models.
 */

/* NONE CURRENTLY PROVIDED */

/*
 * The following table configures the RTC drivers used in this BSP.
 *
 * The first entry which, when probed, is available, will be named /dev/rtc,
 * all others being given the name indicated.
 */

boolean dmv177_icm7170_probe(int minor);

rtc_tbl	RTC_Table[] = {
	{
		"/dev/rtc0",			/* sDeviceName */
                RTC_ICM7170,                    /* deviceType */
		&icm7170_fns,			/* pDeviceFns */
		dmv177_icm7170_probe,		/* deviceProbe */
		(void *) ICM7170_AT_1_MHZ,	/* pDeviceParams */
		DMV170_RTC_ADDRESS,		/* ulCtrlPort1 */
		0,				/* ulDataPort */
		icm7170_get_register_8,        	/* getRegister */
		icm7170_set_register_8,		/* setRegister */
	}
};

/*
 *  Declare some information used by the RTC driver
 */

#define NUM_RTCS (sizeof(RTC_Table)/sizeof(rtc_tbl))

unsigned long  RTC_Count = NUM_RTCS;

rtems_device_minor_number  RTC_Minor;

/*
 *  Hopefully, by checking the card resource register, this BSP
 *  will be able to operate on the DMV171, DMV176, or DMV177.
 */

boolean dmv177_icm7170_probe(int minor)
{
  volatile unsigned16 *card_resource_reg;
  unsigned16 v;

  card_resource_reg = (volatile unsigned16 *) DMV170_CARD_RESORCE_REG;

  v = *card_resource_reg & DMV170_RTC_INST_MASK;

  if ( v == DMV170_RTC_INSTALLED )
    return TRUE;

  return FALSE;
}

