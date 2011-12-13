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
#include <rtems/blkdev.h>

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
#define SD_CARD_CMD_SEND_IF_COND 8
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
 * @name Command Flags
 * @{
 */

#define SD_CARD_FLAG_HCS 0x40000000U

#define SD_CARD_FLAG_VHS_2_7_TO_3_3 0x00000100U

#define SD_CARD_FLAG_CHECK_PATTERN 0x000000aaU

/** @} */

/**
 * @name Command Fields
 * @{
 */

#define SD_CARD_COMMAND_SET_COMMAND( c, cmd) (c) [1] = (uint8_t) (0x40 + ((cmd) & 0x3f))

#define SD_CARD_COMMAND_SET_ARGUMENT( c, arg) sd_card_put_uint32( (arg), &((c) [2]))

#define SD_CARD_COMMAND_SET_CRC7( c, crc7) ((c) [6] = ((crc7) << 1) | 1U)

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
#define SD_CARD_CSD_GET_C_SIZE( csd) ((((uint32_t) (csd) [6] & 0x3) << 10) + (((uint32_t) (csd) [7]) << 2) + ((((uint32_t) (csd) [8]) >> 6) & 0x3))
#define SD_CARD_CSD_GET_C_SIZE_MULT( csd) ((((csd) [9] & 0x3) << 1) + (((csd) [10] >> 7) & 0x1))
#define SD_CARD_CSD_GET_READ_BLK_LEN( csd) ((uint32_t) (csd) [5] & 0xf)
#define SD_CARD_CSD_GET_WRITE_BLK_LEN( csd) ((((uint32_t) (csd) [12] & 0x3) << 2) + ((((uint32_t) (csd) [13]) >> 6) & 0x3))
#define SD_CARD_CSD_1_GET_C_SIZE( csd) ((((uint32_t) (csd) [7] & 0x3f) << 16) + (((uint32_t) (csd) [8]) << 8) + (uint32_t) (csd) [9])

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
	uint32_t ac_100ms = transfer_speed / 80;
	uint32_t n = SD_CARD_CSD_GET_NSAC( csd) * 100;
	/* ac is in ns, transfer_speed in bps, max_access_time in bytes.
	   max_access_time is 100 times typical access time (taac+nsac) */
	ac = ac * transfer_speed / 80000000;
	ac = ac + 100*n;
	if ((uint32_t)ac > ac_100ms)
		return ac_100ms;
	else
		return (uint32_t)ac;
}

/** @} */

/**
 * @name CRC functions
 *
 * Based on http://en.wikipedia.org/wiki/Computation_of_CRC
 *
 * @{
 */

static uint8_t sd_card_compute_crc7 (uint8_t *data, size_t len)
{
	uint8_t e, f, crc;
	size_t i;

	crc = 0;
	for (i = 0; i < len; i++) {
		e   = crc ^ data[i];
		f   = e ^ (e >> 4) ^ (e >> 7);
		crc = (f << 1) ^ (f << 4);
	}
	return crc >> 1;
}

static uint16_t sd_card_compute_crc16 (uint8_t *data, size_t len)
{
	uint8_t s, t;
	uint16_t crc;
	size_t i;

	crc = 0;
	for (i = 0; i < len; i++) {
		s = data[i] ^ (crc >> 8);
		t = s ^ (s >> 4);
		crc = (crc << 8) ^ t ^ (t << 5) ^ (t << 12);
	}
	return crc;
}

/** @} */

/**
 * @name Communication Functions
 * @{
 */

static inline int sd_card_query( sd_card_driver_entry *e, uint8_t *in, int n)
{
	return rtems_libi2c_read_bytes( e->bus, in, n);
}

