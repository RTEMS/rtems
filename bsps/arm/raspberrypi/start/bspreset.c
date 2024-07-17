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

void bsp_reset( rtems_fatal_source source, rtems_fatal_code code )
{
  uint32_t rstc;

  (void) source;
  (void) code;

  BCM2835_REG(BCM2835_PM_WDOG) = BCM2835_PM_PASSWD_MAGIC | 20;
  rstc = BCM2835_REG(BCM2835_PM_RSTC);
  rstc &= ~BCM2835_PM_RSTC_WRCFG;
  rstc |= BCM2835_PM_PASSWD_MAGIC | BCM2835_PM_RSTC_WRCFG_FULL;
  BCM2835_REG(BCM2835_PM_RSTC) = rstc;
  BCM2835_REG(BCM2835_PM_WDOG) = BCM2835_PM_PASSWD_MAGIC | 1;

  while (1) ;
}
