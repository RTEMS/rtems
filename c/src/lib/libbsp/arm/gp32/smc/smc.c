/* smc.c -- s3c2400 smc disk block device implementation

 Squidge's SMC Low-level access routines.
 Inspired and derived from routines provided by Samsung Electronics M/M R&D Center & FireFly.

*/

#include <rtems.h>
#include <rtems/libio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "rtems/blkdev.h"
#include "rtems/diskdevs.h"
#include "smc.h"
#include <rtems/bspIo.h>
#include <s3c24xx.h>

#define SMC_DEVICE_NAME "/dev/smc"
#define SMC_SAMSUNG_ID		0xEC
#define SMC_TOSHIBA_ID		0x98

#define SMC_16MB	0x73
#define SMC_32MB	0x75
#define SMC_64MB	0x76
#define SMC_128MB	0x79

#define LBA_UNUSED		0x80000000
#define LBA_RESERVED		0x80000001

#define BLOCK_UNUSED		0x80000000
#define BLOCK_RESERVED		0x80000001

/* SmartMedia Command */
#define SEQ_DATA_INPUT_CMD	0x80
#define READ1_CMD		0x00
#define READ1_1_CMD		0x01
#define READ2_CMD		0x50
#define READ_ID_CMD		0x90
#define RESET_CMD		0xFF
#define PAGE_PROGRAM_CMD	0x10
#define BLOCK_ERASE_CMD		0x60
#define BLOCK_ERASE_CFM_CMD	0xD0
#define READ_STATUS_CMD		0x70
#define RESET_PTR_CMD		0x00


/* Internal SMC disk descriptor */
struct SMC_INFO
{
  uint8_t  id[3];
  uint32_t bytes_per_page;
  uint32_t pages_per_block;
  uint32_t blocks;
  uint32_t mb;
};

/* Ths S3c2410 uses a different register map */
#ifdef CPU_S3C2410
#define rPBDAT rGPBDAT
#define rPBCON rGPBCON
#define rPDDAT rGPDDAT
#define rPEDAT rGPEDAT
#endif


static struct SMC_INFO smc_info;

uint32_t smc_l2p[0x2000];
uint32_t smc_p2l[0x2000];

#define sm_busy() while (!(rPDDAT & 0x200))
#define sm_chip_en() rPDDAT &= (~0x80)
#define sm_chip_dis() rPDDAT |= 0x80
#define sm_cle_en() rPEDAT |= 0x20
#define sm_cle_dis() rPEDAT &= (~0x20)
#define sm_ale_en() rPEDAT |= 0x10
#define sm_ale_dis() rPEDAT &= (~0x10)
#define sm_wp_en() rPDDAT &= (~0x40)
#define sm_wp_dis() rPDDAT |= 0x40
#define sm_read_en() rPBCON &= 0xFFFF0000
#define sm_read_dis() rPBCON = (rPBCON & 0xFFFF0000) | 0x5555
#define sm_write_en() sm_read_dis()
#define sm_write_dis() sm_read_en()

static void sm_write( uint8_t data)
{
  rPBDAT = (rPBDAT & 0xFF00) | data;
  rPEDAT &= (~0x08);
  rPEDAT |= 0x08;
}

static uint8_t sm_read(void)
{
  uint8_t data;

  rPDDAT &= (~0x100);
  data = rPBDAT & 0xFF;
  rPDDAT |= 0x100;
  return data;
}


/* assumes chip enabled
   bit 7: write protected = 0, write enabled = 1
   bit 6: busy = 0, ready = 1
   bit 0: success = 0, failed = 1

   returns 1 on success, 0 on fail
*/
#if UNUSED
static uint8_t sm_status()
{
  uint8_t status;

  sm_cle_en();
  sm_write_en();
  sm_write(READ_STATUS_CMD);
  sm_write_dis();
  sm_cle_dis();

  sm_read_en();
  status = sm_read();
  sm_read_dis();

  if (status == 0xC0)
    return 1;
  else
    return 0;
}
#endif