static int sd_card_wait( sd_card_driver_entry *e)
{
	int rv = 0;
	int r = 0;
	int n = 2;
	/* For writes, the timeout is 2.5 times that of reads; since we
	   don't know if it is a write or read, assume write.
	   FIXME should actually look at R2W_FACTOR for non-HC cards. */
	int retries = e->n_ac_max * 25 / 10;
	/* n_ac_max/100 is supposed to be the average waiting time. To
	   approximate this, we start with waiting n_ac_max/150 and
	   gradually increase the waiting time. */
	int wait_time_bytes = (retries + 149) / 150;
	while (e->busy) {
		/* Query busy tokens */
		rv = sd_card_query( e, e->response, n);
		RTEMS_CHECK_RV( rv, "Busy");

		/* Search for non busy tokens */
		for (r = 0; r < n; ++r) {
			if (e->response [r] != SD_CARD_BUSY_TOKEN) {
				e->busy = false;
				return 0;
			}
		}
		retries -= n;
		if (retries <= 0) {
			return -RTEMS_TIMEOUT;
		}

		if (e->schedule_if_busy) {
			uint64_t wait_time_us = wait_time_bytes;
			wait_time_us *= 8000000;
			wait_time_us /= e->transfer_mode.baudrate;
			rtems_task_wake_after( RTEMS_MICROSECONDS_TO_TICKS(wait_time_us));
			retries -= wait_time_bytes;
			wait_time_bytes = wait_time_bytes * 15 / 10;
		} else {
			n = SD_CARD_COMMAND_SIZE;
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
	uint8_t crc7;

	SD_CARD_INVALIDATE_RESPONSE_INDEX( e);

	/* Wait until card is not busy */
	rv = sd_card_wait( e);
	RTEMS_CHECK_RV( rv, "Wait");

	/* Write command and read response */
	SD_CARD_COMMAND_SET_COMMAND( e->command, command);
	SD_CARD_COMMAND_SET_ARGUMENT( e->command, argument);
	crc7 = sd_card_compute_crc7( e->command + 1, 5);
	SD_CARD_COMMAND_SET_CRC7( e->command, crc7);
	rv = rtems_libi2c_ioctl( e->bus, RTEMS_LIBI2C_IOCTL_READ_WRITE, &rw);
	RTEMS_CHECK_RV( rv, "Write command and read response");

	/* Check respose */
	for (r = SD_CARD_COMMAND_RESPONSE_START; r < SD_CARD_COMMAND_SIZE; ++r) {
		RTEMS_DEBUG_PRINT( "Token [%02u]: 0x%02x\n", r, e->response [r]);
		e->response_index = r;
		if (SD_CARD_IS_RESPONSE( e->response [r])) {
			if (SD_CARD_IS_ERRORLESS_RESPONSE( e->response [r])) {
				return 0;
			} else {
				RTEMS_SYSLOG_ERROR( "Command error [%02i]: 0x%02" PRIx8 "\n", r, e->response [r]);
				goto sd_card_send_command_error;
			}
		} else if (e->response [r] != SD_CARD_IDLE_TOKEN) {
			RTEMS_SYSLOG_ERROR( "Unexpected token [%02i]: 0x%02" PRIx8 "\n", r, e->response [r]);
			goto sd_card_send_command_error;
		}
	}

	RTEMS_SYSLOG_ERROR( "Timeout\n");

sd_card_send_command_error:

	RTEMS_SYSLOG_ERROR( "Response:");
	for (r = 0; r < SD_CARD_COMMAND_SIZE; ++r) {
		if (e->response_index == r) {
			RTEMS_SYSLOG_PRINT( " %02" PRIx8 ":[%02" PRIx8 "]", e->command [r], e->response [r]);
		} else {
			RTEMS_SYSLOG_PRINT( " %02" PRIx8 ":%02" PRIx8 "", e->command [r], e->response [r]);
		}
	}
	RTEMS_SYSLOG_PRINT( "\n");

	return -RTEMS_IO_ERROR;
}

static int sd_card_send_register_command( sd_card_driver_entry *e, uint32_t command, uint32_t argument, uint32_t *reg)
{
	int rv = 0;
	uint8_t crc7;

	rv = sd_card_send_command( e, command, argument);
	RTEMS_CHECK_RV( rv, "Send command");

	if (e->response_index + 5 > SD_CARD_COMMAND_SIZE) {
		/*
		 * TODO: If this happens in the wild we need to implement a
		 * more sophisticated response query.
		 */
		RTEMS_SYSLOG_ERROR( "Unexpected response position\n");
		return -RTEMS_IO_ERROR;
	}

	crc7 = sd_card_compute_crc7( e->response + e->response_index, 5);
	if (crc7 != SD_CARD_COMMAND_GET_CRC7( e->response + e->response_index) &&
		SD_CARD_COMMAND_GET_CRC7( e->response + e->response_index) != 0x7f) {
		RTEMS_SYSLOG_ERROR( "CRC check failed on register command\n");
		return -RTEMS_IO_ERROR;
	}

	*reg = sd_card_get_uint32( e->response + e->response_index + 1);

	return 0;
}

static int sd_card_stop_multiple_block_read( sd_card_driver_entry *e)
{
	int rv = 0;

	SD_CARD_COMMAND_SET_COMMAND( e->command, SD_CARD_CMD_STOP_TRANSMISSION);
	rv = rtems_libi2c_write_bytes( e->bus, e->command, SD_CARD_COMMAND_SIZE);
	RTEMS_CHECK_RV( rv, "Write stop transfer token");

	return 0;
}

static int sd_card_stop_multiple_block_write( sd_card_driver_entry *e)
{
	int rv = 0;
	uint8_t stop_transfer [3] = { SD_CARD_IDLE_TOKEN, SD_CARD_STOP_TRANSFER_MULTIPLE_BLOCK_WRITE, SD_CARD_IDLE_TOKEN };

	/* Wait until card is not busy */
	rv = sd_card_wait( e);
	RTEMS_CHECK_RV( rv, "Wait");

	/* Send stop token */
	rv = rtems_libi2c_write_bytes( e->bus, stop_transfer, 3);
	RTEMS_CHECK_RV( rv, "Write stop transfer token");

	/* Card is now busy */
	e->busy = true;

	return 0;
}

static int sd_card_read( sd_card_driver_entry *e, uint8_t start_token, uint8_t *in, int n)
{
	int rv = 0;

	/* Discard command response */
	int r = e->response_index + 1;

	/* Standard response size */
	int response_size = SD_CARD_COMMAND_SIZE;

	/* Where the response is stored */
	uint8_t *response = e->response;

	/* Data input index */
	int i = 0;

	/* CRC check of data */
	uint16_t crc16;

	/* Maximum number of tokens to read. */
	int retries = e->n_ac_max;

	SD_CARD_INVALIDATE_RESPONSE_INDEX( e);

	while (true) {
		RTEMS_DEBUG_PRINT( "Search from %u to %u\n", r, response_size - 1);

		/* Search the data start token in in current response buffer */
		retries -= (response_size - r);
		while (r < response_size) {
			RTEMS_DEBUG_PRINT( "Token [%02u]: 0x%02x\n", r, response [r]);
			if (response [r] == start_token) {
				/* Discard data start token */
				++r;
				goto sd_card_read_start;
			} else if (SD_CARD_IS_DATA_ERROR( response [r])) {
				RTEMS_SYSLOG_ERROR( "Data error token [%02i]: 0x%02" PRIx8 "\n", r, response [r]);
				return -RTEMS_IO_ERROR;
			} else if (response [r] != SD_CARD_IDLE_TOKEN) {
				RTEMS_SYSLOG_ERROR( "Unexpected token [%02i]: 0x%02" PRIx8 "\n", r, response [r]);
				return -RTEMS_IO_ERROR;
			}
			++r;
		}

		if (retries <= 0) {
			RTEMS_SYSLOG_ERROR( "Timeout\n");
			return -RTEMS_IO_ERROR;
		}

		if (e->schedule_if_busy)
			rtems_task_wake_after( RTEMS_YIELD_PROCESSOR);

		/* Query more.  We typically have to wait between 10 and 100
		   bytes.  To reduce overhead, read the response in chunks of
		   50 bytes - this doesn't introduce too much copy overhead
		   but does allow SPI DMA transfers to work efficiently. */
		response = in;
		response_size = 50;
		if (response_size > n)
			response_size = n;
		rv = sd_card_query( e, response, response_size);
		RTEMS_CHECK_RV( rv, "Query data start token");

		/* Reset start position */
		r = 0;
	}

sd_card_read_start:

	/* Read data */
	while (r < response_size && i < n) {
		in [i++] = response [r++];
	}

	/* Read more data? */
	if (i < n) {
		rv = sd_card_query( e, &in [i], n - i);
		RTEMS_CHECK_RV( rv, "Read data");
		i += rv;
	}

	/* Read CRC 16 and N_RC */
	rv = sd_card_query( e, e->response, 3);
	RTEMS_CHECK_RV( rv, "Read CRC 16");

	crc16 = sd_card_compute_crc16 (in, n);
	if ((e->response[0] != ((crc16 >> 8) & 0xff)) ||
	    (e->response[1] != (crc16 & 0xff))) {
		RTEMS_SYSLOG_ERROR( "CRC check failed on read\n");
		return -RTEMS_IO_ERROR;
	}

	return i;
}

static int sd_card_write( sd_card_driver_entry *e, uint8_t start_token, uint8_t *out, int n)
{
	int rv = 0;
	uint8_t crc16_bytes [2] = { 0, 0 };
	uint16_t crc16;

	/* Data output index */
	int o = 0;

	/* Wait until card is not busy */
	rv = sd_card_wait( e);
	RTEMS_CHECK_RV( rv, "Wait");

	/* Write data start token */
	rv = rtems_libi2c_write_bytes( e->bus, &start_token, 1);
	RTEMS_CHECK_RV( rv, "Write data start token");

	/* Write data */
	o = rtems_libi2c_write_bytes( e->bus, out, n);
	RTEMS_CHECK_RV( o, "Write data");

	/* Write CRC 16 */
	crc16 = sd_card_compute_crc16(out, n);
	crc16_bytes[0] = (crc16>>8) & 0xff;
	crc16_bytes[1] = (crc16) & 0xff;
	rv = rtems_libi2c_write_bytes( e->bus, crc16_bytes, 2);
	RTEMS_CHECK_RV( rv, "Write CRC 16");

	/* Read data response */
	rv = sd_card_query( e, e->response, 2);
	RTEMS_CHECK_RV( rv, "Read data response");
	if (SD_CARD_IS_DATA_REJECTED( e->response [0])) {
		RTEMS_SYSLOG_ERROR( "Data rejected: 0x%02" PRIx8 "\n", e->response [0]);
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

	sc = rtems_libi2c_send_start( e->bus);
	RTEMS_CHECK_SC( sc, "Send start");

	rv = rtems_libi2c_ioctl( e->bus, RTEMS_LIBI2C_IOCTL_SET_TFRMODE, &e->transfer_mode);
	RTEMS_CHECK_RV_SC( rv, "Set transfer mode");

	sc = rtems_libi2c_send_addr( e->bus, 1);
	RTEMS_CHECK_SC( sc, "Send address");

	return RTEMS_SUCCESSFUL;
}

static inline rtems_status_code sd_card_stop( sd_card_driver_entry *e)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	sc = rtems_libi2c_send_stop( e->bus);
	RTEMS_CHECK_SC( sc, "Send stop");

	return RTEMS_SUCCESSFUL;
}

static rtems_status_code sd_card_init( sd_card_driver_entry *e)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	int rv = 0;
	uint8_t block [SD_CARD_BLOCK_SIZE_DEFAULT];
	uint32_t transfer_speed = 0;
	uint32_t read_block_size = 0;
	uint32_t write_block_size = 0;
	uint8_t csd_structure = 0;
	uint64_t capacity = 0;
	uint8_t crc7;

	/* Assume first that we have a SD card and not a MMC card */
	bool assume_sd = true;

	/*
	 * Assume high capacity until proven wrong (applies to SD and not yet
	 * existing MMC).
	 */
	bool high_capacity = true;

	bool do_cmd58 = true;
	uint32_t cmd_arg = 0;
	uint32_t if_cond_test = SD_CARD_FLAG_VHS_2_7_TO_3_3 | SD_CARD_FLAG_CHECK_PATTERN;
	uint32_t if_cond_reg = if_cond_test;

	/* Start */
	sc = sd_card_start( e);
	RTEMS_CLEANUP_SC( sc, sd_card_driver_init_cleanup, "Start");

	/* Wait until card is not busy */
	rv = sd_card_wait( e);
	RTEMS_CLEANUP_RV_SC( rv, sc, sd_card_driver_init_cleanup, "Wait");

	/* Send idle tokens for at least 74 clock cycles with active chip select */
	memset( block, SD_CARD_IDLE_TOKEN, SD_CARD_BLOCK_SIZE_DEFAULT);
	rv = rtems_libi2c_write_bytes( e->bus, block, SD_CARD_BLOCK_SIZE_DEFAULT);
	RTEMS_CLEANUP_RV_SC( rv, sc, sd_card_driver_init_cleanup, "Active chip select delay");

	/* Stop */
	sc = sd_card_stop( e);
	RTEMS_CHECK_SC( sc, "Stop");

	/* Start with inactive chip select */
	sc = rtems_libi2c_send_start( e->bus);
	RTEMS_CHECK_SC( sc, "Send start");

	/* Set transfer mode */
	rv = rtems_libi2c_ioctl( e->bus, RTEMS_LIBI2C_IOCTL_SET_TFRMODE, &e->transfer_mode);
	RTEMS_CLEANUP_RV_SC( rv, sc, sd_card_driver_init_cleanup, "Set transfer mode");

	/* Send idle tokens with inactive chip select */
	rv = sd_card_query( e, e->response, SD_CARD_COMMAND_SIZE);
	RTEMS_CLEANUP_RV_SC( rv, sc, sd_card_driver_init_cleanup, "Inactive chip select delay");

	/* Activate chip select */
	sc = rtems_libi2c_send_addr( e->bus, 1);
	RTEMS_CLEANUP_SC( sc, sd_card_driver_init_cleanup, "Send address");

	/* Stop multiple block write */
	sd_card_stop_multiple_block_write( e);

	/* Get card status */
	sd_card_send_command( e, SD_CARD_CMD_SEND_STATUS, 0);

	/* Stop multiple block read */
	sd_card_stop_multiple_block_read( e);

	/* Switch to SPI mode */
	rv = sd_card_send_command( e, SD_CARD_CMD_GO_IDLE_STATE, 0);
	RTEMS_CLEANUP_RV_SC( rv, sc, sd_card_driver_init_cleanup, "Send: SD_CARD_CMD_GO_IDLE_STATE");

	/*
	 * Get interface condition, CMD8.  This is new for SD 2.x and enables
	 * getting the High Capacity Support flag HCS and checks that the
	 * voltage is right.  Some MMCs accept this command but will still fail
	 * on ACMD41.  SD 1.x cards will fails this command and do not support
	 * HCS (> 2G capacity).
	 */
	rv = sd_card_send_register_command( e, SD_CARD_CMD_SEND_IF_COND, if_cond_reg, &if_cond_reg);

	/*
	 * Regardless of whether CMD8 above passes or fails, send ACMD41.  If
	 * card is MMC it will fail.  But older SD < 2.0 (which fail CMD8) will
	 * always stay "idle" if cmd_arg is non-zero, so set to 0 above on
	 * fail.
	 */
	if (rv < 0) {
		/* Failed CMD8, so SD 1.x or MMC */
		cmd_arg = 0;
	} else {
		cmd_arg = SD_CARD_FLAG_HCS;
	}

	/* Enable CRC */
	sd_card_send_command( e, SD_CARD_CMD_CRC_ON_OFF, 1);

	/* Initialize card */
	while (true) {
		if (assume_sd) {
			/* This command (CMD55) supported by SD and (most?) MMCs */
			rv = sd_card_send_command( e, SD_CARD_CMD_APP_CMD, 0);
			if (rv < 0) {
				RTEMS_SYSLOG( "CMD55 failed.  Assume MMC and try CMD1\n");
				assume_sd = false;
				continue;
			}

			/*
			 * This command (ACMD41) only supported by SD.  Always
			 * fails if MMC.
			 */
			rv = sd_card_send_command( e, SD_CARD_ACMD_SD_SEND_OP_COND, cmd_arg);
			if (rv < 0) {
				/*
				 * This *will* fail for MMC.  If fails, bad/no
				 * card or card is MMC, do CMD58 then CMD1.
				 */
				RTEMS_SYSLOG( "ACMD41 failed.  Assume MMC and do CMD58 (once) then CMD1\n");
				assume_sd = false;
				cmd_arg = SD_CARD_FLAG_HCS;
				do_cmd58 = true;
				continue;
			} else {
				/*
				 * Passed ACMD41 so SD.  It is now save to
				 * check if_cond_reg from CMD8.  Reject the
				 * card in case of a indicated bad voltage.
				 */
				if (if_cond_reg != if_cond_test) {
					RTEMS_CLEANUP_RV_SC( -1, sc, sd_card_driver_init_cleanup, "Bad voltage for SD");
				}
			}
		} else {
			/*
			 * Does not seem to be SD card.  Do init for MMC.
			 * First send CMD58 once to enable check for HCS
			 * (similar to CMD8 of SD) with bits 30:29 set to 10b.
			 * This will work for MMC >= 4.2.  Older cards (<= 4.1)
			 * may may not respond to CMD1 unless CMD58 is sent
			 * again with zero argument.
			 */
			if (do_cmd58) {
				rv = sd_card_send_command( e, SD_CARD_CMD_READ_OCR, cmd_arg);
				RTEMS_CLEANUP_RV_SC( rv, sc, sd_card_driver_init_cleanup, "Failed CMD58 for MMC");

				/* A one-shot call */
				do_cmd58 = false;
			}

			/* Do CMD1 */
			rv = sd_card_send_command( e, SD_CARD_CMD_SEND_OP_COND, 0);
			if (rv < 0) {
				if (cmd_arg != 0) {
					/*
					 * Send CMD58 again with zero argument
					 * value.  Proves card is not
					 * high_capacity.
					 */
					cmd_arg = 0;
					do_cmd58 = true;
					high_capacity = false;
					continue;
				}

				RTEMS_CLEANUP_RV_SC( rv, sc, sd_card_driver_init_cleanup, "Failed to initialize MMC");
			}
		}

		/*
		 * Not idle?
		 *
		 * This hangs forever if the card remains not idle and sends
		 * always a valid response.
		 */
		if (SD_CARD_IS_NOT_IDLE_RESPONSE( e->response [e->response_index])) {
			break;
		}

		/* Invoke the scheduler */
		rtems_task_wake_after( RTEMS_YIELD_PROCESSOR);
	}

	/* Now we know if we are SD or MMC */
	if (assume_sd) {
		if (cmd_arg == 0) {
			/* SD is < 2.0 so never high capacity (<= 2G) */
			high_capacity = 0;
		} else {
			uint32_t reg = 0;

			/*
			 * SD is definitely 2.x.  Now need to send CMD58 to get
			 * the OCR to see if the HCS bit is set (capacity > 2G)
			 * or if bit is off (capacity <= 2G, standard
			 * capacity).
			 */
			rv = sd_card_send_register_command( e, SD_CARD_CMD_READ_OCR, 0, &reg);
			RTEMS_CLEANUP_RV_SC( rv, sc, sd_card_driver_init_cleanup, "Failed CMD58 for SD 2.x");

			/* Check HCS bit of OCR */
			high_capacity = (reg & SD_CARD_FLAG_HCS) != 0;
		}
	} else {
		/*
		 * Card is MMC.  Unless already proven to be not HCS (< 4.2)
		 * must do CMD58 again to check the OCR bits 30:29.
		 */
		if (high_capacity) {
			uint32_t reg = 0;

			/*
			 * The argument should still be correct since was never
			 * set to 0
			 */
			rv = sd_card_send_register_command( e, SD_CARD_CMD_READ_OCR, cmd_arg, &reg);
			RTEMS_CLEANUP_RV_SC( rv, sc, sd_card_driver_init_cleanup, "Failed CMD58 for MMC 4.2");

			/* Check HCS bit of the OCR */
			high_capacity = (reg & 0x600000) == SD_CARD_FLAG_HCS;
		}
	}

	/* Card Identification */
	if (e->verbose) {
		rv = sd_card_send_command( e, SD_CARD_CMD_SEND_CID, 0);
		RTEMS_CLEANUP_RV_SC( rv, sc, sd_card_driver_init_cleanup, "Send: SD_CARD_CMD_SEND_CID");
		rv = sd_card_read( e, SD_CARD_START_BLOCK_SINGLE_BLOCK_READ, block, SD_CARD_CID_SIZE);
		RTEMS_CLEANUP_RV_SC( rv, sc, sd_card_driver_init_cleanup, "Read: SD_CARD_CMD_SEND_CID");
		RTEMS_SYSLOG( "*** Card Identification ***\n");
		RTEMS_SYSLOG( "Manufacturer ID          : %" PRIu8 "\n", SD_CARD_CID_GET_MID( block));
		RTEMS_SYSLOG( "OEM/Application ID       : %" PRIu16 "\n", SD_CARD_CID_GET_OID( block));
		RTEMS_SYSLOG(
			"Product name             : %c%c%c%c%c%c\n",
			SD_CARD_CID_GET_PNM( block, 0),
			SD_CARD_CID_GET_PNM( block, 1),
			SD_CARD_CID_GET_PNM( block, 2),
			SD_CARD_CID_GET_PNM( block, 3),
			SD_CARD_CID_GET_PNM( block, 4),
			SD_CARD_CID_GET_PNM( block, 5)
		);
		RTEMS_SYSLOG( "Product revision         : %" PRIu8 "\n", SD_CARD_CID_GET_PRV( block));
		RTEMS_SYSLOG( "Product serial number    : %" PRIu32 "\n", SD_CARD_CID_GET_PSN( block));
		RTEMS_SYSLOG( "Manufacturing date       : %" PRIu8 "\n", SD_CARD_CID_GET_MDT( block));
		RTEMS_SYSLOG( "7-bit CRC checksum       : %" PRIu8 "\n",  SD_CARD_CID_GET_CRC7( block));
		crc7 = sd_card_compute_crc7( block, 15);
		if (crc7 != SD_CARD_CID_GET_CRC7( block))
			RTEMS_SYSLOG( "  Failed! (computed %02" PRIx8 ")\n", crc7);
	}

	/* Card Specific Data */

	/* Read CSD */
	rv = sd_card_send_command( e, SD_CARD_CMD_SEND_CSD, 0);
	RTEMS_CLEANUP_RV_SC( rv, sc, sd_card_driver_init_cleanup, "Send: SD_CARD_CMD_SEND_CSD");
	rv = sd_card_read( e, SD_CARD_START_BLOCK_SINGLE_BLOCK_READ, block, SD_CARD_CSD_SIZE);
	RTEMS_CLEANUP_RV_SC( rv, sc, sd_card_driver_init_cleanup, "Read: SD_CARD_CMD_SEND_CSD");

	crc7 = sd_card_compute_crc7( block, 15);
	if (crc7 != SD_CARD_CID_GET_CRC7( block)) {
		RTEMS_SYSLOG( "SD_CARD_CMD_SEND_CSD CRC failed\n");
		sc = RTEMS_IO_ERROR;
		goto sd_card_driver_init_cleanup;
	}
	
	/* CSD Structure */
	csd_structure = SD_CARD_CSD_GET_CSD_STRUCTURE( block);

	/* Transfer speed and access time */
	transfer_speed = sd_card_transfer_speed( block);
	e->transfer_mode.baudrate = transfer_speed;
	e->n_ac_max = sd_card_max_access_time( block, transfer_speed);

	/* Block sizes and capacity */
	if (csd_structure == 0 || !assume_sd) {
		/* Treat MMC same as CSD Version 1.0 */

		read_block_size = 1U << SD_CARD_CSD_GET_READ_BLK_LEN( block);
		e->block_size_shift = SD_CARD_CSD_GET_WRITE_BLK_LEN( block);
		write_block_size = 1U << e->block_size_shift;
		if (read_block_size < write_block_size) {
			RTEMS_SYSLOG_ERROR( "Read block size smaller than write block size\n");
			return -RTEMS_IO_ERROR;
		}
		e->block_size = write_block_size;
		e->block_number = sd_card_block_number( block);
		capacity = sd_card_capacity( block);
	} else if (csd_structure == 1) {
		uint32_t c_size = SD_CARD_CSD_1_GET_C_SIZE( block);

		/* Block size is fixed in CSD Version 2.0 */
		e->block_size_shift = 9;
		e->block_size = 512;

		e->block_number = (c_size + 1) * 1024;
		capacity = (c_size + 1) * 512 * 1024;
		read_block_size = 512;
		write_block_size = 512;

		/* Timeout is fixed at 100ms in CSD Version 2.0 */
		e->n_ac_max = transfer_speed / 80;
	} else {
		RTEMS_DO_CLEANUP_SC( RTEMS_IO_ERROR, sc, sd_card_driver_init_cleanup, "Unexpected CSD Structure number");
	}

	/* Print CSD information */
	if (e->verbose) {
		RTEMS_SYSLOG( "*** Card Specific Data ***\n");
		RTEMS_SYSLOG( "CSD structure            : %" PRIu8 "\n", SD_CARD_CSD_GET_CSD_STRUCTURE( block));
		RTEMS_SYSLOG( "Spec version             : %" PRIu8 "\n", SD_CARD_CSD_GET_SPEC_VERS( block));
		RTEMS_SYSLOG( "Access time [ns]         : %" PRIu32 "\n", sd_card_access_time( block));
		RTEMS_SYSLOG( "Access time [N]          : %" PRIu32 "\n", SD_CARD_CSD_GET_NSAC( block)*100);
		RTEMS_SYSLOG( "Max access time [N]      : %" PRIu32 "\n", e->n_ac_max);
		RTEMS_SYSLOG( "Max read block size [B]  : %" PRIu32 "\n", read_block_size);
		RTEMS_SYSLOG( "Max write block size [B] : %" PRIu32 "\n", write_block_size);
		RTEMS_SYSLOG( "Block size [B]           : %" PRIu32 "\n", e->block_size);
		RTEMS_SYSLOG( "Block number             : %" PRIu32 "\n", e->block_number);
		RTEMS_SYSLOG( "Capacity [B]             : %" PRIu64 "\n", capacity);
		RTEMS_SYSLOG( "Max transfer speed [b/s] : %" PRIu32 "\n", transfer_speed);
	}

	if (high_capacity) {
		/* For high capacity cards the address is in blocks */
		e->block_size_shift = 0;
	} else if (e->block_size_shift == 10) {
		/*
		 * Low capacity 2GByte cards with reported block size of 1024
		 * need to be set back to block size of 512 per 'Simplified
		 * Physical Layer Specification Version 2.0' section 4.3.2.
		 * Otherwise, CMD16 fails if set to 1024.
		 */
		e->block_size_shift = 9;
		e->block_size = 512;
		e->block_number *= 2;
	}

	/* Set read block size */
	rv = sd_card_send_command( e, SD_CARD_CMD_SET_BLOCKLEN, e->block_size);
	RTEMS_CLEANUP_RV_SC( rv, sc, sd_card_driver_init_cleanup, "Send: SD_CARD_CMD_SET_BLOCKLEN");

	/* Stop */
	sc = sd_card_stop( e);
	RTEMS_CHECK_SC( sc, "Stop");

	return RTEMS_SUCCESSFUL;

sd_card_driver_init_cleanup:

	/* Stop */
	sd_card_stop( e);

	return sc;
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

#ifdef DEBUG
	/* Check request */
	if (r->bufs[0].block >= e->block_number) {
		RTEMS_SYSLOG_ERROR( "Start block number out of range");
		return -RTEMS_INTERNAL_ERROR;
	} else if (r->bufnum > e->block_number - RTEMS_BLKDEV_START_BLOCK (r)) {
		RTEMS_SYSLOG_ERROR( "Block count out of range");
		return -RTEMS_INTERNAL_ERROR;
	}
#endif /* DEBUG */

	/* Start */
	sc = sd_card_start( e);
	RTEMS_CLEANUP_SC_RV( sc, rv, sd_card_disk_block_read_cleanup, "Start");

	if (r->bufnum == 1) {
#ifdef DEBUG
		/* Check buffer */
		if (r->bufs [0].length != e->block_size) {
			RTEMS_DO_CLEANUP_RV( -RTEMS_INTERNAL_ERROR, rv, sd_card_disk_block_read_cleanup, "Buffer and disk block size are not equal");
		}
		RTEMS_DEBUG_PRINT( "[01:01]: buffer = 0x%08x, size = %u\n", r->bufs [0].buffer, r->bufs [0].length);
#endif /* DEBUG */

		/* Single block read */
		rv = sd_card_send_command( e, SD_CARD_CMD_READ_SINGLE_BLOCK, start_address);
		RTEMS_CLEANUP_RV( rv, sd_card_disk_block_read_cleanup, "Send: SD_CARD_CMD_READ_SINGLE_BLOCK");
		rv = sd_card_read( e, SD_CARD_START_BLOCK_SINGLE_BLOCK_READ, (uint8_t *) r->bufs [0].buffer, (int) e->block_size);
		RTEMS_CLEANUP_RV( rv, sd_card_disk_block_read_cleanup, "Read: SD_CARD_CMD_READ_SINGLE_BLOCK");
	} else {
		/* Start multiple block read */
		rv = sd_card_send_command( e, SD_CARD_CMD_READ_MULTIPLE_BLOCK, start_address);
		RTEMS_CLEANUP_RV( rv, sd_card_disk_block_read_stop_cleanup, "Send: SD_CARD_CMD_READ_MULTIPLE_BLOCK");

		/* Multiple block read */
		for (i = 0; i < r->bufnum; ++i) {
#ifdef DEBUG
			/* Check buffer */
			if (r->bufs [i].length != e->block_size) {
				RTEMS_DO_CLEANUP_RV( -RTEMS_INTERNAL_ERROR, rv, sd_card_disk_block_read_stop_cleanup, "Buffer and disk block size are not equal");
			}
			RTEMS_DEBUG_PRINT( "[%02u:%02u]: buffer = 0x%08x, size = %u\n", i + 1, r->bufnum, r->bufs [i].buffer, r->bufs [i].length);
#endif /* DEBUG */

			rv = sd_card_read( e, SD_CARD_START_BLOCK_MULTIPLE_BLOCK_READ, (uint8_t *) r->bufs [i].buffer, (int) e->block_size);
			RTEMS_CLEANUP_RV( rv, sd_card_disk_block_read_stop_cleanup, "Read block");
		}

		/* Stop multiple block read */
		rv = sd_card_stop_multiple_block_read( e);
		RTEMS_CLEANUP_RV( rv, sd_card_disk_block_read_cleanup, "Stop multiple block read");
	}

	/* Stop */
	sc = sd_card_stop( e);
	RTEMS_CHECK_SC_RV( sc, "Stop");

	/* Done */
	r->req_done( r->done_arg, RTEMS_SUCCESSFUL);

	return 0;

sd_card_disk_block_read_stop_cleanup:

	/* Stop multiple block read */
	sd_card_stop_multiple_block_read( e);

sd_card_disk_block_read_cleanup:

	/* Stop */
	sd_card_stop( e);

	/* Done */
	r->req_done( r->done_arg, RTEMS_IO_ERROR);

	return rv;
}

static int sd_card_disk_block_write( sd_card_driver_entry *e, rtems_blkdev_request *r)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	int rv = 0;
	uint32_t start_address = RTEMS_BLKDEV_START_BLOCK (r) << e->block_size_shift;
	uint32_t i = 0;

#ifdef DEBUG
	/* Check request */
	if (r->bufs[0].block >= e->block_number) {
		RTEMS_SYSLOG_ERROR( "Start block number out of range");
		return -RTEMS_INTERNAL_ERROR;
	} else if (r->bufnum > e->block_number - RTEMS_BLKDEV_START_BLOCK (r)) {
		RTEMS_SYSLOG_ERROR( "Block count out of range");
		return -RTEMS_INTERNAL_ERROR;
	}
#endif /* DEBUG */

	/* Start */
	sc = sd_card_start( e);
	RTEMS_CLEANUP_SC_RV( sc, rv, sd_card_disk_block_write_cleanup, "Start");

	if (r->bufnum == 1) {
#ifdef DEBUG
		/* Check buffer */
		if (r->bufs [0].length != e->block_size) {
			RTEMS_DO_CLEANUP_RV( -RTEMS_INTERNAL_ERROR, rv, sd_card_disk_block_write_cleanup, "Buffer and disk block size are not equal");
		}
		RTEMS_DEBUG_PRINT( "[01:01]: buffer = 0x%08x, size = %u\n", r->bufs [0].buffer, r->bufs [0].length);
#endif /* DEBUG */

		/* Single block write */
		rv = sd_card_send_command( e, SD_CARD_CMD_WRITE_BLOCK, start_address);
		RTEMS_CLEANUP_RV( rv, sd_card_disk_block_write_cleanup, "Send: SD_CARD_CMD_WRITE_BLOCK");
		rv = sd_card_write( e, SD_CARD_START_BLOCK_SINGLE_BLOCK_WRITE, (uint8_t *) r->bufs [0].buffer, (int) e->block_size);
		RTEMS_CLEANUP_RV( rv, sd_card_disk_block_write_cleanup, "Write: SD_CARD_CMD_WRITE_BLOCK");
	} else {
		/* Start multiple block write */
		rv = sd_card_send_command( e, SD_CARD_CMD_WRITE_MULTIPLE_BLOCK, start_address);
		RTEMS_CLEANUP_RV( rv, sd_card_disk_block_write_stop_cleanup, "Send: SD_CARD_CMD_WRITE_MULTIPLE_BLOCK");

		/* Multiple block write */
		for (i = 0; i < r->bufnum; ++i) {
#ifdef DEBUG
			/* Check buffer */
			if (r->bufs [i].length != e->block_size) {
				RTEMS_DO_CLEANUP_RV( -RTEMS_INTERNAL_ERROR, rv, sd_card_disk_block_write_stop_cleanup, "Buffer and disk block size are not equal");
			}
			RTEMS_DEBUG_PRINT( "[%02u:%02u]: buffer = 0x%08x, size = %u\n", i + 1, r->bufnum, r->bufs [i].buffer, r->bufs [i].length);
#endif /* DEBUG */

			rv = sd_card_write( e, SD_CARD_START_BLOCK_MULTIPLE_BLOCK_WRITE, (uint8_t *) r->bufs [i].buffer, (int) e->block_size);
			RTEMS_CLEANUP_RV( rv, sd_card_disk_block_write_stop_cleanup, "Write block");
		}

		/* Stop multiple block write */
		rv = sd_card_stop_multiple_block_write( e);
		RTEMS_CLEANUP_RV( rv, sd_card_disk_block_write_cleanup, "Stop multiple block write");
	}

	/* Get card status */
	rv = sd_card_send_command( e, SD_CARD_CMD_SEND_STATUS, 0);
	RTEMS_CHECK_RV( rv, "Send: SD_CARD_CMD_SEND_STATUS");

	/* Stop */
	sc = sd_card_stop( e);
	RTEMS_CHECK_SC_RV( sc, "Stop");

	/* Done */
	r->req_done( r->done_arg, RTEMS_SUCCESSFUL);

	return 0;

sd_card_disk_block_write_stop_cleanup:

	/* Stop multiple block write */
	sd_card_stop_multiple_block_write( e);

sd_card_disk_block_write_cleanup:

	/* Get card status */
	rv = sd_card_send_command( e, SD_CARD_CMD_SEND_STATUS, 0);
	RTEMS_CHECK_RV( rv, "Send: SD_CARD_CMD_SEND_STATUS");

	/* Stop */
	sd_card_stop( e);

	/* Done */
	r->req_done( r->done_arg, RTEMS_IO_ERROR);

	return rv;
}

