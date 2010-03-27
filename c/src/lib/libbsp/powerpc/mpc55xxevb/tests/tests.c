/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief Some tests.
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

#include <rtems/irq.h>
#include <mpc55xx/regs.h>
#include <mpc55xx/mpc55xx.h>
#include <mpc55xx/dspi.h>
#include <mpc55xx/edma.h>

#include <bsp/irq-generic.h>

#include <libchip/spi-sd-card.h>

#include <bsp.h>
#include <bsp/irq.h>

#include <rtems/irq-extension.h>

#include <libcpu/powerpc-utility.h>

#include <rtems/status-checks.h>

#undef USE_DSPI_READER_WRITER

#if defined(USE_DSPI_READER_WRITER)
static rtems_driver_address_table test_mpc55xx_drv_ops = {
	initialization_entry : NULL,
	open_entry : NULL,
	close_entry : NULL,
	read_entry : NULL,
	write_entry : NULL,
	control_entry : NULL
};

static rtems_libi2c_drv_t test_mpc55xx_dspi_drv = {
	ops : &test_mpc55xx_drv_ops,
	size : sizeof( rtems_libi2c_drv_t)
};
#endif /* defined(USE_DSPI_READER_WRITER) */

#define MPC55XX_TEST_DSPI_ADDRESS 0

/* #define MPC55XX_TEST_DSPI_BUFSIZE (16 * 32) */
#define MPC55XX_TEST_DSPI_BUFSIZE 8

/* #define MPC55XX_TEST_DSPI_BUFSIZE_CACHE_PROOF MPC55XX_TEST_DSPI_BUFSIZE */
#define MPC55XX_TEST_DSPI_BUFSIZE_CACHE_PROOF 32

rtems_device_minor_number test_mpc55xx_dspi_bus [MPC55XX_DSPI_NUMBER];

static rtems_libi2c_tfr_mode_t test_mpc55xx_dspi_transfer_mode = { baudrate : 550000, bits_per_char : 8, lsb_first : FALSE, clock_inv : FALSE, clock_phs : FALSE };

static rtems_id test_mpc55xx_dspi_ping;

static rtems_id test_mpc55xx_dspi_pong;

#if defined(USE_DSPI_READER_WRITER)
static unsigned char test_mpc55xx_dspi_writer_outbuf [2] [MPC55XX_TEST_DSPI_BUFSIZE_CACHE_PROOF] __attribute__ ((aligned (32)));

static unsigned char test_mpc55xx_dspi_writer_inbuf [MPC55XX_TEST_DSPI_BUFSIZE_CACHE_PROOF] __attribute__ ((aligned (32)));

static unsigned char test_mpc55xx_dspi_reader_inbuf [MPC55XX_TEST_DSPI_BUFSIZE_CACHE_PROOF] __attribute__ ((aligned (32)));

