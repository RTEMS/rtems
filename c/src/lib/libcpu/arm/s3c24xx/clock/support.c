#include <rtems.h>
#include <bsp.h>
#include <s3c24xx.h>

/* ------------------------------------------------------------------------- */
/* NOTE: This describes the proper use of this file.
 *
 * BSP_OSC_FREQ should be defined as the input frequency of the PLL.
 *
 * get_FCLK(), get_HCLK(), get_PCLK() and get_UCLK() return the clock of
 * the specified bus in HZ.
 */
/* ------------------------------------------------------------------------- */

/* return FCLK frequency */
uint32_t get_FCLK(void)
{
    uint32_t r, m, p, s;

    r = rMPLLCON;
    m = ((r & 0xFF000) >> 12) + 8;
    p = ((r & 0x003F0) >> 4) + 2;
    s = r & 0x3;

    return((BSP_OSC_FREQ * m) / (p << s));
}

/* return UCLK frequency */
uint32_t get_UCLK(void)
{
    uint32_t r, m, p, s;

    r = rUPLLCON;
    m = ((r & 0xFF000) >> 12) + 8;
    p = ((r & 0x003F0) >> 4) + 2;
    s = r & 0x3;

    return((BSP_OSC_FREQ * m) / (p << s));
}

/* return HCLK frequency */
uint32_t get_HCLK(void)
{
    if (rCLKDIVN & 0x2)
	return get_FCLK()/2;
    else
	return get_FCLK();
}

/* return PCLK frequency */
uint32_t get_PCLK(void)
{
    if (rCLKDIVN & 0x1)
	return get_HCLK()/2;
    else
	return get_HCLK();
}