static int sd_card_disk_ioctl( rtems_disk_device *dd, uint32_t req, void *arg)
{
	RTEMS_DEBUG_PRINT( "dev = %u, req = %u, arg = 0x08%x\n", dev, req, arg);
	if (req == RTEMS_BLKIO_REQUEST) {
		rtems_device_minor_number minor = rtems_disk_get_minor_number( dd);
		sd_card_driver_entry *e = &sd_card_driver_table [minor];
		rtems_blkdev_request *r = (rtems_blkdev_request *) arg;
		int (*f)( sd_card_driver_entry *, rtems_blkdev_request *);
		uint32_t retries = e->retries;
		int result;

		switch (r->req) {
			case RTEMS_BLKDEV_REQ_READ:
				f = sd_card_disk_block_read;
				break;
			case RTEMS_BLKDEV_REQ_WRITE:
				f = sd_card_disk_block_write;
				break;
			default:
				errno = EINVAL;
				return -1;
		}
		do {
			result = f( e, r);
		} while (retries-- > 0 && result != 0);
		return result;

	} else if (req == RTEMS_BLKIO_CAPABILITIES) {
		*(uint32_t *) arg = RTEMS_BLKDEV_CAP_MULTISECTOR_CONT;
		return 0;
	} else {
		errno = EINVAL;
		return -1;
	}
}

