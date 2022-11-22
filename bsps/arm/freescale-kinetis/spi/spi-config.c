#include <rtems/libi2c.h>
#include <rtems/status-checks.h>

#include <bsp.h>

/* spi bus ioctl command after RTEMS_LIBI2C_IOCTL_READ_WRITE_ASYNC */
#define RTEMS_BSP_SPI_CLOCK_ENABLE (0x6)
#define RTEMS_BSP_SPI_CLOCK_DISABLE (0x7)




static rtems_status_code kinetis_spi_init(rtems_libi2c_bus_t * bushdl);
static rtems_status_code kinetis_spi_send_start(rtems_libi2c_bus_t * bushdl);
static rtems_status_code kinetis_spi_send_stop(rtems_libi2c_bus_t * bushdl);
static rtems_status_code kinetis_spi_send_addr(rtems_libi2c_bus_t * bushdl, uint32_t addr, int rw);
static int kinetis_spi_read(rtems_libi2c_bus_t * bushdl, unsigned char *bytes, int nbytes);
static int kinetis_spi_write(rtems_libi2c_bus_t * bushdl, unsigned char *bytes, int nbytes);
static int kinetis_spi_ioctl(rtems_libi2c_bus_t * bushdl, int cmd, void *args);
static int kinetis_spi_read_write_bytes(rtems_libi2c_bus_t *bh, unsigned char *rbuf, const unsigned char *tbuf, int len);

extern rtems_libi2c_drv_t *spi_flash_w25q80_driver_descriptor;

static const rtems_libi2c_bus_ops_t kinetis_spi_ops = {
	.init        = kinetis_spi_init,
	.send_start  = kinetis_spi_send_start,
	.send_stop   = kinetis_spi_send_stop,
	.send_addr   = kinetis_spi_send_addr,
	.read_bytes  = kinetis_spi_read,
	.write_bytes = kinetis_spi_write,
	.ioctl       = kinetis_spi_ioctl
};

int spi0_busno;
int spi1_busno;

kinetis_spi_bus_entry kinetis_spi_bus_table[2] = {
    /* SPI0 */
    {
        .bus = {
            .ops = &kinetis_spi_ops,
            .size = sizeof(kinetis_spi_bus_entry)

        },
        .regs = SPI0,
		.baud = KINTSI_SPI_BUS_SPEED_HZ
    },

    /* SPI1 */
    {
        .bus = {
            .ops = &kinetis_spi_ops,
            .size = sizeof(kinetis_spi_bus_entry)

        },
        .regs = SPI1,
		.baud = KINTSI_SPI_BUS_SPEED_HZ
    }
};

static rtems_status_code kinetis_spi_init(rtems_libi2c_bus_t * bushdl)
{
	kinetis_spi_bus_entry *spi = (kinetis_spi_bus_entry *)bushdl;
	
    spi_clock_enable(spi->regs);
    spi_init(spi->regs, spi->baud);
    return RTEMS_SUCCESSFUL;
}

static rtems_status_code kinetis_spi_send_start(rtems_libi2c_bus_t * bushdl)
{
	kinetis_spi_bus_entry *spi = (kinetis_spi_bus_entry *)bushdl;
    spi->device_number = 0;

	if (spi->regs == SPI1)
	{
		gpio_set_bit(PTE, IO_4);
	    spi->device_number = RTEMS_BSP_KINETIS_SDCARD_ADDR;
	}
	
    return RTEMS_SUCCESSFUL;
}

static rtems_status_code kinetis_spi_send_stop(rtems_libi2c_bus_t * bushdl)
{
	kinetis_spi_bus_entry *spi = (kinetis_spi_bus_entry *)bushdl;

	switch (spi->device_number)
	{
		case RTEMS_BSP_KINETIS_FLASH_ADDR:
			gpio_set_bit(PTB, IO_23);
			spi->device_number = 0xdeadbeef;
			break;
		case RTEMS_BSP_KINETIS_LCD_ADDR:
			gpio_set_bit(PTA, IO_14);
			spi->device_number = 0xdeadbeef;
			break;
		case RTEMS_BSP_KINETIS_SDCARD_ADDR:
			gpio_set_bit(PTE, IO_4);
	        spi->device_number = 0xdeadbeef;
			break;
		default:
		    break;
	}
    return RTEMS_SUCCESSFUL;
}

static rtems_status_code kinetis_spi_send_addr(rtems_libi2c_bus_t * bushdl, uint32_t addr, int rw)
{
	kinetis_spi_bus_entry *spi = (kinetis_spi_bus_entry *)bushdl;

    switch (addr)
	{
		case RTEMS_BSP_KINETIS_FLASH_ADDR:
			gpio_clear_bit(PTB, IO_23);
	        spi->device_number = addr;
			break;
	    case RTEMS_BSP_KINETIS_LCD_ADDR:
			gpio_clear_bit(PTA, IO_14);
	        spi->device_number = addr;
			break;
		case RTEMS_BSP_KINETIS_SDCARD_ADDR:
			gpio_clear_bit(PTE, IO_4);
	        spi->device_number = addr;
			break;
		default:
		    break;
	}

    return RTEMS_SUCCESSFUL;
}

static int kinetis_spi_read(rtems_libi2c_bus_t * bushdl, unsigned char *bytes, int nbytes)
{
	kinetis_spi_bus_entry *spi = (kinetis_spi_bus_entry *)bushdl;
    
    return spi_receive(spi->regs, bytes, nbytes);
}

