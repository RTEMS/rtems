/*===============================================================*\
| Project: SPI driver for W25Q80BV like spi flash device            |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.org/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
\*===============================================================*/

#include <rtems.h>
#include <rtems/libi2c.h>

#include <rtems/libio.h>

#include <libchip/spi-memdrv.h>
#include <bsp/spi-flash-device.h>

static rtems_status_code spi_flash_w25q80_open(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                      *arg);


static rtems_status_code spi_flash_w25q80_read(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                      *arg);


static rtems_status_code spi_flash_w25q80_write(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                      *arg);

static rtems_status_code spi_flash_w25q80_control(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                      *arg);

static unsigned char spi_flash_w25q80_read_state_register(rtems_device_minor_number minor);
static void spi_flash_w25q80_write_state_register(rtems_device_minor_number minor, unsigned char sta);
static void spi_flash_w25q80_write_enable(rtems_device_minor_number minor);
static void spi_flash_w25q80_write_disable(rtems_device_minor_number minor);
static unsigned char spi_flash_w25q80_busy(rtems_device_minor_number minor);
static int spi_flash_w25q80_page_write(rtems_device_minor_number minor, unsigned int addr, unsigned char *dat, unsigned int n);
static void spi_flash_w25q80_sector_erase(rtems_device_minor_number minor, unsigned int addr);
static void spi_flash_w25q80_chip_erase(rtems_device_minor_number minor);
static unsigned char spi_flash_w25q80_get_device_id(rtems_device_minor_number minor);

/*
 * driver operation tables
 */
rtems_driver_address_table spi_flash_w25q80_ops = {
  .open_entry = spi_flash_w25q80_open,
  .read_entry =  spi_flash_w25q80_read,
  .write_entry = spi_flash_w25q80_write,
  .control_entry = spi_flash_w25q80_control
};

static spi_memdrv_t spi_flash_w25q80_drv_config = {
  {/* public fields */
    .ops =         &spi_flash_w25q80_ops, /* operations of general memdrv */
    .size =        sizeof (spi_flash_w25q80_drv_config),
  },
  { /* our private fields */
    .baudrate =             2000000,
    .erase_before_program = true,
    .empty_state =          0xff,
    .page_size =            256,       /* programming page size in bytes */
    .sector_size =          0x1000,    /* 4K - erase sector size in bytes */
    .mem_size =             0x100000,  /* 1MB - total capacity in bytes */
  }
};

rtems_libi2c_drv_t *spi_flash_w25q80_driver_descriptor =
&spi_flash_w25q80_drv_config.libi2c_drv_entry;



unsigned char spi_flash_w25q80_read_state_register(rtems_device_minor_number minor)
{
    unsigned char txdbuff = 0x05;
    rtems_libi2c_send_start(minor);
    rtems_libi2c_send_addr(minor, true);
    rtems_libi2c_write_bytes(minor, &txdbuff, 1);	
    rtems_libi2c_read_bytes(minor, &txdbuff, 1 );
    rtems_libi2c_send_stop(minor);
    return txdbuff;
}

void spi_flash_w25q80_write_state_register(rtems_device_minor_number minor, unsigned char sta)
{
    unsigned char txdbuff = 0x01;

    rtems_libi2c_send_start(minor);
    rtems_libi2c_send_addr(minor, true);
    rtems_libi2c_write_bytes(minor, &txdbuff, 1);	
    rtems_libi2c_write_bytes(minor, &sta, 1 );
    rtems_libi2c_send_stop(minor);
}


void spi_flash_w25q80_write_enable(rtems_device_minor_number minor)
{
    unsigned char txdbuff = 0x06;
    rtems_libi2c_send_start(minor);
    rtems_libi2c_send_addr(minor, true);
    rtems_libi2c_write_bytes(minor, (unsigned char *)&txdbuff, 1 );	
    rtems_libi2c_send_stop(minor);
}

