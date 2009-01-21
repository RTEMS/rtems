/**
 * @file
 *
 * @brief SD Card LibI2C driver.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#include <stdio.h>
#include <errno.h>
#include <inttypes.h>

#include <rtems.h>
#include <rtems/libi2c.h>
#include <rtems/libio.h>
#include <rtems/diskdevs.h>

#include <libchip/spi-sd-card.h>

#include <rtems/status-checks.h>

/**
 * @name Integer to and from Byte-Stream Converter
 * @{
 */

static inline uint16_t sd_card_get_uint16( const uint8_t *s)
{
	return (uint16_t) ((s [0] << 8) | s [1]);
}

static inline uint32_t sd_card_get_uint32( const uint8_t *s)
{
	return ((uint32_t) s [0] << 24) | ((uint32_t) s [1] << 16) | ((uint32_t) s [2] << 8) | (uint32_t) s [3];
}

static inline void sd_card_put_uint16( uint16_t v, uint8_t *s)
{
	*s++ = (uint8_t) (v >> 8);
	*s   = (uint8_t) (v);
}

static inline void sd_card_put_uint32( uint32_t v, uint8_t *s)
{
	*s++ = (uint8_t) (v >> 24);
	*s++ = (uint8_t) (v >> 16);
	*s++ = (uint8_t) (v >> 8);
	*s   = (uint8_t) (v);
}

/** @} */

#define SD_CARD_BUSY_TOKEN 0

#define SD_CARD_BLOCK_SIZE_DEFAULT 512

#define SD_CARD_COMMAND_RESPONSE_START 7

/**
 * @name Commands
 * @{
 */

#define SD_CARD_CMD_GO_IDLE_STATE 0
#define SD_CARD_CMD_SEND_OP_COND 1
#define SD_CARD_CMD_SEND_CSD 9
#define SD_CARD_CMD_SEND_CID 10
#define SD_CARD_CMD_STOP_TRANSMISSION 12
#define SD_CARD_CMD_SEND_STATUS 13
#define SD_CARD_CMD_SET_BLOCKLEN 16
#define SD_CARD_CMD_READ_SINGLE_BLOCK 17
#define SD_CARD_CMD_READ_MULTIPLE_BLOCK 18
#define SD_CARD_CMD_SET_BLOCK_COUNT 23
#define SD_CARD_CMD_WRITE_BLOCK 24
#define SD_CARD_CMD_WRITE_MULTIPLE_BLOCK 25
#define SD_CARD_CMD_PROGRAM_CSD 27
#define SD_CARD_CMD_SET_WRITE_PROT 28
#define SD_CARD_CMD_CLR_WRITE_PROT 29
#define SD_CARD_CMD_SEND_WRITE_PROT 30
#define SD_CARD_CMD_TAG_SECTOR_START 32
#define SD_CARD_CMD_TAG_SECTOR_END 33
#define SD_CARD_CMD_UNTAG_SECTOR 34
#define SD_CARD_CMD_TAG_ERASE_GROUP_START 35
#define SD_CARD_CMD_TAG_ERASE_GROUP_END 36
#define SD_CARD_CMD_UNTAG_ERASE_GROUP 37
#define SD_CARD_CMD_ERASE 38
#define SD_CARD_CMD_LOCK_UNLOCK 42
#define SD_CARD_CMD_APP_CMD 55
#define SD_CARD_CMD_GEN_CND 56
#define SD_CARD_CMD_READ_OCR 58
#define SD_CARD_CMD_CRC_ON_OFF 59

/** @} */

/**
 * @name Application Commands
 * @{
 */

#define SD_CARD_ACMD_SD_SEND_OP_COND 41

/** @} */

/**
 * @name Command Fields
 * @{
 */

#define SD_CARD_COMMAND_SET_COMMAND( c, cmd) (c) [1] = (uint8_t) (0x40 + ((cmd) & 0x3f))
#define SD_CARD_COMMAND_SET_ARGUMENT( c, arg) sd_card_put_uint32( (arg), &((c) [2]))
#define SD_CARD_COMMAND_SET_CRC7( c, crc7) ((c) [6] = (crc7) << 1)

#define SD_CARD_COMMAND_GET_CRC7( c) ((c) [6] >> 1)

/** @} */

/**
 * @name Response Fields
 * @{
 */

#define SD_CARD_IS_RESPONSE( r) (((r) & 0x80) == 0)

#define SD_CARD_IS_ERRORLESS_RESPONSE( r) (((r) & 0x7e) == 0)

#define SD_CARD_IS_NOT_IDLE_RESPONSE( r) (((r) & 0x81) == 0)

#define SD_CARD_IS_DATA_ERROR( r) (((r) & 0xe0) == 0)

#define SD_CARD_IS_DATA_REJECTED( r) (((r) & 0x1f) != 0x05)

/** @} */

/**
 * @name Card Identification
 * @{
 */

#define SD_CARD_CID_SIZE 16

#define SD_CARD_CID_GET_MID( cid) ((cid) [0])
#define SD_CARD_CID_GET_OID( cid) sd_card_get_uint16( cid + 1)
#define SD_CARD_CID_GET_PNM( cid, i) ((char) (cid) [3 + (i)])
#define SD_CARD_CID_GET_PRV( cid) ((cid) [9])
#define SD_CARD_CID_GET_PSN( cid) sd_card_get_uint32( cid + 10)
#define SD_CARD_CID_GET_MDT( cid) ((cid) [14])
#define SD_CARD_CID_GET_CRC7( cid) ((cid) [15] >> 1)

/** @} */

/**
 * @name Card Specific Data
 * @{
 */

#define SD_CARD_CSD_SIZE 16

