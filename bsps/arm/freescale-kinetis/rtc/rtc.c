#include <bsp/rtc.h>
#include <bsp/sim.h>


int kinetis_rtc_clock_enable (void)
{
	KINETIS_SIM->scgc6 |= SIM_SCGC6_RTC_MASK;;
	return 0;
}

int kinetis_rtc_clock_disable (void)
{
	KINETIS_SIM->scgc6 &= ~SIM_SCGC6_RTC_MASK;;
	return 0;
}
int kinetis_rtc_start ( void )
{
    KINETIS_RTC->sr |= RTC_SR_TCE_MASK;
	return 0;
}

int kinetis_rtc_stop ( void )
{
    KINETIS_RTC->sr &= ~RTC_SR_TCE_MASK;
	return 0;
}

int kinetis_rtc_init (void)
{
    kinetis_rtc_clock_enable();
    if (KINETIS_RTC->tsr == 0) {
		KINETIS_RTC->cr  = RTC_CR_SWR_MASK;
		KINETIS_RTC->cr  &= ~RTC_CR_SWR_MASK;
		KINETIS_RTC->tsr = 0;
		KINETIS_RTC->tar = 0;
	}
    KINETIS_RTC->cr |= RTC_CR_OSCE_MASK;
    
    KINETIS_RTC->tcr = RTC_TCR_CIR(0) | RTC_TCR_TCR(0);  
    KINETIS_RTC->tpr = 0;
	kinetis_rtc_start();
	
	return 0;
}

int kinetis_rtc_set_second ( unsigned int sec )
{
	kinetis_rtc_stop();
	KINETIS_RTC->tsr = sec;
	kinetis_rtc_start();
	return 0;
}
unsigned int kinetis_rtc_get_second ( void )
{
	return (KINETIS_RTC->tsr);
}
int kinetis_rtc_set_alarm ( unsigned int alarm )
{
    KINETIS_RTC->tar = alarm;
	return 0;
}