void spi_flash_w25q80_write_disable(rtems_device_minor_number minor)
{
    unsigned char txdbuff = 0x04;
    rtems_libi2c_send_start(minor);
    rtems_libi2c_send_addr(minor, true);
    rtems_libi2c_write_bytes(minor, (unsigned char *)&txdbuff, 1 );	
    rtems_libi2c_send_stop(minor);	
}

unsigned char spi_flash_w25q80_busy(rtems_device_minor_number minor)
{
	return (spi_flash_w25q80_read_state_register(minor) & 0x01);
}

int spi_flash_w25q80_page_write(rtems_device_minor_number minor, unsigned int addr, unsigned char *dat, unsigned int n)
{
    unsigned char txdbuff[6];
    int rc = 0;

    spi_flash_w25q80_write_enable(minor);
	
    txdbuff[0] = 0x02;
    txdbuff[1] = addr >> 16;
    txdbuff[2] = addr >> 8;
    txdbuff[3] = addr;
	
    rtems_libi2c_send_start(minor);
    rtems_libi2c_send_addr(minor, true);

    rc = rtems_libi2c_write_bytes(minor, txdbuff, 4 );
    if (rc < 0)
    {
        return rc;
    }

    rc = rtems_libi2c_write_bytes(minor, dat, n );
    if (rc < 0)
    {
        return rc;
    }

    rtems_libi2c_send_stop(minor);
    while (spi_flash_w25q80_busy(minor));

    return rc;
}

void spi_flash_w25q80_sector_erase(rtems_device_minor_number minor, unsigned int addr)
{
    unsigned char txdbuff[4];
    unsigned int aligned_addr = addr & 0xFFFFF000;
    
    txdbuff[0] = 0x20;
    txdbuff[1] = aligned_addr >> 16;
    txdbuff[2] = aligned_addr >> 8;
    txdbuff[3] = aligned_addr;

    spi_flash_w25q80_write_enable(minor);	
    rtems_libi2c_send_start(minor);
    rtems_libi2c_send_addr(minor, true);
    rtems_libi2c_write_bytes(minor, txdbuff, 4);	
    rtems_libi2c_send_stop(minor);
    while (spi_flash_w25q80_busy(minor));
}

void spi_flash_w25q80_chip_erase(rtems_device_minor_number minor)
{
    unsigned char txdbuff = 0x60;
    spi_flash_w25q80_write_enable(minor);	
    rtems_libi2c_send_start(minor);
    rtems_libi2c_send_addr(minor, true);
    rtems_libi2c_write_bytes(minor, &txdbuff, 1);
    rtems_libi2c_send_stop(minor);
    while (spi_flash_w25q80_busy(minor));
}