#define SD_CARD_CSD_GET_CSD_STRUCTURE( csd) ((csd) [0] >> 6)
#define SD_CARD_CSD_GET_SPEC_VERS( csd) (((csd) [0] >> 2) & 0xf)
#define SD_CARD_CSD_GET_TAAC( csd) ((csd) [1])
#define SD_CARD_CSD_GET_NSAC( csd) ((uint32_t) (csd) [2])
#define SD_CARD_CSD_GET_TRAN_SPEED( csd) ((csd) [3])
#define SD_CARD_CSD_GET_C_SIZE( csd) ((((uint32_t) (csd) [6] & 0x3) << 10) + ((uint32_t) (csd) [7] << 2) + (((uint32_t) (csd) [8] >> 6) & 0x3))
#define SD_CARD_CSD_GET_C_SIZE_MULT( csd) ((((csd) [9] & 0x3) << 1) + (((csd) [10] >> 7) & 0x1))
#define SD_CARD_CSD_GET_READ_BLK_LEN( csd) ((uint32_t) (csd) [5] & 0xf)
#define SD_CARD_CSD_GET_WRITE_BLK_LEN( csd) ((((csd) [12] & 0x3) << 2) + (((csd) [13] >> 6) & 0x3))

/** @} */

#define SD_CARD_INVALIDATE_RESPONSE_INDEX( e) e->response_index = SD_CARD_COMMAND_SIZE

/**
 * @name Data Start and Stop Tokens
 * @{
 */

#define SD_CARD_START_BLOCK_SINGLE_BLOCK_READ 0xfe
#define SD_CARD_START_BLOCK_MULTIPLE_BLOCK_READ 0xfe
#define SD_CARD_START_BLOCK_SINGLE_BLOCK_WRITE 0xfe
#define SD_CARD_START_BLOCK_MULTIPLE_BLOCK_WRITE 0xfc
#define SD_CARD_STOP_TRANSFER_MULTIPLE_BLOCK_WRITE 0xfd

/** @} */

/**
 * @name Card Specific Data Functions
 * @{
 */

static inline uint32_t sd_card_block_number( const uint8_t *csd)
{
	uint32_t size = SD_CARD_CSD_GET_C_SIZE( csd);
	uint32_t mult = 1U << (SD_CARD_CSD_GET_C_SIZE_MULT( csd) + 2);
	return (size + 1) * mult;
}

static inline uint32_t sd_card_capacity( const uint8_t *csd)
{
	uint32_t block_size = 1U << SD_CARD_CSD_GET_READ_BLK_LEN( csd);
	return sd_card_block_number( csd) * block_size;
}

static inline uint32_t sd_card_transfer_speed( const uint8_t *csd)
{
	uint32_t s = SD_CARD_CSD_GET_TRAN_SPEED( csd);
	uint32_t e = s & 0x7;
	uint32_t m = s >> 3;
	switch (e) {
		case 0: s = 10000; break;
		case 1: s = 100000; break;
		case 2: s = 1000000; break;
		case 3: s = 10000000; break;
		default: s = 0; break;
	}
	switch (m) {
		case 1: s *= 10; break;
		case 2: s *= 12; break;
		case 3: s *= 13; break;
		case 4: s *= 15; break;
		case 5: s *= 20; break;
		case 6: s *= 25; break;
		case 7: s *= 30; break;
		case 8: s *= 35; break;
		case 9: s *= 40; break;
		case 10: s *= 45; break;
		case 11: s *= 50; break;
		case 12: s *= 55; break;
		case 13: s *= 60; break;
		case 14: s *= 70; break;
		case 15: s *= 80; break;
		default: s *= 0; break;
	}
	return s;
}

static inline uint32_t sd_card_access_time( const uint8_t *csd)
{
	uint32_t ac = SD_CARD_CSD_GET_TAAC( csd);
	uint32_t e = ac & 0x7;
	uint32_t m = ac >> 3;
	switch (e) {
		case 0: ac = 1; break;
		case 1: ac = 10; break;
		case 2: ac = 100; break;
		case 3: ac = 1000; break;
		case 4: ac = 10000; break;
		case 5: ac = 100000; break;
		case 6: ac = 1000000; break;
		case 7: ac = 10000000; break;
		default: ac = 0; break;
	}
	switch (m) {
		case 1: ac *= 10; break;
		case 2: ac *= 12; break;
		case 3: ac *= 13; break;
		case 4: ac *= 15; break;
		case 5: ac *= 20; break;
		case 6: ac *= 25; break;
		case 7: ac *= 30; break;
		case 8: ac *= 35; break;
		case 9: ac *= 40; break;
		case 10: ac *= 45; break;
		case 11: ac *= 50; break;
		case 12: ac *= 55; break;
		case 13: ac *= 60; break;
		case 14: ac *= 70; break;
		case 15: ac *= 80; break;
		default: ac *= 0; break;
	}
	return ac / 10;
}

static inline uint32_t sd_card_max_access_time( const uint8_t *csd, uint32_t transfer_speed)
{
	uint64_t ac = sd_card_access_time( csd);
	uint32_t n = SD_CARD_CSD_GET_NSAC( csd) * 100;
	ac = (ac * transfer_speed) / 8000000000ULL;
	return n + (uint32_t) ac;
}

/** @} */

/**
 * @name Communication Functions
 * @{
 */

static inline int sd_card_query( sd_card_driver_entry *e, uint8_t *in, int n)
{
	return rtems_libi2c_read_bytes( e->minor, in, n);
}

static int sd_card_wait( sd_card_driver_entry *e)
{
	int rv = 0;
	int r = 0;
	int n = 2;
	while (e->busy) {
		/* Query busy tokens */
		rv = sd_card_query( e, e->response, n);
		CHECK_RV( rv, "Busy");

		/* Search for non busy tokens */
		for (r = 0; r < n; ++r) {
			if (e->response [r] != SD_CARD_BUSY_TOKEN) {
				e->busy = false;
				return 0;
			}
		}
		n = SD_CARD_COMMAND_SIZE;

		if (e->schedule_if_busy) {
			/* Invoke the scheduler */
			rtems_task_wake_after( RTEMS_YIELD_PROCESSOR);
		}
	}
	return 0;
}