static rtems_task test_mpc55xx_dspi_writer( rtems_task_argument arg)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	int rv = 0;
	rtems_device_minor_number device;
	rtems_libi2c_read_write_t read_and_write = { rd_buf : NULL, wr_buf : NULL, byte_cnt : 0 };
	int i = 0;

	RTEMS_DEBUG_PRINT( "Task started\n");

	device = rtems_libi2c_register_drv( NULL, &test_mpc55xx_dspi_drv, test_mpc55xx_dspi_bus [2], 0);
	RTEMS_CHECK_RV_TASK( device, "rtems_libi2c_register_drv");

	sc = rtems_libi2c_send_start( device);
	RTEMS_CHECK_SC_TASK( sc, "rtems_libi2c_send_start");

	rv = rtems_libi2c_ioctl( device, RTEMS_LIBI2C_IOCTL_SET_TFRMODE, &test_mpc55xx_dspi_transfer_mode);
	RTEMS_CHECK_RV_TASK( rv, "rtems_libi2c_ioctl");

	sc = rtems_libi2c_send_addr( device, MPC55XX_TEST_DSPI_ADDRESS);
	RTEMS_CHECK_SC_TASK( sc, "rtems_libi2c_send_addr");

	for (i = 0; i < MPC55XX_TEST_DSPI_BUFSIZE; ++i) {
		test_mpc55xx_dspi_writer_outbuf [0] [i] = 0xa5;
		test_mpc55xx_dspi_writer_outbuf [1] [i] = 0xa5;
		/* test_mpc55xx_dspi_writer_outbuf [0] [i] = i + 1; */
		/* test_mpc55xx_dspi_writer_outbuf [1] [i] = -(i + 1); */
	}

	int toggle = 0;
	read_and_write.byte_cnt = MPC55XX_TEST_DSPI_BUFSIZE;
	read_and_write.rd_buf = test_mpc55xx_dspi_writer_inbuf;
	read_and_write.wr_buf = test_mpc55xx_dspi_writer_outbuf [toggle];
	while (1) {
		tic();

		/* sc = rtems_semaphore_obtain( test_mpc55xx_dspi_pong, RTEMS_WAIT, RTEMS_NO_TIMEOUT); */
		/* RTEMS_CHECK_SC_TASK( sc, "rtems_semaphore_obtain"); */

		RTEMS_DEBUG_PRINT( "Ping\n");

		/* sc = rtems_libi2c_send_start( device); */
		/* RTEMS_CHECK_SC_TASK( sc, "rtems_libi2c_send_start"); */

		/* sc = rtems_libi2c_send_addr( device, MPC55XX_TEST_DSPI_ADDRESS); */
		/* RTEMS_CHECK_SC_TASK( sc, "rtems_libi2c_send_addr"); */

		rv = rtems_libi2c_ioctl( device, RTEMS_LIBI2C_IOCTL_READ_WRITE, &read_and_write);
		RTEMS_CHECK_RV_TASK( rv, "rtems_libi2c_ioctl: RTEMS_LIBI2C_IOCTL_READ_WRITE");

		/* rv = rtems_libi2c_write_bytes( device, test_mpc55xx_dspi_writer_outbuf [0], MPC55XX_TEST_DSPI_BUFSIZE); */
		/* RTEMS_CHECK_RV_TASK( rv, "rtems_libi2c_write_bytes"); */

		/* sc = rtems_libi2c_send_stop( device); */
		/* RTEMS_CHECK_SC_TASK( sc, "rtems_libi2c_send_stop"); */

		toggle = toggle ? 0 : 1;
		read_and_write.wr_buf = test_mpc55xx_dspi_writer_outbuf [toggle];

		/* sc = rtems_semaphore_release( test_mpc55xx_dspi_ping); */
		/* RTEMS_CHECK_SC_TASK( sc, "rtems_semaphore_release"); */
	}

	sc = rtems_libi2c_send_stop( device);
	RTEMS_CHECK_SC_TASK( sc, "rtems_libi2c_send_stop");

	sc = rtems_task_delete( RTEMS_SELF);
	RTEMS_CHECK_SC_TASK( sc, "rtems_task_delete");
}

