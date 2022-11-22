#ifndef __SPI_FLASH_DEVICE_H_

typedef struct
{
    unsigned int address;
    unsigned int device_id;
} spi_flash_control_t;

/* Flash device control command */
#define RTEMS_BSP_FLASH_CONTROL_DISABLE_DEVICE (0x1)
#define RTEMS_BSP_FLASH_CONTROL_ERASE_SECTOR (0x2)
#define RTEMS_BSP_FLASH_CONTROL_ERASE_CHIP (0x3)
#define RTEMS_BSP_FLASH_CONTROL_DEVICE_ID (0x4)

#endif