static int sd_card_send_command( sd_card_driver_entry *e, uint32_t command, uint32_t argument)
{
	int rv = 0;
	rtems_libi2c_read_write_t rw = {
		.rd_buf = e->response,
		.wr_buf = e->command,
		.byte_cnt = SD_CARD_COMMAND_SIZE
	};
	int r = 0;

	SD_CARD_INVALIDATE_RESPONSE_INDEX( e);

	/* Wait until card is not busy */
	rv = sd_card_wait( e);
	CHECK_RV( rv, "Wait");

	/* Write command and read response */
	SD_CARD_COMMAND_SET_COMMAND( e->command, command);
	SD_CARD_COMMAND_SET_ARGUMENT( e->command, argument);
	rv = rtems_libi2c_ioctl( e->minor, RTEMS_LIBI2C_IOCTL_READ_WRITE, &rw);
	CHECK_RV( rv, "Write command and read response");

	/* Check respose */
	for (r = SD_CARD_COMMAND_RESPONSE_START; r < SD_CARD_COMMAND_SIZE; ++r) {
		DEBUG_PRINT( "Token [%02u]: 0x%02x\n", r, e->response [r]);
		e->response_index = r;
		if (SD_CARD_IS_RESPONSE( e->response [r])) {
			if (SD_CARD_IS_ERRORLESS_RESPONSE( e->response [r])) {
				return 0;
			} else {
				SYSLOG_ERROR( "Command error [%02i]: 0x%02" PRIx8 "\n", r, e->response [r]);
				goto sd_card_send_command_error;
			}
		} else if (e->response [r] != SD_CARD_IDLE_TOKEN) {
			SYSLOG_ERROR( "Unexpected token [%02i]: 0x%02" PRIx8 "\n", r, e->response [r]);
			goto sd_card_send_command_error;
		}
	}

	SYSLOG_ERROR( "Timeout\n");

sd_card_send_command_error:

	SYSLOG_ERROR( "Response:");
	for (r = 0; r < SD_CARD_COMMAND_SIZE; ++r) {
		if (e->response_index == r) {
			SYSLOG_PRINT( " %02" PRIx8 ":[%02" PRIx8 "]", e->command [r], e->response [r]);
		} else {
			SYSLOG_PRINT( " %02" PRIx8 ":%02" PRIx8 "", e->command [r], e->response [r]);
		}
	}
	SYSLOG_PRINT( "\n");

	return -RTEMS_IO_ERROR;
}

static int sd_card_stop_multiple_block_read( sd_card_driver_entry *e)
{
	int rv = 0;

	SD_CARD_COMMAND_SET_COMMAND( e->command, SD_CARD_CMD_STOP_TRANSMISSION);
	rv = rtems_libi2c_write_bytes( e->minor, e->command, SD_CARD_COMMAND_SIZE);
	CHECK_RV( rv, "Write stop transfer token");

	return 0;
}

static int sd_card_stop_multiple_block_write( sd_card_driver_entry *e)
{
	int rv = 0;
	uint8_t stop_transfer [3] = { SD_CARD_IDLE_TOKEN, SD_CARD_STOP_TRANSFER_MULTIPLE_BLOCK_WRITE, SD_CARD_IDLE_TOKEN };

	/* Wait until card is not busy */
	rv = sd_card_wait( e);
	CHECK_RV( rv, "Wait");

	/* Send stop token */
	rv = rtems_libi2c_write_bytes( e->minor, stop_transfer, 3);
	CHECK_RV( rv, "Write stop transfer token");

	/* Card is now busy */
	e->busy = true;

	return 0;
}

static int sd_card_read( sd_card_driver_entry *e, uint8_t start_token, uint8_t *in, int n)
{
	int rv = 0;

	/* Access time idle tokens */
	uint32_t n_ac = 1;

	/* Discard command response */
	int r = e->response_index + 1;

	/* Minimum token number before data start */
	int next_response_size = 2;

	/* Standard response size */
	int response_size = SD_CARD_COMMAND_SIZE;

	/* Data input index */
	int i = 0;

	SD_CARD_INVALIDATE_RESPONSE_INDEX( e);

	while (1) {
		DEBUG_PRINT( "Search from %u to %u\n", r, response_size - 1);

		/* Search the data start token in in current response buffer */
		while (r < response_size) {
			DEBUG_PRINT( "Token [%02u]: 0x%02x\n", r, e->response [r]);
			if (n_ac > e->n_ac_max) {
				SYSLOG_ERROR( "Timeout\n");
				return -RTEMS_IO_ERROR;
			} else if (e->response [r] == start_token) {
				/* Discard data start token */
				++r;
				goto sd_card_read_start;
			} else if (SD_CARD_IS_DATA_ERROR( e->response [r])) {
				SYSLOG_ERROR( "Data error token [%02i]: 0x%02" PRIx8 "\n", r, e->response [r]);
				return -RTEMS_IO_ERROR;
			} else if (e->response [r] != SD_CARD_IDLE_TOKEN) {
				SYSLOG_ERROR( "Unexpected token [%02i]: 0x%02" PRIx8 "\n", r, e->response [r]);
				return -RTEMS_IO_ERROR;
			}
			++n_ac;
			++r;
		}

		/* Query more */
		rv = sd_card_query( e, e->response, next_response_size);
		CHECK_RV( rv, "Query data start token");

		/* Set standard query size */
		response_size = next_response_size;
		next_response_size = SD_CARD_COMMAND_SIZE;

		/* Reset start position */
		r = 0;
	}

sd_card_read_start:

	/* Read data */
	while (r < response_size && i < n) {
		in [i++] = e->response [r++];
	}

	/* Read more data? */
	if (i < n) {
		rv = sd_card_query( e, &in [i], n - i);
		CHECK_RV( rv, "Read data");
		i += rv;
	}

	/* Read CRC 16 and N_RC */
	rv = sd_card_query( e, e->response, 3);
	CHECK_RV( rv, "Read CRC 16");

	return i;
}

