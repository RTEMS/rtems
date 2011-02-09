/*
 *  Timer Init
 *
 *  Use the last DMA timer (DTIM3) as the diagnostic timer.
 */

#include <rtems.h>
#include <rtems/btimer.h>
#include <bsp.h>

void
benchmark_timer_initialize(void)
{
    int preScaleDivisor = 58;
    int div = MCF5282_TIMER_DTMR_CLK_DIV1;
    MCF5282_TIMER3_DTRR = 0x2710;
    MCF5282_TIMER3_DTMR = 0;
    MCF5282_TIMER3_DTMR = MCF5282_TIMER_DTMR_PS(preScaleDivisor - 1) | div |
                          MCF5282_TIMER_DTMR_RST;
}

/*
 * Return timer value in microsecond units
 */
uint32_t
benchmark_timer_read(void)
{
    return MCF5282_TIMER3_DTCN;
}

void
benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
}