static rtems_task test_mpc55xx_dspi_reader( rtems_task_argument arg)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	int rv = 0;
	rtems_device_minor_number device;
	int i = 0;

	RTEMS_DEBUG_PRINT( "Task started\n");

	device = rtems_libi2c_register_drv( NULL, &test_mpc55xx_dspi_drv, test_mpc55xx_dspi_bus [3], 0);
	RTEMS_CHECK_RV_TASK( device, "rtems_libi2c_register_drv");

	sc = rtems_libi2c_send_start( device);
	RTEMS_CHECK_SC_TASK( sc, "rtems_libi2c_send_start");

	rv = rtems_libi2c_ioctl( device, RTEMS_LIBI2C_IOCTL_SET_TFRMODE, &test_mpc55xx_dspi_transfer_mode);
	RTEMS_CHECK_RV_TASK( rv, "rtems_libi2c_ioctl");

	sc = rtems_libi2c_send_addr( device, MPC55XX_TEST_DSPI_ADDRESS);
	RTEMS_CHECK_SC_TASK( sc, "rtems_libi2c_send_addr");

	for (i = 0; i < MPC55XX_TEST_DSPI_BUFSIZE; ++i) {
		test_mpc55xx_dspi_reader_inbuf [i] = -1;
	}

	sc = rtems_semaphore_obtain( test_mpc55xx_dspi_ping, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
	RTEMS_CHECK_SC_TASK( sc, "rtems_semaphore_obtain");

	RTEMS_DEBUG_PRINT( "Pong\n");

	sc = rtems_semaphore_release( test_mpc55xx_dspi_pong);
	RTEMS_CHECK_SC_TASK( sc, "rtems_semaphore_release");

	while (1) {
		sc = rtems_semaphore_obtain( test_mpc55xx_dspi_ping, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		RTEMS_CHECK_SC_TASK( sc, "rtems_semaphore_obtain");

		RTEMS_DEBUG_PRINT( "Pong\n");

		rv = rtems_libi2c_read_bytes( device, test_mpc55xx_dspi_reader_inbuf, MPC55XX_TEST_DSPI_BUFSIZE);
		RTEMS_CHECK_RV_TASK( rv, "rtems_libi2c_read_bytes");

		sc = rtems_semaphore_release( test_mpc55xx_dspi_pong);
		RTEMS_CHECK_SC_TASK( sc, "rtems_semaphore_release");

		printk( "Time: %i, Value: 0x%02x%02x%02x%02x\n", tac(),
			test_mpc55xx_dspi_reader_inbuf [0], test_mpc55xx_dspi_reader_inbuf [1],
			test_mpc55xx_dspi_reader_inbuf [2], test_mpc55xx_dspi_reader_inbuf [3]);
	}

	sc = rtems_libi2c_send_stop( device);
	RTEMS_CHECK_SC_TASK( sc, "rtems_libi2c_send_stop");

	sc = rtems_task_delete( RTEMS_SELF);
	RTEMS_CHECK_SC_TASK( sc, "rtems_task_delete");
}
#endif /* defined(USE_DSPI_READER_WRITER) */

rtems_task test_sd_card( rtems_task_argument arg);

static rtems_task test_mpc55xx_intc( rtems_task_argument arg);

rtems_status_code mpc55xx_dspi_register(void)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	int rv = 0;
	int i = 0;
	char device_name [] = "/dev/spi0";
	union SIU_PCR_tag pcr = MPC55XX_ZERO_FLAGS;

	printk( "Boot time: %u\n", ppc_time_base());

	/*
	test_mpc55xx_intc( 0);
	*/

	rv = rtems_libi2c_initialize();
	RTEMS_CHECK_RV_SC( rv, "rtems_libi2c_initialize");

	/* DSPI D inputs are taken from DSPI C */
	SIU.DISR.R = 0x000000FC;

	/* DSPI A signals */
	pcr.B.PA = 1;
	pcr.B.ODE = 0;
	pcr.B.HYS = 0;
	pcr.B.SRC = 3;
	pcr.B.WPE = 1;
	pcr.B.WPS = 1;

	/* SCK */
	pcr.B.OBE = 1;
	pcr.B.IBE = 0;
	SIU.PCR [93].R = pcr.R;

	/* SIN */
	pcr.B.OBE = 0;
	pcr.B.IBE = 1;
	SIU.PCR [94].R = pcr.R;

	/* SOUT */
	pcr.B.OBE = 1;
	pcr.B.IBE = 0;
	SIU.PCR [95].R = pcr.R;

	/* PCSx */
	pcr.B.OBE = 1;
	pcr.B.IBE = 0;
	SIU.PCR [96].R = pcr.R;
	SIU.PCR [97].R = pcr.R;
	SIU.PCR [98].R = pcr.R;
	SIU.PCR [99].R = pcr.R;
	SIU.PCR [100].R = pcr.R;
	SIU.PCR [101].R = pcr.R;

	mpc55xx_dspi_bus_table [3].master = 0;
	for (i = 0; i < MPC55XX_DSPI_NUMBER; ++i) {
		device_name [8] = '0' + i;
		rv = rtems_libi2c_register_bus( device_name, (rtems_libi2c_bus_t *) &mpc55xx_dspi_bus_table [i]);
		RTEMS_CHECK_RV_SC( rv, device_name);
		test_mpc55xx_dspi_bus [i] = rv;
	}

	sc = rtems_semaphore_create (
		rtems_build_name ( 'P', 'I', 'N', 'G'),
		1,
		RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_PRIORITY,
		RTEMS_NO_PRIORITY,
		&test_mpc55xx_dspi_ping
	);
	RTEMS_CHECK_SC( sc, "rtems_semaphore_create");

	sc = rtems_semaphore_create (
		rtems_build_name ( 'P', 'O', 'N', 'G'),
		0,
		RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_PRIORITY,
		RTEMS_NO_PRIORITY,
		&test_mpc55xx_dspi_pong
	);
	RTEMS_CHECK_SC( sc, "rtems_semaphore_create");

#if defined(USE_DSPI_READER_WRITER)
	rtems_id writer_task_id;
	rtems_id reader_task_id;

	sc = rtems_task_create(
		rtems_build_name( 'T', 'W', 'R', 'T'),
		2,
		RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_DEFAULT_ATTRIBUTES,
		&writer_task_id
	);
	RTEMS_CHECK_SC( sc, "rtems_task_create");
	sc = rtems_task_create(
		rtems_build_name( 'T', 'R', 'D', 'R'),
		1,
		RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_DEFAULT_ATTRIBUTES,
		&reader_task_id
	);
	RTEMS_CHECK_SC( sc, "rtems_task_create");

	sc = rtems_task_start( writer_task_id, test_mpc55xx_dspi_writer, 0);
	RTEMS_CHECK_SC( sc, "rtems_task_start");
	sc = rtems_task_start( reader_task_id, test_mpc55xx_dspi_reader, 0);
	RTEMS_CHECK_SC( sc, "rtems_task_start");
#endif

	rtems_id sd_card_task_id;
	sc = rtems_task_create(
		rtems_build_name( 'T', 'S', 'D', 'C'),
		1,
		RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_DEFAULT_ATTRIBUTES,
		&sd_card_task_id
	);
	RTEMS_CHECK_SC( sc, "rtems_task_create");
	sc = rtems_task_start( sd_card_task_id, test_sd_card, 0);
	RTEMS_CHECK_SC( sc, "rtems_task_start");

	return RTEMS_SUCCESSFUL;

	rtems_id intc_id;
	sc = rtems_task_create(
		rtems_build_name( 'I', 'N', 'T', 'C'),
		2,
		RTEMS_MINIMUM_STACK_SIZE,
		RTEMS_DEFAULT_MODES,
		RTEMS_DEFAULT_ATTRIBUTES,
		&intc_id
	);
	RTEMS_CHECK_SC( sc, "rtems_task_create");
	sc = rtems_task_start( intc_id, test_mpc55xx_intc, 0);
	RTEMS_CHECK_SC( sc, "rtems_task_start");

	return RTEMS_SUCCESSFUL;
}