static rtems_status_code sd_card_disk_init( rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;

	/* Initialize disk IO */
	sc = rtems_disk_io_initialize();
	RTEMS_CHECK_SC( sc, "Initialize RTEMS disk IO");

	for (minor = 0; minor < sd_card_driver_table_size; ++minor) {
		sd_card_driver_entry *e = &sd_card_driver_table [minor];
		dev_t dev = rtems_filesystem_make_dev_t( major, minor);
		uint32_t retries = e->retries;

		/* Initialize SD Card */
		do {
			sc = sd_card_init( e);
		} while (retries-- > 0 && sc != RTEMS_SUCCESSFUL);
		RTEMS_CHECK_SC( sc, "Initialize SD Card");

		/* Create disk device */
		sc = rtems_disk_create_phys( dev, e->block_size, e->block_number, sd_card_disk_ioctl, NULL, e->device_name);
		RTEMS_CHECK_SC( sc, "Create disk device");
	}

	return RTEMS_SUCCESSFUL;
}

/** @} */

static const rtems_driver_address_table sd_card_disk_ops = {
	.initialization_entry = sd_card_disk_init,
	.open_entry = rtems_blkdev_generic_open,
	.close_entry = rtems_blkdev_generic_close,
	.read_entry = rtems_blkdev_generic_read,
	.write_entry = rtems_blkdev_generic_write,
	.control_entry = rtems_blkdev_generic_ioctl
};

rtems_status_code sd_card_register( void)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	rtems_device_major_number major = 0;

	sc = rtems_io_register_driver( 0, &sd_card_disk_ops, &major);
	RTEMS_CHECK_SC( sc, "Register disk SD Card driver");

	return RTEMS_SUCCESSFUL;
}