static int sd_card_write( sd_card_driver_entry *e, uint8_t start_token, uint8_t *out, int n)
{
	int rv = 0;
	uint8_t crc16 [2] = { 0, 0 };

	/* Data output index */
	int o = 0;

	/* Wait until card is not busy */
	rv = sd_card_wait( e);
	CHECK_RV( rv, "Wait");

	/* Write data start token */
	rv = rtems_libi2c_write_bytes( e->minor, &start_token, 1);
	CHECK_RV( rv, "Write data start token");

	/* Write data */
	o = rtems_libi2c_write_bytes( e->minor, out, n);
	CHECK_RV( o, "Write data");

	/* Write CRC 16 */
	rv = rtems_libi2c_write_bytes( e->minor, crc16, 2);
	CHECK_RV( rv, "Write CRC 16");

	/* Read data response */
	rv = sd_card_query( e, e->response, 2);
	CHECK_RV( rv, "Read data response");
	if (SD_CARD_IS_DATA_REJECTED( e->response [0])) {
		SYSLOG_ERROR( "Data rejected: 0x%02" PRIx8 "\n", e->response [0]);
		return -RTEMS_IO_ERROR;
	}

	/* Card is now busy */
	e->busy = true;

	return o;
}

static inline rtems_status_code sd_card_start( sd_card_driver_entry *e)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	int rv = 0;

	sc = rtems_libi2c_send_start( e->minor);
	CHECK_SC( sc, "Send start");

	rv = rtems_libi2c_ioctl( e->minor, RTEMS_LIBI2C_IOCTL_SET_TFRMODE, &e->transfer_mode);
	CHECK_RVSC( rv, "Set transfer mode");

	sc = rtems_libi2c_send_addr( e->minor, 1);
	CHECK_SC( sc, "Send address");

	return RTEMS_SUCCESSFUL;
}

static inline rtems_status_code sd_card_stop( sd_card_driver_entry *e)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	sc = rtems_libi2c_send_stop( e->minor);
	CHECK_SC( sc, "Send stop");

	return RTEMS_SUCCESSFUL;
}
/** @} */

/**
 * @name Disk Driver Functions
 * @{
 */

static int sd_card_disk_block_read( sd_card_driver_entry *e, rtems_blkdev_request *r)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	int rv = 0;
	uint32_t start_address = RTEMS_BLKDEV_START_BLOCK (r) << e->block_size_shift;
	uint32_t i = 0;

	DEBUG_PRINT( "start = %u, bufnum = %u\n", r->start, r->bufnum);

#ifdef DEBUG
	/* Check request */
  if (r->bufs[0].block >= e->block_number) {
		SYSLOG_ERROR( "Start block number out of range");
		return -RTEMS_INTERNAL_ERROR;
	} else if (r->bufnum > e->block_number - RTEMS_BLKDEV_START_BLOCK (r)) {
		SYSLOG_ERROR( "Block count out of range");
		return -RTEMS_INTERNAL_ERROR;
	}
#endif /* DEBUG */

	/* Start */
	sc = sd_card_start( e);
	CLEANUP_SCRV( sc, rv, sd_card_disk_block_read_cleanup, "Start");

	if (r->bufnum == 1) {
#ifdef DEBUG
		/* Check buffer */
		if (r->bufs [0].length != e->block_size) {
			DO_CLEANUP_RV( -RTEMS_INTERNAL_ERROR, rv, sd_card_disk_block_read_cleanup, "Buffer and disk block size are not equal");
		}
		DEBUG_PRINT( "[%02u]: buffer = 0x%08x, size = %u\n", 0, r->bufs [0].buffer, r->bufs [0].length);
#endif /* DEBUG */

		/* Single block read */
		rv = sd_card_send_command( e, SD_CARD_CMD_READ_SINGLE_BLOCK, start_address);
		CLEANUP_RV( rv, sd_card_disk_block_read_cleanup, "Send: SD_CARD_CMD_READ_SINGLE_BLOCK");
		rv = sd_card_read( e, SD_CARD_START_BLOCK_SINGLE_BLOCK_READ, (uint8_t *) r->bufs [0].buffer, (int) e->block_size);
		CLEANUP_RV( rv, sd_card_disk_block_read_cleanup, "Read: SD_CARD_CMD_READ_SINGLE_BLOCK");
	} else {
		/* Start multiple block read */
		rv = sd_card_send_command( e, SD_CARD_CMD_READ_MULTIPLE_BLOCK, start_address);
		CLEANUP_RV( rv, sd_card_disk_block_read_stop_cleanup, "Send: SD_CARD_CMD_READ_MULTIPLE_BLOCK");

		/* Multiple block read */
		for (i = 0; i < r->bufnum; ++i) {
#ifdef DEBUG
			/* Check buffer */
			if (r->bufs [i].length != e->block_size) {
				DO_CLEANUP_RV( -RTEMS_INTERNAL_ERROR, rv, sd_card_disk_block_read_stop_cleanup, "Buffer and disk block size are not equal");
			}
			DEBUG_PRINT( "[%02u]: buffer = 0x%08x, size = %u\n", i, r->bufs [i].buffer, r->bufs [i].length);
#endif /* DEBUG */

			rv = sd_card_read( e, SD_CARD_START_BLOCK_MULTIPLE_BLOCK_READ, (uint8_t *) r->bufs [i].buffer, (int) e->block_size);
			CLEANUP_RV( rv, sd_card_disk_block_read_stop_cleanup, "Read block");
		}

		/* Stop multiple block read */
		rv = sd_card_stop_multiple_block_read( e);
		CLEANUP_RV( rv, sd_card_disk_block_read_cleanup, "Stop multiple block read");
	}

	/* Stop */
	sc = sd_card_stop( e);
	CHECK_SCRV( sc, "Stop");

	/* Done */
	r->req_done( r->done_arg, RTEMS_SUCCESSFUL, 0);

	return 0;

sd_card_disk_block_read_stop_cleanup:

	/* Stop multiple block read */
	sd_card_stop_multiple_block_read( e);

sd_card_disk_block_read_cleanup:

	/* Stop */
	sd_card_stop( e);

	/* Done */
	r->req_done( r->done_arg, RTEMS_IO_ERROR, 0);

	return rv;
}