#if 0

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>
#include <rtems/fsmount.h>
#include <rtems/dosfs.h>
#include <rtems/ide_part_table.h>

#define TEST_SD_CARD_BUF_SIZE 512
#define TEST_SD_CARD_BIGBUF_SIZE (256 * 1024)

#define TEST_SD_CARD_DEVICE_FILE "/dev/sd-card-a"
#define TEST_SD_CARD_PARTITION "/dev/sd-card-a1"
#define TEST_SD_CARD_MOUNT_POINT "/mnt"
#define TEST_SD_CARD_DIRECTORY "/mnt/testdir"

static fstab_t test_sd_card_fs_table [] = { {
		TEST_SD_CARD_PARTITION, TEST_SD_CARD_MOUNT_POINT,
		&msdos_ops, RTEMS_FILESYSTEM_READ_WRITE,
		FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
		FSMOUNT_MNT_OK
	}, {
		TEST_SD_CARD_DEVICE_FILE, TEST_SD_CARD_MOUNT_POINT,
		&msdos_ops, RTEMS_FILESYSTEM_READ_WRITE,
		FSMOUNT_MNT_OK | FSMOUNT_MNTPNT_CRTERR | FSMOUNT_MNT_FAILED,
		0
	}
};

static uint8_t test_sd_card_buf [TEST_SD_CARD_BUF_SIZE] __attribute__ ((aligned (32)));

