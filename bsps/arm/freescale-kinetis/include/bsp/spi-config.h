#ifndef __SPI_CONFIG_H_
#define __SPI_CONFIG_H_

#include <rtems/libi2c.h>

/* spi device select */
#define RTEMS_BSP_KINETIS_FLASH_ADDR (1)
#define RTEMS_BSP_KINETIS_LCD_ADDR (2)
#define RTEMS_BSP_KINETIS_SDCARD_ADDR (3)


typedef struct {
	/**
	 * @brief Standard bus driver fields.
	 */
	rtems_libi2c_bus_t bus;

	/**
	 * @brief Index in the bus table: @ref kinetis_spi_bus_table.
	 */
	unsigned int idle_char;

	/**
	 * @brief Hardware registers.
	 */
	kinetis_spi_t *regs;

	unsigned int device_number;

	uint32_t baud;

	uint32_t sck;
} kinetis_spi_bus_entry;

rtems_status_code bsp_register_spi(void);
void bsp_register_spi_on_boot_time(void);


#endif

