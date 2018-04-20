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
uint32_t
benchmark_timer_read(void)
{
    return MCF5235_TIMER3_DTCN;
}

void
benchmark_timer_disable_subtracting_average_overhead(bool find_flag)
{
}
