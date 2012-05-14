/*
 * Real Time Clock (MK48T08) for RTEMS on MVME162
 *
 *  Author:
 *    COPYRIGHT (C) 1997
 *    by Katsutoshi Shibuya - BU Denken Co.,Ltd. - Sapporo - JAPAN
 *    ALL RIGHTS RESERVED
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  This material is a part of the MVME162 Board Support Package
 *  for the RTEMS executive. Its licensing policies are those of the
 *  RTEMS above.
 */

#include <rtems.h>
#include <tod.h>

#define	tod	((volatile unsigned char *)0xfffc1ff8)

static int	getTod(int n, unsigned char mask)
{
    unsigned char	x;

    x = tod[n]&mask;
    return (x>>4)*10+(x&0x0f);
}

static void	setTod(int n, unsigned char d)
{
    tod[n] = ((d/10)<<4)+(d%10);
}

void	setRealTimeToRTEMS(void)
{
    rtems_time_of_day	t;

    tod[0] |= 0x40;	/* Stop read register */
    t.year = 1900+getTod(7,0xff);
    t.month = getTod(6,0x1f);
    t.day = getTod(5,0x3f);
    t.hour = getTod(3,0x3f);
    t.minute = getTod(2,0x7f);
    t.second = getTod(1,0x7f);
    t.ticks = 0;
    tod[0] &= 0x3f;	/* Release read register */

    rtems_clock_set(&t);
}

void	setRealTimeFromRTEMS()
{
    rtems_time_of_day	t;

    rtems_clock_get(RTEMS_CLOCK_GET_TOD,&t);
    t.year -= 1900;

    tod[0] |= 0x80;	/* Stop write register */
    setTod(7,t.year);
    setTod(6,t.month);
    setTod(5,t.day);
    setTod(4,1);	/* I don't know which day of week is */
    setTod(3,t.hour);
    setTod(2,t.minute);
    setTod(1,t.second);
    tod[0] &= 0x3f;	/* Write these parameters */
}

int	checkRealTime()
{
    rtems_time_of_day	t;
    int		d;

    tod[0] |= 0x40;	/* Stop read register */
    rtems_clock_get(RTEMS_CLOCK_GET_TOD,&t);
    if((t.year != 1900+getTod(7,0xff))
       || (t.month != getTod(6,0x1f))
       || (t.day != getTod(5,0x3f)))
	d = 9999;
    else
	d = (t.hour-getTod(3,0x3f))*3600
	    + (t.minute-getTod(3,0x7f))*60
		+ (t.second - getTod(1,0x7f));
    tod[1] &= 0x3f;
    return d;
}
