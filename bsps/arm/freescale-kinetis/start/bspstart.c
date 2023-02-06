/**
 * @file
 *
 * @ingroup RTEMSBSPsKINETIS
 *
 * @brief Startup code.
 */

#include <bsp.h>
#include <bsp/irq-generic.h>
#include <bsp/start.h>
#include <bsp/bootcard.h>
#include <bsp/linker-symbols.h>

#include <rtems/sysinit.h>

extern rtems_status_code kinetis_sd_card_init( bool mount);

void bsp_start( void )
{
    spi_gpio_init();

    bsp_interrupt_initialize();
}

void bsp_register_spi_on_boot_time(void);
RTEMS_SYSINIT_ITEM(
  bsp_register_spi_on_boot_time,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_LAST_BUT_5
);