unsigned char spi_flash_w25q80_get_device_id(rtems_device_minor_number minor)
{
    unsigned char txdbuff[6];
	txdbuff[0] = 0x90;	//fast read
	txdbuff[3] = 0x00;
	
    rtems_libi2c_send_start(minor);
	rtems_libi2c_send_addr(minor, true);
	rtems_libi2c_write_bytes(minor, txdbuff, 4 );
	rtems_libi2c_read_bytes(minor, txdbuff, 2 );
	rtems_libi2c_send_stop(minor);
	return txdbuff[1];
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code spi_flash_w25q80_open
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   open and initialize a flash device                                      |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_device_major_number major,        /* major device number            */
 rtems_device_minor_number minor,        /* minor device number            */
 void                      *arg          /* ptr to read argument struct    */
)
{
    spi_flash_w25q80_write_state_register(minor, 0x00);
    return RTEMS_SUCCESSFUL;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code spi_flash_w25q80_read
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   read a block of data from flash                                         |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_device_major_number major,        /* major device number            */
 rtems_device_minor_number minor,        /* minor device number            */
 void                      *arg          /* ptr to read argument struct    */
)
{
    rtems_libio_rw_args_t *rwargs = arg;
    unsigned char            *dat = (unsigned char *)rwargs->buffer;
    off_t                    addr = rwargs->offset;
    int                       n   = rwargs->count;
    unsigned char *p;
    unsigned char txdbuff[6];
    int rc = 0;
    txdbuff[0] = 0x0B;
    p = (unsigned char *)&addr;
    txdbuff[3] = *p++;
    txdbuff[2] = *p++;
    txdbuff[1] = *p;    	
    rtems_libi2c_send_start(minor);
    rtems_libi2c_send_addr(minor, true);
    
    rc = rtems_libi2c_write_bytes(minor, txdbuff, 5 );
    if (rc < 0)
    {
        return rc;
    }

    rc = rtems_libi2c_read_bytes(minor, dat, n );
    if (rc > 0)
    {
        rwargs->bytes_moved = rc;
    }

    rtems_libi2c_send_stop(minor);

    return RTEMS_SUCCESSFUL;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code spi_flash_w25q80_write
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   write a block of data to flash                                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_device_major_number major,        /* major device number            */
 rtems_device_minor_number minor,        /* minor device number            */
 void                      *arg          /* ptr to write argument struct   */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/
{
    rtems_libio_rw_args_t *rwargs = arg;
    unsigned char            *dat = (unsigned char *)rwargs->buffer;
    off_t                    addr = rwargs->offset;
    int                       n   = rwargs->count;
    unsigned int m;
    int rc = 0;

    if (((addr & 0xff) + n) > 0x100) {
    	m = 0x100 - (addr & 0xff);
    	rc += spi_flash_w25q80_page_write(minor, addr, dat, m);
        if (rc < 0)
        {
            return rc;
        }

    	rc += spi_flash_w25q80_page_write(minor, (addr + m), dat + m, n - m);
        if (rc < 0)
        {
            return rc;
        }
    }
    else {
    	rc = spi_flash_w25q80_page_write(minor, addr, dat, n);
        if (rc < 0)
        {
            return rc;
        }
    }
    
    rwargs->bytes_moved = rc;
    return RTEMS_SUCCESSFUL;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_status_code spi_flash_w25q80_control
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   Send control command to flash                                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_device_major_number major,        /* major device number            */
 rtems_device_minor_number minor,        /* minor device number            */
 void                      *arg          /* ptr to write argument struct   */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    o = ok or error code                                                   |
\*=========================================================================*/
{
    rtems_libio_ioctl_args_t *rwargs = arg;
    spi_flash_control_t *control = (spi_flash_control_t *)rwargs->buffer;
    unsigned int cmd = rwargs->command;
    unsigned int addr = control->address;
    int rc = RTEMS_SUCCESSFUL;

    switch (cmd)
    {
        case RTEMS_BSP_FLASH_CONTROL_DISABLE_DEVICE:
            spi_flash_w25q80_write_disable(minor);
            break;
        case RTEMS_BSP_FLASH_CONTROL_ERASE_SECTOR:
            spi_flash_w25q80_sector_erase(minor, addr);
            break;
        case RTEMS_BSP_FLASH_CONTROL_ERASE_CHIP:
            spi_flash_w25q80_chip_erase(minor);
            break;
        case RTEMS_BSP_FLASH_CONTROL_DEVICE_ID:
            control->device_id = spi_flash_w25q80_get_device_id(minor);
            break;
        default:
            rc = RTEMS_NOT_DEFINED;
            break;
    }

    return rc;
}

// #include <rtems/libi2c.h>
// #include <rtems/blkdev.h>


// static int spi_flash_chip_ioctl( rtems_disk_device *dd, uint32_t req, void *arg)
// {
//     return RTEMS_SUCCESSFUL;
// }

// int spi_flash_device_register(void)
// {
//     rtems_status_code sc;
//     sc = rtems_blkdev_create("/dev/mtd0", 0, 0, spi_flash_chip_ioctl, NULL);
// 	RTEMS_CHECK_SC( sc, "Create flash device");
// }