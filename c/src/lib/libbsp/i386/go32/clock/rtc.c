/* 
 *  $Id$
 */

#define IO_RTC		0x70	/* RTC */

#define RTC_SEC		0x00	/* seconds */
#define RTC_SECALRM	0x01	/* seconds alarm */
#define RTC_MIN		0x02	/* minutes */
#define RTC_MINALRM	0x03	/* minutes alarm */
#define RTC_HRS		0x04	/* hours */
#define RTC_HRSALRM	0x05	/* hours alarm */
#define RTC_WDAY	0x06	/* week day */
#define RTC_DAY		0x07	/* day of month */
#define RTC_MONTH	0x08	/* month of year */
#define RTC_YEAR	0x09	/* month of year */
#define RTC_STATUSA	0x0a	/* status register A */
#define  RTCSA_TUP	 0x80	/* time update, don't look now */

#define RTC_STATUSB	0x0b	/* status register B */

#define RTC_INTR	0x0c	/* status register C (R) interrupt source */
#define  RTCIR_UPDATE	 0x10	/* update intr */
#define  RTCIR_ALARM	 0x20	/* alarm intr */
#define  RTCIR_PERIOD	 0x40	/* periodic intr */
#define  RTCIR_INT	 0x80	/* interrupt output signal */

#define RTC_STATUSD	0x0d	/* status register D (R) Lost Power */
#define  RTCSD_PWR	 0x80	/* clock lost power */

#define RTC_DIAG	0x0e	/* status register E - bios diagnostic */
#define RTCDG_BITS	"\020\010clock_battery\007ROM_cksum\006config_unit\005memory_size\004fixed_disk\003invalid_time"

#define RTC_CENTURY	0x32	/* current century - increment in Dec99 */



#include <rtems.h>
#include <cpu.h>
#include <memory.h>

void init_rtc( void )
{
    int s;

	/* initialize brain-dead battery powered clock */
	outport_byte( IO_RTC, RTC_STATUSA );
	outport_byte( IO_RTC+1, 0x26 );
	outport_byte( IO_RTC, RTC_STATUSB );
	outport_byte( IO_RTC+1, 2 );

	outport_byte( IO_RTC, RTC_DIAG );
	inport_byte( IO_RTC+1, s );
#if 0
	if (s) printf("RTC BIOS diagnostic error %b\n", s, RTCDG_BITS);
#endif
}


/* convert 2 digit BCD number */
static int bcd( unsigned int i )
{
	return ((i/16)*10 + (i%16));
}

/* convert years to seconds (from 1970) */
static unsigned long ytos( int y )
{
	int i;
	unsigned long ret;

	ret = 0;
	for(i = 1970; i < y; i++) {
		if (i % 4) ret += 365*24*60*60;
		else ret += 366*24*60*60;
	}
	return ret;
}

/* convert months to seconds */
static unsigned long mtos( int m, int leap )
{
	int i;
	unsigned long ret;

	ret = 0;
	for(i=1;i<m;i++) {
		switch(i){
		case 1: case 3: case 5: case 7: case 8: case 10: case 12:
			ret += 31*24*60*60;
			break;
		case 4: case 6: case 9: case 11:
			ret += 30*24*60*60;
			break;
		case 2:
			if (leap)
			    ret += 29*24*60*60;
			else
			    ret += 28*24*60*60;
		}
	}
	return ret;
}


static inline unsigned int rtcin( unsigned int what )
{
    unsigned int r;
    outport_byte( IO_RTC, what );
    inport_byte( IO_RTC+1, r );
    return r;
}


/*
 * Initialize the time of day register, based on the time base which is, e.g.
 * from a filesystem.
 */
long rtc_read( rtems_time_of_day * tod )
{
        int sa;
	unsigned long sec = 0;

	memset( tod, 0, sizeof *tod );

	/* do we have a realtime clock present? (otherwise we loop below) */
	sa = rtcin(RTC_STATUSA);
	if (sa == 0xff || sa == 0)
	    return -1;

	/* ready for a read? */
	while ((sa&RTCSA_TUP) == RTCSA_TUP)
	    sa = rtcin(RTC_STATUSA);

	tod->year	= bcd(rtcin(RTC_YEAR)) + 1900;	/* year	  */
	if (tod->year < 1970)	tod->year += 100;	
	tod->month	= bcd(rtcin(RTC_MONTH));	/* month   */
	tod->day	= bcd(rtcin(RTC_DAY));		/* day	   */
	(void)		  bcd(rtcin(RTC_WDAY));		/* weekday */
	tod->hour	= bcd(rtcin(RTC_HRS));		/* hour    */
	tod->minute	= bcd(rtcin(RTC_MIN));		/* minutes */
	tod->second	= bcd(rtcin(RTC_SEC));		/* seconds */
	tod->ticks	= 0;
#if 0
	sec = ytos( tod->year );
	sec += mtos( tod->month, tod->year % 4 == 0 );
	sec += tod->day * 24*60*60;
	sec += tod->hour * 60*60;			/* hour    */
	sec += tod->minute * 60;			/* minutes */
	sec += tod->second;				/* seconds */
#else
	sec = 0;
#endif
	return sec;
}



/* from djgpp: include before rtems.h to avoid conflicts */
#undef delay
#include <dos.h>

void rtc_set_dos_date( void )
{
    int s;
    struct date date;
    struct time time;

    /* initialize brain-dead battery powered clock */
    outport_byte( IO_RTC, RTC_STATUSA );
    outport_byte( IO_RTC+1, 0x26 );
    outport_byte( IO_RTC, RTC_STATUSB );
    outport_byte( IO_RTC+1, 2 );

    outport_byte( IO_RTC, RTC_DIAG );
    inport_byte( IO_RTC+1, s );
    if (s)  {
#if 0
	printf("RTC BIOS diagnostic error %b\n", s, RTCDG_BITS);
#else
	return;
#endif
    }

    /* check for presence of clock */
    s = rtcin(RTC_STATUSA);
    if ( s == 0xff  ||  s == 0 )  {
#if 0
	printf( "Real-time clock not found\n" );
#endif
	return;
    }

    /* ready for a read? */
    while ((s & RTCSA_TUP) == RTCSA_TUP)
	s = rtcin(RTC_STATUSA);

    date.da_year	= bcd(rtcin(RTC_YEAR)) + 1900;	/* year	  */
    if ( date.da_year < 1970)	date.da_year += 100;
    date.da_year	-= 1980;
    date.da_mon		= bcd(rtcin(RTC_MONTH));	/* month   */
    date.da_day		= bcd(rtcin(RTC_DAY));		/* day	   */

    (void)bcd(rtcin(RTC_WDAY));		/* weekday */

    time.ti_hour	= bcd(rtcin(RTC_HRS));		/* hour    */
    time.ti_min		= bcd(rtcin(RTC_MIN));		/* minutes */
    time.ti_sec		= bcd(rtcin(RTC_SEC));		/* seconds */
    time.ti_hund	= 0;

    setdate( & date );
    settime( & time );
}