void smc_read_id( uint8_t* buf, uint32_t length)
{

  uint32_t i;

  sm_chip_en();

  sm_cle_en();
  sm_write_en();
  sm_write(READ_ID_CMD);
  sm_write_dis();
  sm_cle_dis();

  sm_ale_en();
  sm_write_en();
  sm_write( 0);
  sm_write_dis();
  sm_ale_dis();

  sm_read_en();
  for (i=0;i<length;i++) *(buf+i) = sm_read();
  sm_read_dis();

  sm_chip_dis();
}

/* read an entire logical page of 512 bytes.*/
uint8_t smc_read_page (uint32_t lpage, uint8_t* buf)
{
	uint32_t block, page, i;

	/* convert logical block to physical block
	   and then convert into page suitable for read1 command...
	*/
	block = lpage >> 5;
	if (smc_l2p[block] < LBA_UNUSED) {
		page = smc_l2p[block] << 5;
		page += (lpage & 0x1F);
	}
	else
		return 0;

	sm_chip_en();

	sm_cle_en();
	sm_write_en();
	sm_write(READ1_CMD);
	sm_write_dis();
	sm_cle_dis();

			sm_ale_en();
			sm_write_en();
			sm_write( 0x00);
			sm_write( (uint8_t)(page >> 0));
			sm_write( (uint8_t)(page >> 8));
			if (smc_info.mb >= 64) sm_write( (uint8_t)(page >> 16));
			sm_write_dis();
			sm_ale_dis();

			sm_busy();

			sm_read_en();
			for (i = 0; i < 512; i++)
			{
				*buf = sm_read();
				buf++;
			}
			sm_read_dis();
			sm_chip_dis();

			sm_busy();
	return 1;
}

void smc_read_spare( uint32_t page, uint8_t* buf, uint8_t length)
{
  uint32_t i;


  sm_chip_en();

  sm_cle_en();
  sm_read_dis();
  sm_write(READ2_CMD);
  sm_read_en();
  sm_cle_dis();

  sm_ale_en();
  sm_read_dis();
  sm_write( 0x00);
  sm_write( (uint8_t)(page >> 0));
  sm_write( (uint8_t)(page >> 8));
  if (smc_info.mb >= 64) sm_write( (uint8_t)(page >> 16));
  sm_read_en();
  sm_ale_dis();

  sm_busy();

  sm_read_en();
  for (i=0;i<length;i++) *(buf+i) = sm_read();
  sm_read_dis();

  sm_chip_dis();

}

void smc_make_l2p(void)
{
  uint32_t pblock, i, j, lblock, zone, count, cnt1, cnt2, cnt3;
  uint8_t data[512];

  cnt1 = 0;
  cnt2 = 0;
  cnt3 = 0;

  for (i=0;i<0x2000;i++)
  {
    smc_l2p[i] = LBA_RESERVED;
    smc_p2l[i] = BLOCK_RESERVED;
  }
  for (pblock=0;pblock<smc_info.blocks;pblock++)
  {
    /* read physical block - first page */
    smc_read_spare( pblock*smc_info.pages_per_block, (uint8_t*)&data, 16);

    zone = pblock >> 10; /* divide by 1024 to get zone */
    if ((data[5] == 0xFF) && ((data[6]&0xF8) == 0x10))
    {
      lblock = ((((data[6]<<8)|(data[7]<<0)) >> 1) & 0x03FF) + (zone * 1000);
      smc_l2p[lblock] = pblock;
      smc_p2l[pblock] = lblock;
      cnt1++;
    }
    else
    {
      count = 0;
      for (j=0;j<16;j++)
      {
        if (data[j] == 0xFF) count++;
      }
      if (count == 16)
      {
      	smc_p2l[pblock] = BLOCK_UNUSED;
      	cnt2++;
      }
      else
      {
      	smc_p2l[pblock] = BLOCK_RESERVED;
      	cnt3++;
      }
    }
  }
}


