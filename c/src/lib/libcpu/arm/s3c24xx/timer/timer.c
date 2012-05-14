/*
 * S3C2400 Timer driver
 *
 * This uses timer 1 for timing measurments.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 * Notes:
 *  This file manages the benchmark timer used by the RTEMS Timing Test
 *  Suite.  Each measured time period is demarcated by calls to
 *  benchmark_timer_initialize() and benchmark_timer_read().  benchmark_timer_read() usually returns
 *  the number of microseconds since benchmark_timer_initialize() exitted.
 *
 *  It is important that the timer start/stop overhead be determined
 *  when porting or modifying this code.
*/

#include <rtems.h>
#include <bsp.h>
#include <s3c24xx.h>

uint32_t g_start;
uint32_t g_freq;

bool benchmark_timer_find_average_overhead;


/*
 * Set up Timer 1
 */
void benchmark_timer_initialize( void )
{
    uint32_t cr;

    /* stop TIMER1*/
    cr=rTCON & 0xFFFFF0FF;
    rTCON=(cr | (0x0 << 8));

    /* set MUX for Timer1 to 1/2 */
    cr=rTCFG1 & 0xFFFFFF0F;
    rTCFG1=(cr | (0<<4));

    /* input freq=PLCK/2 Mhz*/
    g_freq = get_PCLK() / 2000;
    rTCNTB1 = 0xFFFF;

    /* start TIMER1 with manual reload */
    cr=rTCON & 0xFFFFF0FF;
    rTCON=(cr | (0x1 << 9));
    rTCON=(cr | (0x1 << 8));

    g_start =  rTCNTO1;
}

/*
 *  The following controls the behavior of benchmark_timer_read().
 *
 *  AVG_OVEREHAD is the overhead for starting and stopping the timer.  It
 *  is usually deducted from the number returned.
 *
 *  LEAST_VALID is the lowest number this routine should trust.  Numbers
 *  below this are "noise" and zero is returned.
 */

#define AVG_OVERHEAD      0  /* It typically takes X.X microseconds */
                             /* (Y countdowns) to start/stop the timer. */
                             /* This value is in microseconds. */
#define LEAST_VALID       1  /* Don't trust a clicks value lower than this */

int benchmark_timer_read( void )
{
    uint32_t t;
    unsigned long long total;

    t =  rTCNTO1;
    /*
     *  Total is calculated by taking into account the number of timer overflow
     *  interrupts since the timer was initialized and clicks since the last
     *  interrupts.
     */

    total = (g_start - t);

    /* convert to microseconds */
    total = (total*1000) / g_freq;

    if ( benchmark_timer_find_average_overhead == 1 ) {
        return (int) total;
    } else if ( total < LEAST_VALID ) {
        return 0;
    }

    /*
     *  Somehow convert total into microseconds
     */
    return (total - AVG_OVERHEAD);
}

void benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
    benchmark_timer_find_average_overhead = find_flag;
}