static int sd_card_disk_block_write( sd_card_driver_entry *e, rtems_blkdev_request *r)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	int rv = 0;
	uint32_t start_address = RTEMS_BLKDEV_START_BLOCK (r) << e->block_size_shift;
	uint32_t i = 0;

	DEBUG_PRINT( "start = %u, count = %u, bufnum = %u\n", r->start, r->count, r->bufnum);

#ifdef DEBUG
	/* Check request */
  if (r->bufs[0].block >= e->block_number) {
		SYSLOG_ERROR( "Start block number out of range");
		return -RTEMS_INTERNAL_ERROR;
	} else if (r->bufnum > e->block_number - RTEMS_BLKDEV_START_BLOCK (r)) {
		SYSLOG_ERROR( "Block count out of range");
		return -RTEMS_INTERNAL_ERROR;
	}
#endif /* DEBUG */

	/* Start */
	sc = sd_card_start( e);
	CLEANUP_SCRV( sc, rv, sd_card_disk_block_write_cleanup, "Start");

	if (r->bufnum == 1) {
#ifdef DEBUG
		/* Check buffer */
		if (r->bufs [0].length != e->block_size) {
			DO_CLEANUP_RV( -RTEMS_INTERNAL_ERROR, rv, sd_card_disk_block_write_cleanup, "Buffer and disk block size are not equal");
		}
		DEBUG_PRINT( "[%02u]: buffer = 0x%08x, size = %u\n", 0, r->bufs [0].buffer, r->bufs [0].length);
#endif /* DEBUG */

		/* Single block write */
		rv = sd_card_send_command( e, SD_CARD_CMD_WRITE_BLOCK, start_address);
		CLEANUP_RV( rv, sd_card_disk_block_write_cleanup, "Send: SD_CARD_CMD_WRITE_BLOCK");
		rv = sd_card_write( e, SD_CARD_START_BLOCK_SINGLE_BLOCK_WRITE, (uint8_t *) r->bufs [0].buffer, (int) e->block_size);
		CLEANUP_RV( rv, sd_card_disk_block_write_cleanup, "Write: SD_CARD_CMD_WRITE_BLOCK");
	} else {
		/* Start multiple block write */
		rv = sd_card_send_command( e, SD_CARD_CMD_WRITE_MULTIPLE_BLOCK, start_address);
		CLEANUP_RV( rv, sd_card_disk_block_write_stop_cleanup, "Send: SD_CARD_CMD_WRITE_MULTIPLE_BLOCK");

		/* Multiple block write */
		for (i = 0; i < r->bufnum; ++i) {
#ifdef DEBUG
			/* Check buffer */
			if (r->bufs [i].length != e->block_size) {
				DO_CLEANUP_RV( -RTEMS_INTERNAL_ERROR, rv, sd_card_disk_block_write_stop_cleanup, "Buffer and disk block size are not equal");
			}
			DEBUG_PRINT( "[%02u]: buffer = 0x%08x, size = %u\n", i, r->bufs [i].buffer, r->bufs [i].length);
#endif /* DEBUG */

			rv = sd_card_write( e, SD_CARD_START_BLOCK_MULTIPLE_BLOCK_WRITE, (uint8_t *) r->bufs [i].buffer, (int) e->block_size);
			CLEANUP_RV( rv, sd_card_disk_block_write_stop_cleanup, "Write block");
		}

		/* Stop multiple block write */
		rv = sd_card_stop_multiple_block_write( e);
		CLEANUP_RV( rv, sd_card_disk_block_write_cleanup, "Stop multiple block write");
	}

	/* Get card status */
	rv = sd_card_send_command( e, SD_CARD_CMD_SEND_STATUS, 0);
	CHECK_RV( rv, "Send: SD_CARD_CMD_SEND_STATUS");

	/* Stop */
	sc = sd_card_stop( e);
	CHECK_SCRV( sc, "Stop");

	/* Done */
	r->req_done( r->done_arg, RTEMS_SUCCESSFUL, 0);

	return 0;

sd_card_disk_block_write_stop_cleanup:

	/* Stop multiple block write */
	sd_card_stop_multiple_block_write( e);

sd_card_disk_block_write_cleanup:

	/* Get card status */
	rv = sd_card_send_command( e, SD_CARD_CMD_SEND_STATUS, 0);
	CHECK_RV( rv, "Send: SD_CARD_CMD_SEND_STATUS");

	/* Stop */
	sd_card_stop( e);

	/* Done */
	r->req_done( r->done_arg, RTEMS_IO_ERROR, 0);

	return rv;
}

static int sd_card_disk_ioctl( dev_t dev, uint32_t req, void *arg)
{
	DEBUG_PRINT( "dev = %u, req = %u, arg = 0x08%x\n", dev, req, arg);
	if (req == RTEMS_BLKIO_REQUEST) {
		rtems_device_minor_number minor = rtems_filesystem_dev_minor_t( dev);
		sd_card_driver_entry *e = &sd_card_driver_table [minor];
		rtems_blkdev_request *r = (rtems_blkdev_request *) arg;
		switch (r->req) {
			case RTEMS_BLKDEV_REQ_READ:
				return sd_card_disk_block_read( e, r);
			case RTEMS_BLKDEV_REQ_WRITE:
				return sd_card_disk_block_write( e, r);
			default:
				errno = EBADRQC;
				return -1;
		}
	} else if (req == RTEMS_BLKDEV_CAPABILITIES) {
		*(uint32_t *) arg = RTEMS_BLKDEV_CAP_MULTISECTOR_CONT;
		return 0;
	} else {
		errno = EBADRQC;
		return -1;
	}
}

static rtems_status_code sd_card_disk_init( rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	/* Do nothing */

	return RTEMS_SUCCESSFUL;
}

/** @} */

static const rtems_driver_address_table sd_card_disk_ops = {
	.initialization_entry = sd_card_disk_init,
	.open_entry = NULL,
	.close_entry = NULL,
	.read_entry = NULL,
	.write_entry = NULL,
	.control_entry = NULL
};