static int kinetis_spi_write(rtems_libi2c_bus_t * bushdl, unsigned char *bytes, int nbytes)
{
	kinetis_spi_bus_entry *spi = (kinetis_spi_bus_entry *)bushdl;

    return spi_send(spi->regs, bytes, nbytes);
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int kinetis_spi_read_write_bytes
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   transmit/receive some bytes from SPI device                             |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 unsigned char *rbuf,                    /* buffer to store bytes          */
 const unsigned char *tbuf,              /* buffer to send  bytes          */
 int len                                 /* number of bytes to transceive  */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    number of bytes received or (negative) error code                      |
\*=========================================================================*/
{
    kinetis_spi_bus_entry *spi = (kinetis_spi_bus_entry *)bh;
    rtems_status_code rc = RTEMS_SUCCESSFUL;

    if (tbuf == NULL)
	{
        /* perform idle char write to read byte */
        spi_send_receive(spi->regs, spi->idle_char);
    }
    else
	{
		while(len-- != 0)
		{
            *rbuf++ = spi_send_receive(spi->regs, *tbuf++);
		}
    }

  return rc;
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
static rtems_status_code kinetis_spi_set_tfr_mode
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   set SPI to desired baudrate/clock mode/character mode                   |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_libi2c_bus_t *bh,                 /* bus specifier structure        */
 const rtems_libi2c_tfr_mode_t *tfr_mode /* transfer mode info             */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{
    kinetis_spi_bus_entry *spi = (kinetis_spi_bus_entry *)bh;
	uint8_t ctar_br;
    rtems_status_code rc = RTEMS_SUCCESSFUL;

    /* Set idle character */
    spi->idle_char = tfr_mode->idle_char;

    /* Set baudrate */
    rc = spi_baudrate_evaluation(tfr_mode->baudrate, &spi->sck, &ctar_br);
    if (rc == RTEMS_SUCCESSFUL)
    {
        spi->regs->ctar[0] &= ~(SPI_CTAR_BR_MASK);
        spi->regs->ctar[0] |= SPI_CTAR_BR(ctar_br);
    }
    else
    {
    	return RTEMS_NOT_CONFIGURED;
    }

	/* Set char mode */
	if (tfr_mode->lsb_first)
	{
		spi->regs->ctar[0] |= SPI_CTAR_LSBFE_MASK;
	}

	/* Set clock phase */
	if (tfr_mode->clock_phs)
	{
		spi->regs->ctar[0] |= SPI_CTAR_CPHA_MASK;
	}
	
	return rc;
}

static int kinetis_spi_ioctl(rtems_libi2c_bus_t * bushdl, int cmd, void *args)
{
	kinetis_spi_bus_entry *spi = (kinetis_spi_bus_entry *)bushdl;
    rtems_libi2c_read_write_t *rw = (rtems_libi2c_read_write_t *)args;

	switch (cmd)
	{
		case RTEMS_LIBI2C_IOCTL_READ_WRITE:
            kinetis_spi_read_write_bytes(bushdl, rw->rd_buf, rw->wr_buf, rw->byte_cnt);
		    break;
        case RTEMS_LIBI2C_IOCTL_START_TFM_READ_WRITE:
		    break;
        case RTEMS_LIBI2C_IOCTL_SET_TFRMODE:
		    kinetis_spi_set_tfr_mode(bushdl, (rtems_libi2c_tfr_mode_t *)args);
		    break;
        case RTEMS_LIBI2C_IOCTL_GET_DRV_T:
        case RTEMS_LIBI2C_IOCTL_READ_WRITE_ASYNC:
		    break;
		case RTEMS_BSP_SPI_CLOCK_DISABLE:
		    spi_clock_disable(spi->regs);
			break;
		case RTEMS_BSP_SPI_CLOCK_ENABLE:
		    spi_clock_enable(spi->regs);
			break;
		default:
			break;
	}

    return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_register_spi(void)
{
    int rv = 0;

    rv = rtems_libi2c_initialize();
	RTEMS_CHECK_RV_SC( rv, "rtems_libi2c_initialize");

	rv = rtems_libi2c_register_bus("/dev/spi0", (rtems_libi2c_bus_t *) &kinetis_spi_bus_table[0]);
	RTEMS_CHECK_RV_SC( rv, "/dev/spi0");
	spi0_busno = rv;

	rv = rtems_libi2c_register_bus("/dev/spi1", (rtems_libi2c_bus_t *) &kinetis_spi_bus_table[1]);
	RTEMS_CHECK_RV_SC( rv, "/dev/spi1");
	spi1_busno = rv;
	
	/* if flash was registered normally, we can get /dev/spi0.flash */
    rv = rtems_libi2c_register_drv("flash",
				       spi_flash_w25q80_driver_descriptor,
				       spi0_busno,RTEMS_BSP_KINETIS_FLASH_ADDR);
	RTEMS_CHECK_RV_SC( rv, "register flash device");

	return RTEMS_SUCCESSFUL;
}

void bsp_register_spi_on_boot_time(void)
{
    if (bsp_register_spi() != RTEMS_SUCCESSFUL)
    {
        printk("[FATAL]Failed to register spi bus.\n");
    }
}