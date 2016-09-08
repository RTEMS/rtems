/**
 * @file
 *
 * @ingroup raspberrypi
 *
 * @brief Raspberry pi restart chip function
 */

#include <bsp/bootcard.h>
#include <bsp/raspberrypi.h>
#include <bsp.h>
#include <rtems.h>

void bsp_reset(void)
{
  uint32_t rstc;

  BCM2835_REG(BCM2835_PM_WDOG) = BCM2835_PM_PASSWD_MAGIC | 20;
  rstc = BCM2835_REG(BCM2835_PM_RSTC);
  rstc &= ~BCM2835_PM_RSTC_WRCFG;
  rstc |= BCM2835_PM_PASSWD_MAGIC | BCM2835_PM_RSTC_WRCFG_FULL;
  BCM2835_REG(BCM2835_PM_RSTC) = rstc;
  BCM2835_REG(BCM2835_PM_WDOG) = BCM2835_PM_PASSWD_MAGIC | 1;

  while (1) ;
}