static rtems_device_major_number sd_card_disk_major = 0;

static int sd_card_driver_first = 1;

/**
 * @name LibI2C Driver Functions
 * @{
 */

static inline int sd_card_driver_get_entry( rtems_device_minor_number minor, sd_card_driver_entry **e)
{
	return rtems_libi2c_ioctl( minor, RTEMS_LIBI2C_IOCTL_GET_DRV_T, e);
}

static rtems_status_code sd_card_driver_init( rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	int rv = 0;
	sd_card_driver_entry *e = NULL;
	uint8_t block [SD_CARD_BLOCK_SIZE_DEFAULT];
	uint32_t transfer_speed = 0;
	uint32_t read_block_size = 0;
	uint32_t write_block_size = 0;
	dev_t dev = 0;

	/* Get driver entry */
	rv = sd_card_driver_get_entry( minor, &e);
	CHECK_RVSC( rv, "Get driver entry");

	/* Start */
	sc = sd_card_start( e);
	CLEANUP_SC( sc, sd_card_driver_init_cleanup, "Start");

	/* Save minor number for disk operations */
	e->minor = minor;

	/* Register disk driver */
	if (sd_card_driver_first) {
		sd_card_driver_first = 0;
		sc = rtems_io_register_driver( 0, &sd_card_disk_ops, &sd_card_disk_major);
		CLEANUP_SC( sc, sd_card_driver_init_cleanup, "Register disk IO driver");
	}

	/* Wait until card is not busy */
	rv = sd_card_wait( e);
	CLEANUP_RVSC( rv, sc, sd_card_driver_init_cleanup, "Wait");

	/* Send idle tokens for at least 74 clock cycles with active chip select */
	memset( block, SD_CARD_IDLE_TOKEN, SD_CARD_BLOCK_SIZE_DEFAULT);
	rv = rtems_libi2c_write_bytes( e->minor, block, SD_CARD_BLOCK_SIZE_DEFAULT);
	CLEANUP_RVSC( rv, sc, sd_card_driver_init_cleanup, "Active chip select delay");

	/* Stop */
	sc = sd_card_stop( e);
	CHECK_SC( sc, "Stop");

	/* Start with inactive chip select */
	sc = rtems_libi2c_send_start( e->minor);
	CHECK_SC( sc, "Send start");

	/* Set transfer mode */
	rv = rtems_libi2c_ioctl( e->minor, RTEMS_LIBI2C_IOCTL_SET_TFRMODE, &e->transfer_mode);
	CLEANUP_RVSC( rv, sc, sd_card_driver_init_cleanup, "Set transfer mode");

	/* Send idle tokens with inactive chip select */
	rv = sd_card_query( e, e->response, SD_CARD_COMMAND_SIZE);
	CLEANUP_RVSC( rv, sc, sd_card_driver_init_cleanup, "Inactive chip select delay");

	/* Activate chip select */
	sc = rtems_libi2c_send_addr( e->minor, 1);
	CLEANUP_SC( sc, sd_card_driver_init_cleanup, "Send address");

	/* Stop multiple block write */
	sd_card_stop_multiple_block_write( e);

	/* Get card status */
	sd_card_send_command( e, SD_CARD_CMD_SEND_STATUS, 0);

	/* Stop multiple block read */
	sd_card_stop_multiple_block_read( e);

	/* Switch to SPI mode */
	rv = sd_card_send_command( e, SD_CARD_CMD_GO_IDLE_STATE, 0);
	CLEANUP_RVSC( rv, sc, sd_card_driver_init_cleanup, "Send: SD_CARD_CMD_GO_IDLE_STATE");

	/* Initialize card */
	while (1) {
		rv = sd_card_send_command( e, SD_CARD_CMD_APP_CMD, 0);
		CLEANUP_RVSC( rv, sc, sd_card_driver_init_cleanup, "Send: SD_CARD_CMD_APP_CMD");
		rv = sd_card_send_command( e, SD_CARD_ACMD_SD_SEND_OP_COND, 0);
		CLEANUP_RVSC( rv, sc, sd_card_driver_init_cleanup, "Send: SD_CARD_ACMD_SD_SEND_OP_COND");
	
		/* Not idle? */
		if (SD_CARD_IS_NOT_IDLE_RESPONSE( e->response [e->response_index])) {
			break;
		}

		/* Invoke the scheduler */
		rtems_task_wake_after( RTEMS_YIELD_PROCESSOR);
	};

	/* Card Identification */
	if (e->verbose) {
		rv = sd_card_send_command( e, SD_CARD_CMD_SEND_CID, 0);
		CLEANUP_RVSC( rv, sc, sd_card_driver_init_cleanup, "Send: SD_CARD_CMD_SEND_CID");
		rv = sd_card_read( e, SD_CARD_START_BLOCK_SINGLE_BLOCK_READ, block, SD_CARD_CID_SIZE);
		CLEANUP_RVSC( rv, sc, sd_card_driver_init_cleanup, "Read: SD_CARD_CMD_SEND_CID");
		SYSLOG( "*** Card Identification ***\n");
		SYSLOG( "Manufacturer ID          : %" PRIu8 "\n", SD_CARD_CID_GET_MID( block));
		SYSLOG( "OEM/Application ID       : %" PRIu16 "\n", SD_CARD_CID_GET_OID( block));
		SYSLOG(
			"Product name             : %c%c%c%c%c%c\n",
			SD_CARD_CID_GET_PNM( block, 0),
			SD_CARD_CID_GET_PNM( block, 1),
			SD_CARD_CID_GET_PNM( block, 2),
			SD_CARD_CID_GET_PNM( block, 3),
			SD_CARD_CID_GET_PNM( block, 4),
			SD_CARD_CID_GET_PNM( block, 5)
		);
		SYSLOG( "Product revision         : %" PRIu8 "\n", SD_CARD_CID_GET_PRV( block));
		SYSLOG( "Product serial number    : %" PRIu32 "\n", SD_CARD_CID_GET_PSN( block));
		SYSLOG( "Manufacturing date       : %" PRIu8 "\n", SD_CARD_CID_GET_MDT( block));
		SYSLOG( "7-bit CRC checksum       : %" PRIu8 "\n",  SD_CARD_CID_GET_CRC7( block));
	}

	/* Card Specific Data */
	rv = sd_card_send_command( e, SD_CARD_CMD_SEND_CSD, 0);
	CLEANUP_RVSC( rv, sc, sd_card_driver_init_cleanup, "Send: SD_CARD_CMD_SEND_CSD");
	rv = sd_card_read( e, SD_CARD_START_BLOCK_SINGLE_BLOCK_READ, block, SD_CARD_CSD_SIZE);
	CLEANUP_RVSC( rv, sc, sd_card_driver_init_cleanup, "Read: SD_CARD_CMD_SEND_CSD");
	transfer_speed = sd_card_transfer_speed( block);
	e->transfer_mode.baudrate = transfer_speed;
	e->n_ac_max = sd_card_max_access_time( block, transfer_speed);
	read_block_size = 1U << SD_CARD_CSD_GET_READ_BLK_LEN( block);
	e->block_size_shift = SD_CARD_CSD_GET_READ_BLK_LEN( block);
	write_block_size = 1U << e->block_size_shift;
	if (read_block_size < write_block_size) {
		SYSLOG_ERROR( "Read block size smaller than write block size\n");
		return -RTEMS_IO_ERROR;
	}
	e->block_size = write_block_size;
	e->block_number = sd_card_block_number( block);
	if (e->verbose) {
		SYSLOG( "*** Card Specific Data ***\n");
		SYSLOG( "CSD structure            : %" PRIu8 "\n", SD_CARD_CSD_GET_CSD_STRUCTURE( block));
		SYSLOG( "Spec version             : %" PRIu8 "\n", SD_CARD_CSD_GET_SPEC_VERS( block));
		SYSLOG( "Access time [ns]         : %" PRIu32 "\n", sd_card_access_time( block));
		SYSLOG( "Max access time [N]      : %" PRIu32 "\n", e->n_ac_max);
		SYSLOG( "Max read block size [B]  : %" PRIu32 "\n", read_block_size);
		SYSLOG( "Max write block size [B] : %" PRIu32 "\n", write_block_size);
		SYSLOG( "Block size [B]           : %" PRIu32 "\n", e->block_size);
		SYSLOG( "Block number             : %" PRIu32 "\n", e->block_number);
		SYSLOG( "Capacity [B]             : %" PRIu32 "\n", sd_card_capacity( block));
		SYSLOG( "Max transfer speed [b/s] : %" PRIu32 "\n", transfer_speed);
	}

	/* Set read block size */
	rv = sd_card_send_command( e, SD_CARD_CMD_SET_BLOCKLEN, e->block_size);
	CLEANUP_RVSC( rv, sc, sd_card_driver_init_cleanup, "Send: SD_CARD_CMD_SET_BLOCKLEN");

	/* Create disk device */
	dev = rtems_filesystem_make_dev_t( sd_card_disk_major, (rtems_device_minor_number) e->table_index);
	sc = rtems_disk_io_initialize();
	CLEANUP_SC( sc, sd_card_driver_init_cleanup, "Initialize RTEMS disk IO");
	sc = rtems_disk_create_phys( dev, (int) e->block_size, (int) e->block_number, sd_card_disk_ioctl, e->disk_device_name);
	CLEANUP_SC( sc, sd_card_driver_init_cleanup, "Create disk device");

	/* Stop */
	sc = sd_card_stop( e);
	CHECK_SC( sc, "Stop");

	return RTEMS_SUCCESSFUL;

sd_card_driver_init_cleanup:

	/* Stop */
	sd_card_stop( e);

	return sc;
}