static int test_sd_card_print_dir( const char* dirname, unsigned level)
{
	int rv = 0;
	DIR *dir = NULL;
	struct dirent *ent;
	struct stat s;
	int i = 0;

	/* Open */
	dir = opendir( dirname);
	rv = dir == NULL ? -1 : 0;
	RTEMS_CHECK_RV( rv, "Open directory");

	/* Change CWD */
	rv = chdir( dirname);
	RTEMS_CHECK_RV( rv, "Change directory");

	/* Read */
	ent = readdir( dir);
	while (ent != NULL) {
		if (stat( ent->d_name, &s) == 0 && strcmp( ".", ent->d_name) != 0 && strcmp( "..", ent->d_name)) {
			for (i = 0; i < level; ++i) {
				printk( "\t");
			}
			printk( "<%s>\n", ent->d_name);
			if (S_ISDIR( s.st_mode)) {
				rv = test_sd_card_print_dir( ent->d_name, level + 1);
				RTEMS_CHECK_RV( rv, "Next directory");
			}
		}
		ent = readdir( dir);
	}

	/* Change CWD */
	rv = chdir( "..");
	RTEMS_CHECK_RV( rv, "Change directory");

	/* Close */
	rv = closedir( dir);
	RTEMS_CHECK_RV( rv, "Close directory");

	return 0;
}