void smc_detect( uint8_t id1, uint8_t id2, uint8_t id3)
{
  smc_info.id[0] = id1;
  smc_info.id[1] = id2;
  smc_info.id[2] = id3;
  smc_info.mb    = 0;
  smc_info.bytes_per_page  = 0;
  smc_info.pages_per_block = 0;
  smc_info.blocks          = 0;

  switch (id1)
  {
    case SMC_SAMSUNG_ID:
    case SMC_TOSHIBA_ID:
    {
      switch (id2)
      {
        case SMC_16MB  : smc_info.mb = 16; break;
        case SMC_32MB  : smc_info.mb = 32; break;
        case SMC_64MB  : smc_info.mb = 64; break;
        case SMC_128MB : smc_info.mb = 128; break;
      }
      break;
    }
  }

  switch (smc_info.mb)
  {
    case 16  : smc_info.bytes_per_page = 512; smc_info.pages_per_block = 32; smc_info.blocks = 0x0400; break;
    case 32  : smc_info.bytes_per_page = 512; smc_info.pages_per_block = 32; smc_info.blocks = 0x0800; break;
    case 64  : smc_info.bytes_per_page = 512; smc_info.pages_per_block = 32; smc_info.blocks = 0x1000; break;
    case 128 : smc_info.bytes_per_page = 512; smc_info.pages_per_block = 32; smc_info.blocks = 0x2000; break;
  }
}

void smc_init( void)
{
	unsigned char buf[32];
	int i;

	/* reset smc */
	sm_chip_en();
	sm_cle_en();
	sm_write_en();
	sm_write(0xFF);
	sm_write_dis();
	sm_cle_dis();
	for(i=0;i<10;i++);
	sm_busy();
	sm_chip_dis();

	smc_read_id (buf, 4);
	smc_detect (buf[0], buf[1], buf[2]);
	printk ("SMC: [%02X-%02X-%02X-%02X]\n", buf[0], buf[1], buf[2], buf[3]);
	printk ("SMC size: %dMB detected\n",smc_info.mb);
	smc_make_l2p();
}

/**********
 * Function: sm_ECCEncode (completely ripped, unaltered, from the samsung routines)
 * Remark:
 *	- adopted from "ECC Algorithm for SmartMedia V3.0"
 *		by Memory Product & Technology, Samsung Electronics Co. (ecc30.pdf)
 **********/