static rtems_status_code sd_card_driver_read( rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	int rv = 0;
	rtems_libio_rw_args_t *rw = (rtems_libio_rw_args_t *) arg;
	sd_card_driver_entry *e = NULL;
	uint32_t block_size_mask = 0;
	uint32_t block_count = 0;
	uint32_t start_block = 0;
	uint32_t i = 0;

	/* Clear moved bytes counter */
	rw->bytes_moved = 0;

	/* Get driver entry */
	rv = sd_card_driver_get_entry( minor, &e);
	CHECK_RVSC( rv, "Get driver entry");

	/* Start */
	sc = sd_card_start( e);
	CLEANUP_SC( sc, sd_card_driver_read_cleanup, "Start");

	/* Check arguments */
	block_size_mask = e->block_size - 1;
	block_count = (uint32_t) rw->count >> e->block_size_shift;
	start_block = (uint32_t) rw->offset >> e->block_size_shift;
	if (rw->offset & block_size_mask) {
		DO_CLEANUP_SC( RTEMS_INVALID_ADDRESS, sc, sd_card_driver_read_cleanup, "Invalid offset");
	} else if ((rw->count & block_size_mask) || (start_block >= e->block_number) || (block_count > e->block_number - start_block)) {
		DO_CLEANUP_SC( RTEMS_INVALID_NUMBER, sc, sd_card_driver_read_cleanup, "Invalid count or out of range");
	}

	if (block_count == 0) {
		/* Do nothing */
	} else if (block_count == 1) {
		/* Single block read */
		rv = sd_card_send_command( e, SD_CARD_CMD_READ_SINGLE_BLOCK, (uint32_t) rw->offset);
		CLEANUP_RVSC( rv, sc, sd_card_driver_read_cleanup, "Send: SD_CARD_CMD_READ_SINGLE_BLOCK");
		rv = sd_card_read( e, SD_CARD_START_BLOCK_SINGLE_BLOCK_READ, (uint8_t *) rw->buffer, (int) e->block_size);
		CLEANUP_RVSC( rv, sc, sd_card_driver_read_cleanup, "Read: SD_CARD_CMD_READ_SINGLE_BLOCK");

		/* Set moved bytes counter */
		rw->bytes_moved = (uint32_t) rv;
	} else {
		/* Start multiple block read */
		rv = sd_card_send_command( e, SD_CARD_CMD_READ_MULTIPLE_BLOCK, (uint32_t) rw->offset);
		CLEANUP_RVSC( rv, sc, sd_card_driver_read_stop_cleanup, "Send: SD_CARD_CMD_READ_MULTIPLE_BLOCK");

		/* Multiple block read */
		for (i = 0; i < block_count; ++i) {
			rv = sd_card_read( e, SD_CARD_START_BLOCK_MULTIPLE_BLOCK_READ, (uint8_t *) rw->buffer + (i << e->block_size_shift), (int) e->block_size);
			CLEANUP_RVSC( rv, sc, sd_card_driver_read_stop_cleanup, "Read block");

			/* Update moved bytes counter */
			rw->bytes_moved += (uint32_t) rv;
		}

		/* Stop multiple block read */
		rv = sd_card_stop_multiple_block_read( e);
		CLEANUP_RVSC( rv, sc, sd_card_driver_read_cleanup, "Stop multiple block read");
	}

	/* Stop */
	sc = sd_card_stop( e);
	CHECK_SC( sc, "Stop");

	return RTEMS_SUCCESSFUL;

sd_card_driver_read_stop_cleanup:

	/* Stop multiple block read */
	sd_card_stop_multiple_block_read( e);

sd_card_driver_read_cleanup:

	/* Stop */
	sd_card_stop( e);

	return sc;
}