rtems_task test_sd_card( rtems_task_argument arg)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	int rv = 0;
	rtems_device_minor_number minor;
	sd_card_driver_entry *e = &sd_card_driver_table [0];
	int fd = 0;
	unsigned i = 0;
	unsigned avg = 0;
	unsigned t = 0;
	char file_name [] = "00000000.TXT";
	uint8_t *buf = NULL;

	RTEMS_DEBUG_PRINT( "Task started\n");

	minor = rtems_libi2c_register_drv( e->device_name, (rtems_libi2c_drv_t *) e, test_mpc55xx_dspi_bus [0], 0);
	RTEMS_CHECK_RV_TASK( (int) minor, "rtems_libi2c_register_drv");

	buf = malloc( TEST_SD_CARD_BIGBUF_SIZE);
	for (i = 0; i < TEST_SD_CARD_BIGBUF_SIZE; ++i) {
		if (i % 27 == 26) {
			buf [i] = '\n';
		} else {
			buf [i] = 'A' + i % 27;
		}
	}
	buf [i - 1] = '\n';

	rv = test_sd_card_print_dir( "/dev", 0);
	RTEMS_CHECK_RV_TASK( rv, "Print directory");

	sc = rtems_ide_part_table_initialize( TEST_SD_CARD_DEVICE_FILE);
	RTEMS_CHECK_SC_TASK( sc, "Initialize IDE partition table");

	rv = mkdir( TEST_SD_CARD_MOUNT_POINT, S_IRWXU);
	RTEMS_CHECK_RV_TASK( rv, "Create mount point");

	rv = rtems_fsmount( test_sd_card_fs_table, sizeof( test_sd_card_fs_table) / sizeof( test_sd_card_fs_table [0]), NULL);
	RTEMS_CHECK_RV_TASK( rv, "Mount file systems");

	rv = test_sd_card_print_dir( "/dev", 0);
	RTEMS_CHECK_RV_TASK( rv, "Print directory");

	rv = test_sd_card_print_dir( TEST_SD_CARD_MOUNT_POINT, 0);
	RTEMS_CHECK_RV_TASK( rv, "Print directory");

	(void) rtems_task_delete( RTEMS_SELF);

	rv = mkdir( TEST_SD_CARD_DIRECTORY, S_IRWXU);

	rv = chdir( TEST_SD_CARD_DIRECTORY);
	RTEMS_CHECK_RV_TASK( rv, "Change directory");

	i = 0;
	while (1) {
		snprintf( file_name, 13, "%08i.TXT", i);
		tic();
		fd = creat( file_name, S_IREAD | S_IWRITE);
		RTEMS_CHECK_RV_TASK( fd, "Create file");
		rv = write( fd, buf, TEST_SD_CARD_BIGBUF_SIZE);
		RTEMS_CHECK_RV_TASK( rv, "Write file");
		rv = close( fd);
		RTEMS_CHECK_RV_TASK( rv, "Close file");
		t = tac();
		avg = ((uint64_t) avg * ((uint64_t) i) + (uint64_t) t) / ((uint64_t) i + 1);
		printk( "%s: %u (%u)\n", file_name, tac(), avg);
		++i;
	}

	rv = chdir( "..");
	RTEMS_CHECK_RV_TASK( rv, "Change directory");

	rv = test_sd_card_print_dir( TEST_SD_CARD_DIRECTORY, 0);
	RTEMS_CHECK_RV_TASK( rv, "Print directory");

	(void) rtems_task_delete( RTEMS_SELF);


	#if 0
	/* Write */
	int b = 0;
	const char device_name [] = "/dev/spi0.sd-card-0";
	fd = open( device_name, O_RDWR);

	RTEMS_CHECK_RV_TASK( fd, "open");
	while (1) {
		for (i = 0; i < TEST_SD_CARD_BUF_SIZE; ++i) {
			test_sd_card_buf [i] = b;
		}
		++b;
		rv = write( fd, test_sd_card_buf, TEST_SD_CARD_BUF_SIZE);
		if (rv < 0) {
			break;
		}
	}
	rv = close( fd);
	RTEMS_CHECK_RV_TASK( rv, "close");

	/* Read */
	fd = open( device_name, O_RDWR);
	RTEMS_CHECK_RV_TASK( fd, "open");
	while (1) {
		rv = read( fd, test_sd_card_buf, TEST_SD_CARD_BUF_SIZE);
		if (rv < 0) {
			break;
		}
		printk( "%02x", test_sd_card_buf [rv - 1]);
		if (i++ % 64 == 0) {
			printk( "\n");
		}
	}
	rv = close( fd);
	RTEMS_CHECK_RV_TASK( rv, "close");
	#endif

	(void) rtems_task_delete( RTEMS_SELF);
}

#endif

#define ITER 4
#define BUFSIZE (128 * ITER)

static char inbuf [BUFSIZE];
static char outbuf [BUFSIZE];

#if 0
static void test_mpc55xx_edma_done( mpc55xx_edma_channel_entry *e, uint32_t error_status)
{
	rtems_semaphore_release( e->id);

	if (error_status != 0) {
		printk( "%s: Error status: 0x%08x\n", __func__, error_status);
	}
}

