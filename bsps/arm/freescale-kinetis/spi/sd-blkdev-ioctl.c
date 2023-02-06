#include <errno.h>

#include <rtems/blkdev.h>
#include <rtems/status-checks.h>

#include <bsp/spi_sd.h>
#include <bsp/sd-blkdev.h>
#include <bsp/psram.h>

static int sd_card_block_read(rtems_blkdev_request *r)
{
	int rv = 0;
	uint32_t start_address = RTEMS_BLKDEV_START_BLOCK (r);
	uint32_t i = 0;
    unsigned char buf[512];

	if (r->bufnum == 1)
    {
		/* Single block read */
		rv = sd_read_sector(start_address, buf);
        kinetis_memcpy((char *) r->bufs [0].buffer, (const char *)buf, 512);
		RTEMS_CHECK_RV( rv, "Read: SD_CARD_CMD_READ_SINGLE_BLOCK");
	}
    else
    {
		/* Multiple block read */
        for (i = 0; i < r->bufnum; ++i)
        {
            rv = sd_read_sector(start_address, buf);
            kinetis_memcpy((char *) r->bufs [i].buffer, (const char *)buf, 512);
		    RTEMS_CHECK_RV( rv, "Read block");
        }
	}

	/* Done */
	rtems_blkdev_request_done( r, RTEMS_SUCCESSFUL);

	return RTEMS_SUCCESSFUL;
}

static int sd_card_block_write(rtems_blkdev_request *r)
{
	int rv = 0;
	uint32_t start_address = RTEMS_BLKDEV_START_BLOCK (r) << 9;
    uint32_t i = 0;
    unsigned char buf[512];

	if (r->bufnum == 1)
    {
        kinetis_memcpy((char *)buf, (const char *) r->bufs [0].buffer, 512);
		rv = sd_write_sector(start_address, buf);
		RTEMS_CHECK_RV( rv, "Write: SD_CARD_CMD_WRITE_BLOCK");
	}
    else
    {
		/* Multiple block write */
        for (i = 0; i < r->bufnum; ++i)
        {
            kinetis_memcpy((char *)buf, (const char *) r->bufs [i].buffer, 512);
            rv = sd_write_sector(start_address, buf);
            RTEMS_CHECK_RV( rv, "Write block");
        }
	}

	/* Done */
	rtems_blkdev_request_done( r, RTEMS_SUCCESSFUL);

	return RTEMS_SUCCESSFUL;
}

int sd_card_blkdev_ioctl( rtems_disk_device *dd, uint32_t req, void *arg)
{
    if (req == RTEMS_BLKIO_REQUEST)
    {
		rtems_blkdev_request *r = (rtems_blkdev_request *) arg;
		int result;

		switch (r->req) {
			case RTEMS_BLKDEV_REQ_READ:
				result = sd_card_block_read(r);
				break;
			case RTEMS_BLKDEV_REQ_WRITE:
				result = sd_card_block_write(r);
				break;
			default:
				errno = EINVAL;
				return -1;
		}

		return result;
	}
    else if (req == RTEMS_BLKIO_CAPABILITIES)
    {
		*(uint32_t *) arg = RTEMS_BLKDEV_CAP_MULTISECTOR_CONT;
		return 0;
	}
    else
    {
		return rtems_blkdev_ioctl( dd, req, arg );
	}
}

int sd_card_blkdev_bnum(uint32_t *pNum)
{
    uint32_t bnum = sd_get_total_sectors_num();
    if (bnum != GET_CSD_ERROR)
    {
        *pNum = bnum;
        return RTEMS_SUCCESSFUL;
    }
    else
    {
        return RTEMS_INVALID_NUMBER;
    }
}