static rtems_status_code sd_card_driver_write( rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	int rv = 0;
	rtems_libio_rw_args_t *rw = (rtems_libio_rw_args_t *) arg;
	sd_card_driver_entry *e = NULL;
	uint32_t block_size_mask = 0;
	uint32_t block_count = 0;
	uint32_t start_block = 0;
	uint32_t i = 0;

	/* Clear moved bytes counter */
	rw->bytes_moved = 0;

	/* Get driver entry */
	rv = sd_card_driver_get_entry( minor, &e);
	CHECK_RVSC( rv, "Get driver entry");

	/* Start */
	sc = sd_card_start( e);
	CLEANUP_SC( sc, sd_card_driver_write_cleanup, "Start");

	/* Check arguments */
	block_size_mask = e->block_size - 1;
	block_count = (uint32_t) rw->count >> e->block_size_shift;
	start_block = (uint32_t) rw->offset >> e->block_size_shift;
	if (rw->offset & block_size_mask) {
		DO_CLEANUP_SC( RTEMS_INVALID_ADDRESS, sc, sd_card_driver_write_cleanup, "Invalid offset");
	} else if ((rw->count & block_size_mask) || (start_block >= e->block_number) || (block_count > e->block_number - start_block)) {
		DO_CLEANUP_SC( RTEMS_INVALID_NUMBER, sc, sd_card_driver_write_cleanup, "Invalid count or out of range");
	}

	if (block_count == 0) {
		/* Do nothing */
	} else if (block_count == 1) {
		/* Single block write */
		rv = sd_card_send_command( e, SD_CARD_CMD_WRITE_BLOCK, (uint32_t) rw->offset);
		CLEANUP_RVSC( rv, sc, sd_card_driver_write_cleanup, "Send: SD_CARD_CMD_WRITE_BLOCK");
		rv = sd_card_write( e, SD_CARD_START_BLOCK_SINGLE_BLOCK_WRITE, (uint8_t *) rw->buffer, (int) e->block_size);
		CLEANUP_RVSC( rv, sc, sd_card_driver_write_cleanup, "Write: SD_CARD_CMD_WRITE_BLOCK");

		/* Set moved bytes counter */
		rw->bytes_moved = (uint32_t) rv;
	} else {
		/* Start multiple block write */
		rv = sd_card_send_command( e, SD_CARD_CMD_WRITE_MULTIPLE_BLOCK, (uint32_t) rw->offset);
		CLEANUP_RVSC( rv, sc, sd_card_driver_write_stop_cleanup, "Send: SD_CARD_CMD_WRITE_MULTIPLE_BLOCK");

		/* Multiple block write */
		for (i = 0; i < block_count; ++i) {
			rv = sd_card_write( e, SD_CARD_START_BLOCK_MULTIPLE_BLOCK_WRITE, (uint8_t *) rw->buffer + (i << e->block_size_shift), (int) e->block_size);
			CLEANUP_RVSC( rv, sc, sd_card_driver_write_stop_cleanup, "Write: SD_CARD_CMD_WRITE_MULTIPLE_BLOCK");

			/* Update moved bytes counter */
			rw->bytes_moved += (uint32_t) rv;
		}

		/* Stop multiple block write */
		rv = sd_card_stop_multiple_block_write( e);
		CLEANUP_RVSC( rv, sc, sd_card_driver_write_cleanup, "Stop multiple block write");
	}

	/* Get card status */
	rv = sd_card_send_command( e, SD_CARD_CMD_SEND_STATUS, 0);
	CHECK_RV( rv, "Send: SD_CARD_CMD_SEND_STATUS");

	/* Stop */
	sc = sd_card_stop( e);
	CHECK_SC( sc, "Stop");

	return RTEMS_SUCCESSFUL;

sd_card_driver_write_stop_cleanup:

	/* Stop multiple block write */
	sd_card_stop_multiple_block_write( e);

sd_card_driver_write_cleanup:

	/* Get card status */
	rv = sd_card_send_command( e, SD_CARD_CMD_SEND_STATUS, 0);
	CHECK_RV( rv, "Send: SD_CARD_CMD_SEND_STATUS");

	/* Stop */
	sd_card_stop( e);

	return sc;
}

/** @} */

const rtems_driver_address_table sd_card_driver_ops = {
	.initialization_entry = sd_card_driver_init,
	.open_entry = NULL,
	.close_entry = NULL,
	.read_entry = sd_card_driver_read,
	.write_entry = sd_card_driver_write,
	.control_entry = NULL
};
