/*
 *  Timer Init
 *
 *  Use the last DMA timer (DTIM3) as the diagnostic timer.
 */

#include <rtems.h>
#include <bsp.h>

void
Timer_initialize(void)
{
    int preScaleDivisor = 0x4A;
    int div = MCF5235_TIMER_DTMR_CLK_DIV1;
    MCF5235_TIMER_DTRR3 = 0x2710;
    MCF5235_TIMER3_DTMR = 0;
    MCF5235_TIMER3_DTMR = MCF5235_TIMER_DTMR_PS(preScaleDivisor) | div |
                          MCF5235_TIMER_DTMR_RST;
}

/*
 * Return timer value in microsecond units
 */
int
Read_timer(void)
{
    return MCF5235_TIMER3_DTCN;
}

/*
 *  Empty function call used in loops to measure basic cost of looping
 *  in Timing Test Suite.
 */
rtems_status_code
Empty_function(void)
{
    return RTEMS_SUCCESSFUL;
}

void
Set_find_average_overhead(rtems_boolean find_flag)
{
}