static rtems_status_code test_mpc55xx_edma(void)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	mpc55xx_edma_channel_entry e = {
		.channel = 0,
		.done = test_mpc55xx_edma_done,
		.id = RTEMS_ID_NONE
	};

	sc = rtems_semaphore_create (
		rtems_build_name ( 'T', 'S', 'T', 'C'),
		0,
		RTEMS_SIMPLE_BINARY_SEMAPHORE | RTEMS_PRIORITY,
		RTEMS_NO_PRIORITY,
		&e.id
	);
	RTEMS_CHECK_SC( sc, "rtems_semaphore_create");

	sc = mpc55xx_edma_obtain_channel( &e);
	RTEMS_CHECK_RV( sc, "mpc55xx_edma_obtain_channel");

	int i = 0;
	for (i = 0; i < BUFSIZE; ++i) {
		inbuf [i] = i;
		outbuf [i] = -1;
	}
	rtems_cache_flush_multiple_data_lines( inbuf, BUFSIZE);
	rtems_cache_flush_multiple_data_lines( outbuf, BUFSIZE);

	struct tcd_t tcd = EDMA_TCD_DEFAULT;
	tcd.SADDR = (uint32_t) &inbuf;
	tcd.DADDR = (uint32_t) &outbuf;
	tcd.NBYTES = BUFSIZE / ITER;
	tcd.SLAST = -BUFSIZE;
	tcd.CDF.B.CITER = ITER;
	tcd.BMF.B.BITER = ITER;
	tcd.BMF.B.INT_HALF = 1;

	EDMA.TCD [e.channel] = tcd;

	while (1) {
		while (1) {
			if (EDMA.TCD [e.channel].BMF.B.DONE == 1) {
				EDMA.TCD [e.channel].BMF.B.DONE = 0;
				printk( "%s: Done\n", __func__);
				break;
			} else if (EDMA.TCD [e.channel].BMF.B.ACTIVE == 0) {
				EDMA.SSBR.R = e.channel;
				printk( "%s: Start: %i (%i)\n", __func__, EDMA.TCD [e.channel].CDF.B.CITER, EDMA.TCD [e.channel].BMF.B.BITER);
			}
			sc = rtems_semaphore_obtain( e.id, RTEMS_WAIT, 10);
			if (sc == RTEMS_TIMEOUT) {
				continue;
			}
			RTEMS_CHECK_SC( sc, "rtems_semaphore_obtain");
		}
	}

	return RTEMS_SUCCESSFUL;
}
#endif /* 0 */

#include <stdlib.h>

static unsigned test_mpc55xx_intc_counter = 0;

static inline void test_mpc55xx_intc_worker( void *data)
{
	int s = 0;
	int i = *(int *) data;
	printk( "(%i): Start: %u\n", i, tac());
	s = mpc55xx_intc_clear_software_irq( i);
	if (i < MPC55XX_IRQ_SOFTWARE_NUMBER) {
		tic();
		s = mpc55xx_intc_raise_software_irq( i + 1);
	}
	++test_mpc55xx_intc_counter;
	printk( "(%i): Done\n", i);
}

static void test_mpc55xx_intc_handler( void *data)
{
	test_mpc55xx_intc_worker( data);
}

static void test_mpc55xx_intc_handler_2( void *data)
{
	test_mpc55xx_intc_worker( data);
}

static void test_mpc55xx_intc_handler_3( void *data)
{
	test_mpc55xx_intc_worker( data);
}

static int test_mpc55xx_intc_handler_data [MPC55XX_IRQ_SOFTWARE_NUMBER];

static rtems_task test_mpc55xx_intc( rtems_task_argument arg)
{
	rtems_status_code sc = RTEMS_SUCCESSFUL;
	rtems_irq_connect_data e = {
		.on = NULL,
		.off = NULL,
		.isOn = NULL
	};
	volatile int i = 0;
	int p = 0;
	unsigned s = 0;

	for (i = MPC55XX_IRQ_SOFTWARE_MIN, p = MPC55XX_INTC_MIN_PRIORITY; i <= MPC55XX_IRQ_SOFTWARE_MAX; ++i, ++p) {
		test_mpc55xx_intc_handler_data [i] = i;
		e.name = i;
		e.handle = &test_mpc55xx_intc_handler_data [i];

		sc = rtems_interrupt_handler_install( i, "test_mpc55xx_intc_handler", RTEMS_INTERRUPT_SHARED, test_mpc55xx_intc_handler, e.handle);
		if (sc != RTEMS_SUCCESSFUL) {
			BSP_panic( "Handler install failed");
		}

		e.hdl = test_mpc55xx_intc_handler_2;
		if (BSP_install_rtems_shared_irq_handler( &e) != 1) {
			BSP_panic( "Handler install 2 failed");
		}

		e.hdl = test_mpc55xx_intc_handler_3;
		if (BSP_install_rtems_shared_irq_handler( &e) != 1) {
			BSP_panic( "Handler install 3 failed");
		}
	}

	while (1) {
		i = (int) (7.0 * (rand_r( &s) / (RAND_MAX + 1.0)));
		tic();
		mpc55xx_intc_raise_software_irq( i);
	}
}