int sm_ECCEncode(const uint8_t * p_buf, uint8_t * p_ecc)
{
	uint32_t i, j;
	uint8_t paritr[256], tmp = 0, tmp2 = 0;
	uint8_t data_table0[16] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };
	uint8_t data_table1[16] = { 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1 };
	uint8_t sum = 0, paritc = 0;
	uint8_t parit0c = 0, parit1c = 0, parit2c = 0, parit3c = 0;
	uint8_t parit4c = 0, parit5c = 0, parit6c = 0, parit7c = 0;
	uint8_t parit1_1, parit1_2, parit2_1, parit2_2, parit4_1, parit4_2;
	uint8_t parit8_1 = 0, parit8_2 = 0, parit16_1 = 0, parit16_2 = 0, parit32_1 = 0, parit32_2 = 0;
	uint8_t parit64_1 = 0, parit64_2 = 0, parit128_1 = 0, parit128_2 = 0, parit256_1 = 0, parit256_2 = 0;
	uint8_t parit512_1 = 0, parit512_2 = 0, parit1024_1 = 0, parit1024_2 = 0;
	uint8_t* paritr_ptr;

	paritr_ptr = paritr;
	for (i = 0; i < 256; ++i, ++paritr_ptr, ++p_buf)
	{
		paritc ^= *p_buf;
		tmp = (*p_buf & 0xf0) >> 4;
		tmp2 = *p_buf & 0x0f;

		switch (tmp)
		{
			case 0:
			case 3:
			case 5:
			case 6:
			case 9:
			case 10:
			case 12:
			case 15:
				*paritr_ptr = *(data_table0 + tmp2);
				break;

			case 1:
			case 2:
			case 4:
			case 7:
			case 8:
			case 11:
			case 13:
			case 14:
				*paritr_ptr = *(data_table1 + tmp2);
				break;
		}
	}

	parit0c = (paritc & 0x01) ? 1 : 0;
	parit1c = (paritc & 0x02) ? 1 : 0;
	parit2c = (paritc & 0x04) ? 1 : 0;
	parit3c = (paritc & 0x08) ? 1 : 0;
	parit4c = (paritc & 0x10) ? 1 : 0;
	parit5c = (paritc & 0x20) ? 1 : 0;
	parit6c = (paritc & 0x40) ? 1 : 0;
	parit7c = (paritc & 0x80) ? 1 : 0;
	parit1_2 = parit6c ^ parit4c ^ parit2c ^ parit0c;
	parit1_1 = parit7c ^ parit5c ^ parit3c ^ parit1c;
	parit2_2 = parit5c ^ parit4c ^ parit1c ^ parit0c;
	parit2_1 = parit7c ^ parit6c ^ parit3c ^ parit2c;
	parit4_2 = parit3c ^ parit2c ^ parit1c ^ parit0c;
	parit4_1 = parit7c ^ parit6c ^ parit5c ^ parit4c;

	paritr_ptr = paritr;
	for (i = 0; i < 256; ++i, ++paritr_ptr)
	{
		sum ^= *paritr_ptr;
	}

	paritr_ptr = paritr;
	for (i = 0; i < 256; i += 2, paritr_ptr += 2)
	{
		parit8_2 ^= *paritr_ptr;
	}

	paritr_ptr = paritr;
	for (i = 0; i < 256; i += 4, paritr_ptr += 4)
	{
		parit16_2 ^= *paritr_ptr;
		parit16_2 ^= *(paritr_ptr + 1);
	}

	paritr_ptr = paritr;
	for (i = 0; i < 256; i += 8, paritr_ptr += 8)
	{
		for (j = 0; j <= 3; ++j)
		{
			parit32_2 ^= *(paritr_ptr + j);
		}
	}

	paritr_ptr = paritr;
	for (i = 0; i < 256; i += 16, paritr_ptr += 16)
	{
		for (j = 0; j <= 7; ++j)
		{
			parit64_2 ^= *(paritr_ptr + j);
		}
	}

	paritr_ptr = paritr;
	for (i = 0; i < 256; i += 32, paritr_ptr += 32)
	{
		for (j = 0; j <= 15; ++j)
		{
			parit128_2 ^= *(paritr_ptr + j);
		}
	}

	paritr_ptr = paritr;
	for (i = 0; i < 256; i += 64, paritr_ptr += 64)
	{
		for (j = 0; j <= 31; ++j)
		{
			parit256_2 ^= *(paritr_ptr + j);
		}
	}

	paritr_ptr = paritr;
	for (i = 0; i < 256; i += 128, paritr_ptr += 128)
	{
		for (j = 0; j <= 63; ++j)
		{
			parit512_2 ^= *(paritr_ptr + j);
		}
	}

	paritr_ptr = paritr;
	for (i = 0; i < 256; i += 256, paritr_ptr += 256)
	{
		for (j = 0; j <= 127; ++j)
		{
			parit1024_2 ^= *(paritr_ptr + j);
		}
	}

	if (sum==0)
	{
		parit1024_1 = parit1024_2;
		parit512_1 = parit512_2;
		parit256_1 = parit256_2;
		parit128_1 = parit128_2;
		parit64_1 = parit64_2;
		parit32_1 = parit32_2;
		parit16_1 = parit16_2;
		parit8_1 = parit8_2;
	}
	else
	{
		parit1024_1 = parit1024_2 ? 0 : 1;
		parit512_1 = parit512_2 ? 0 : 1;
		parit256_1 = parit256_2 ? 0 : 1;
		parit128_1 = parit128_2 ? 0 : 1;
		parit64_1 = parit64_2 ? 0 : 1;
		parit32_1 = parit32_2 ? 0 : 1;
		parit16_1 = parit16_2 ? 0 : 1;
		parit8_1 = parit8_2 ? 0 : 1;
	}

	parit1_2 <<= 2;
	parit1_1 <<= 3;
	parit2_2 <<= 4;
	parit2_1 <<= 5;
	parit4_2 <<= 6;
	parit4_1 <<= 7;
	parit128_1 <<= 1;
	parit256_2 <<= 2;
	parit256_1 <<= 3;
	parit512_2 <<= 4;
	parit512_1 <<= 5;
	parit1024_2 <<= 6;
	parit1024_1 <<= 7;
	parit8_1 <<= 1;
	parit16_2 <<= 2;
	parit16_1 <<= 3;
	parit32_2 <<= 4;
	parit32_1 <<= 5;
	parit64_2 <<= 6;
	parit64_1 <<= 7;

	p_ecc[0] = ~(parit64_1 | parit64_2 | parit32_1 | parit32_2 | parit16_1 | parit16_2 | parit8_1 | parit8_2);
	p_ecc[1] = ~(parit1024_1 |parit1024_2 | parit512_1 | parit512_2 | parit256_1 | parit256_2 | parit128_1 | parit128_2);
	p_ecc[2] = ~(parit4_1 | parit4_2 | parit2_1 | parit2_2 | parit1_1 | parit1_2);

	return 0;
}

