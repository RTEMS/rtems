/*
 *  $Id$
 */

#include <stdio.h>
#include <dos.h>

#define IO_RTC		0x70	/* RTC */

#define RTC_SEC		0x00	/* seconds */
#define RTC_MIN		0x02	/* minutes */
#define RTC_HRS		0x04	/* hours */
#define RTC_WDAY	0x06	/* week day */
#define RTC_DAY		0x07	/* day of month */
#define RTC_MONTH	0x08	/* month of year */
#define RTC_YEAR	0x09	/* month of year */
#define RTC_STATUSA	0x0a	/* status register A */
#define  RTCSA_TUP	 0x80	/* time update, don't look now */

#define RTC_STATUSB	0x0b	/* status register B */

#define RTC_DIAG	0x0e	/* status register E - bios diagnostic */
#define RTCDG_BITS	"\020\010clock_battery\007ROM_cksum\006config_unit\005memory_size\004fixed_disk\003invalid_time"



/* convert 2 digit BCD number */
static int bcd( unsigned int i )
{
	return ((i/16)*10 + (i%16));
}

static unsigned int rtcin( unsigned int what )
{
    outportb( IO_RTC, what );
    return inportb( IO_RTC+1 );
}


void fix_date( void )
{
    int s;
    struct date date;
    struct time time;

    /* initialize brain-dead battery powered clock */
    outportb( IO_RTC, RTC_STATUSA );
    outportb( IO_RTC+1, 0x26 );
    outportb( IO_RTC, RTC_STATUSB );
    outportb( IO_RTC+1, 2 );

    outportb( IO_RTC, RTC_DIAG );
    s = inportb( IO_RTC+1 );
    if (s) printf("RTC BIOS diagnostic error %b\n", s, RTCDG_BITS);

    /* check for presence of clock */
    s = rtcin(RTC_STATUSA);
    if ( s == 0xff  ||  s == 0 )  {
	printf( "Real-time clock not found\n" );
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




void fix_timer( void )
{
#define PIT_PORT	0x40
#define TIMER_CNTR0	(PIT_PORT + 0)	/* timer 0 counter port */
#define TIMER_MODE	(PIT_PORT + 3)	/* timer mode port */
#define   TIMER_SEL0	0x00		/* select counter 0 */
#define   TIMER_RATEGEN	0x04		/* mode 2, rate generator */
#define   TIMER_16BIT	0x30		/* r/w counter 16 bits, LSB first */

    /* set timer 0 to generate interrupts every period */
    outportb( TIMER_MODE, TIMER_SEL0|TIMER_RATEGEN|TIMER_16BIT );
    outportb( TIMER_CNTR0, 0 );  /* counter low */
    outportb( TIMER_CNTR0, 0 );  /* counter high */
}

int main(void)
{
    /* Make timer count at correct pace */
    fix_timer();

    /* Fix the date */
    fix_date();

    return 0;
}
