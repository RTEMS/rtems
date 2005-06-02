#include <rtems.h>
#include <bsp.h>
#include <s3c2400.h>

#define MPLL 0
#define UPLL 1

/* ------------------------------------------------------------------------- */
/* NOTE: This describes the proper use of this file.
 *
 * BSP_OSC_FREQ should be defined as the input frequency of the PLL.
 *
 * get_FCLK(), get_HCLK(), get_PCLK() and get_UCLK() return the clock of
 * the specified bus in HZ.
 */
/* ------------------------------------------------------------------------- */

static uint32_t get_PLLCLK(int pllreg)
{
    uint32_t r, m, p, s;

    if (pllreg == MPLL)
	r = rMPLLCON;
    else if (pllreg == UPLL)
	r = rUPLLCON;
    else
	return 0;

    m = ((r & 0xFF000) >> 12) + 8;
    p = ((r & 0x003F0) >> 4) + 2;
    s = r & 0x3;

    return((BSP_OSC_FREQ * m) / (p << s));
}

/* return FCLK frequency */
uint32_t get_FCLK(void)
{
    return(get_PLLCLK(MPLL));
}

/* return HCLK frequency */
uint32_t get_HCLK(void)
{
    return((rCLKDIVN & 0x2) ? get_FCLK()/2 : get_FCLK());
}

/* return PCLK frequency */
uint32_t get_PCLK(void)
{
    return((rCLKDIVN & 0x1) ? get_HCLK()/2 : get_HCLK());
}

/* return UCLK frequency */
uint32_t get_UCLK(void)
{
    return(get_PLLCLK(UPLL));
}