/* smc_write --
 * write stub
*/
static int smc_write(rtems_blkdev_request *req)
{
	req->req_done(req->done_arg, RTEMS_SUCCESSFUL);
	return 0;
}


/* smc_read --
 * PARAMETERS:
 *     req - pointer to the READ block device request info
 *
 * RETURNS:
 *     ioctl return value
 */
static int
smc_read(rtems_blkdev_request *req)
{
    uint32_t   i;
    rtems_blkdev_sg_buffer *sg;
    uint32_t   remains;

    remains = smc_info.bytes_per_page * req->bufnum;
    sg = req->bufs;
    for (i = 0; (remains > 0) && (i < req->bufnum); i++, sg++)
    {
        int count = sg->length;
        if (count > remains)
            count = remains;
	smc_read_page(sg->block,sg->buffer);
        remains -= count;
    }
    req->req_done(req->done_arg, RTEMS_SUCCESSFUL);
    return 0;
}

/* smc_ioctl --
 *     IOCTL handler for SMC device.
 *
 * PARAMETERS:
 *      dev  - device number (major, minor number)
 *      req  - IOCTL request code
 *      argp - IOCTL argument
 *
 * RETURNS:
 *     IOCTL return value
 */
static int
smc_ioctl(rtems_disk_device *dd, uint32_t req, void *argp)
{
    switch (req)
    {
        case RTEMS_BLKIO_REQUEST:
        {
            rtems_blkdev_request *r = argp;
            switch (r->req)
            {
                case RTEMS_BLKDEV_REQ_READ:
                    return smc_read(r);
                case RTEMS_BLKDEV_REQ_WRITE:
		    return smc_write(r);
                default:
                    errno = EINVAL;
                    return -1;
            }
            break;
        }

        default:
            errno = EINVAL;
            return -1;
    }
}

/* smc_initialize --
 *     RAM disk device driver initialization. Run through RAM disk
 *     configuration information and configure appropriate RAM disks.
 *
 * PARAMETERS:
 *     major - RAM disk major device number
 *     minor - minor device number, not applicable
 *     arg   - initialization argument, not applicable
 *
 * RETURNS:
 *     none
 */
rtems_device_driver
smc_initialize(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg)
{
    rtems_status_code rc;
    dev_t dev;
    uint32_t block_num;

    rc = rtems_disk_io_initialize();
    if (rc != RTEMS_SUCCESSFUL)
        return rc;

    smc_init();
    block_num = smc_info.blocks << 5;

    dev = rtems_filesystem_make_dev_t(major, 0);
    rc = rtems_disk_create_phys(dev, 512, block_num,
                                    smc_ioctl, NULL, SMC_DEVICE_NAME);

    return RTEMS_SUCCESSFUL;
}
