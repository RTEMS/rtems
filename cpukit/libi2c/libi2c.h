/**
 * @file
 *
 * @ingroup libi2c
 *
 * @brief I2C library.
 */

#ifndef _RTEMS_LIBI2C_H
#define _RTEMS_LIBI2C_H

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2005,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */

#include <rtems.h>

#include <rtems/io.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup libi2c I2C Library
 *
 * @brief I2C library.
 *
 * @{
 */

/* Simple I2C driver API */

/* Initialize the libary - may fail if no semaphore or no driver slot is available */
extern int rtems_libi2c_initialize (void);

/* Alternatively to rtems_libi2c_initialize() the library can also be
 * initialized by means of a traditional driver table entry containing
 * the following entry points:
 */
extern rtems_status_code
rtems_i2c_init (
	rtems_device_major_number major,
	rtems_device_minor_number minor,
    void *arg);

extern rtems_status_code
rtems_i2c_open (
	rtems_device_major_number major,
	rtems_device_minor_number minor,
    void *arg);

extern rtems_status_code
rtems_i2c_close (
	rtems_device_major_number major,
	rtems_device_minor_number minor,
    void *arg);

extern rtems_status_code
rtems_i2c_read (
	rtems_device_major_number major,
	rtems_device_minor_number minor,
    void *arg);

extern rtems_status_code
rtems_i2c_write (
	rtems_device_major_number major,
	rtems_device_minor_number minor,
    void *arg);

extern rtems_status_code
rtems_i2c_ioctl (
	rtems_device_major_number major,
	rtems_device_minor_number minor,
    void *arg);

extern const rtems_driver_address_table rtems_libi2c_io_ops;

/* Unfortunately, if you want to add this driver to
 * a RTEMS configuration table then you need all the
 * members explicitly :-( (Device_driver_table should
 * hold pointers to rtems_driver_address_tables rather
 * than full structs).
 *
 * The difficulty is that adding this driver to the
 * configuration table is not enough; you still need
 * to populate the framework with low-level bus-driver(s)
 * and high-level drivers and/or device-files...
 *
 * Currently the preferred way is having the BSP
 * call 'rtems_libi2c_initialize' followed by
 * 'rtems_libi2c_register_bus' and
 * 'rtems_libi2c_register_drv' and/or
 * 'mknod' (for 'raw' device nodes)
 * from the 'pretasking_hook'.
 */
#define RTEMS_LIBI2C_DRIVER_TABLE_ENTRY   \
{                                         \
  initialization_entry:  rtems_i2c_init,  \
  open_entry:            rtems_i2c_open,  \
  close_entry:           rtems_i2c_close, \
  read_entry:            rtems_i2c_read,  \
  write_entry:           rtems_i2c_write, \
  control_entry:         rtems_i2c_ioctl, \
}

/* Bus Driver API
 *
 * Bus drivers provide access to low-level i2c functions
 * such as 'send start', 'send address', 'get bytes' etc.
 */

/* first field must be a pointer to ops; driver
 * may add its own fields after this.
 * the struct that is registered with the library
 * is not copied; a pointer will we passed
 * to the callback functions (ops).
 */
typedef struct rtems_libi2c_bus_t_
{
  const struct rtems_libi2c_bus_ops_ *ops;
  int size;                     /* size of whole structure */
} rtems_libi2c_bus_t;

/* Access functions a low level driver must provide;
 *
 * All of these, except read_bytes and write_bytes
 * return RTEMS_SUCCESSFUL on success and an error status
 * otherwise. The read and write ops return the number
 * of chars read/written or -(status code) on error.
 */
typedef struct rtems_libi2c_bus_ops_
{
  /* Initialize the bus; might be called again to reset the bus driver */
  rtems_status_code (*init) (rtems_libi2c_bus_t * bushdl);
  /* Send start condition */
  rtems_status_code (*send_start) (rtems_libi2c_bus_t * bushdl);
  /* Send stop  condition */
  rtems_status_code (*send_stop) (rtems_libi2c_bus_t * bushdl);
  /* initiate transfer from (rw!=0) or to a device */
  rtems_status_code (*send_addr) (rtems_libi2c_bus_t * bushdl,
                                  uint32_t addr, int rw);
  /* read a number of bytes */
  int (*read_bytes) (rtems_libi2c_bus_t * bushdl, unsigned char *bytes,
                     int nbytes);
  /* write a number of bytes */
  int (*write_bytes) (rtems_libi2c_bus_t * bushdl, unsigned char *bytes,
                      int nbytes);
  /* ioctl misc functions */
  int (*ioctl) (rtems_libi2c_bus_t * bushdl,
		int   cmd,
		void *buffer
		);
} rtems_libi2c_bus_ops_t;


/*
 * Register a lowlevel driver
 *
 * TODO: better description
 *
 * This  allocates a major number identifying *this* driver
 * (i.e., libi2c) and the minor number encodes a bus# and a i2c address.
 *
 * The name will be registered in the filesystem (parent
 * directories must exist, also IMFS filesystem must exist see
 * CONFIGURE_USE_IMFS_AS_BASE_FILESYSTEM). It may be NULL in which case
 * the library will pick a default.
 *
 * RETURNS: bus # (>=0) or -1 on error (errno set).
 */

extern int rtems_libi2c_register_bus (const char *name, rtems_libi2c_bus_t * bus);

extern rtems_device_major_number rtems_libi2c_major;

#define RTEMS_LIBI2C_MAKE_MINOR(busno, i2caddr)	\
	((((busno)&((1<<3)-1))<<10) | ((i2caddr)&((1<<10)-1)))

/* After the library is initialized, a major number is available.
 * As soon as a low-level bus driver is registered (above routine
 * returns a 'busno'), a device node can be created in the filesystem
 * with a major/minor number pair of
 *
 *    rtems_libi2c_major / RTEMS_LIBI2C_MAKE_MINOR(busno, i2caddr)
 *
 * and a 'raw' hi-level driver is then attached to this device
 * node.
 * This 'raw' driver has very simple semantics:
 *
 *   'open'         sends a start condition
 *   'read'/'write' address the device identified by the i2c bus# and address
 *                  encoded in the minor number and read or write, respectively
 *                  a stream of bytes from or to the device. Every time the
 *                  direction is changed, a 're-start' condition followed by
 *                  an 'address' cycle is generated on the i2c bus.
 *   'close'        sends a stop condition.
 *
 * Hence, using the 'raw' driver, e.g., 100 bytes at offset 0x200 can be
 * read from an EEPROM by the following pseudo-code:
 *
 *   mknod("/dev/i2c-54", mode, MKDEV(rtems_libi2c_major, RTEMS_LIBI2C_MAKE_MINOR(0,0x54)))
 *
 *   int fd;
 *   char off[2]={0x02,0x00};
 *
 *   fd = open("/dev/i2c-54",O_RDWR);
 *   write(fd,off,2);
 *   read(fd,buf,100);
 *   close(fd);
 *
 */

/* Higher Level Driver API
 *
 * Higher level drivers know how to deal with specific i2c
 * devices (independent of the bus interface chip) and provide
 * an abstraction, i.e., the usual read/write/ioctl access.
 *
 * Using the above example, such a high level driver could
 * prevent the user from issuing potentially destructive write
 * operations (the aforementioned EEPROM interprets any 3rd
 * and following byte written to the device as data, i.e., the
 * contents could easily be changed!).
 * The correct 'read-pointer offset' programming could be
 * implemented in 'open' and 'ioctl' of a high-level driver and
 * the user would then only have to perform harmless read
 * operations, e.g.,
 *
 *    fd = open("/dev/i2c.eeprom",O_RDONLY) / * opens and sets EEPROM read pointer * /
 *    ioctl(fd, IOCTL_SEEK, 0x200)			/ * repositions the read pointer       * /
 *    read(fd, buf, 100)
 *    close(fd)
 *
 */

/* struct provided at driver registration. The driver may store
 * private data behind the mandatory first fields but the size
 * must be set to the size of the entire struct, e.g.,
 *
 * struct driver_pvt {
 * 	rtems_libi2c_drv_t pub;
 * 	struct {  ...    } pvt;
 * } my_driver = {
 * 		{  ops: my_ops,
 * 		  size: sizeof(my_driver)
 * 		},
 * 		{ ...};
 * };
 *
 * A pointer to this struct is passed to the callback ops.
 */

typedef struct rtems_libi2c_drv_t_
{
  const rtems_driver_address_table *ops;      /* the driver ops */
  int size;                     /* size of whole structure (including appended private data) */
} rtems_libi2c_drv_t;

/*
 * The high level driver must be registered with a particular
 * bus number and i2c address.
 *
 * The registration procedure also creates a filesystem node,
 * i.e., the returned minor number is not really needed.
 *
 * If the 'name' argument is NULL, no filesystem node is
 * created (but this can be done 'manually' using rtems_libi2c_major
 * and the return value of this routine).
 *
 * RETURNS minor number (FYI) or -1 on failure
 */
extern int
rtems_libi2c_register_drv (const char *name, rtems_libi2c_drv_t * drvtbl,
                           unsigned bus, unsigned i2caddr);

/* Operations available to high level drivers */

/* NOTES: The bus a device is attached to is LOCKED from the first send_start
 *        until send_stop is executed!
 *
 *        Bus tenure MUST NOT span multiple system calls - otherwise, a single
 *        thread could get into the protected sections (or would deadlock if the
 *		  mutex was not nestable).
 *		  E.g., consider what happens if 'open' sends a 'start' and 'close'
 *		  sends a 'stop' (i.e., the bus mutex would be locked in 'open' and
 *        released in 'close'. A single thread could try to open two devices
 *        on the same bus and would either deadlock or nest into the bus mutex
 *        and potentially mess up the i2c messages.
 *
 *        The correct way is to *always* relinquish the i2c bus (i.e., send 'stop'
 *		  from any driver routine prior to returning control to the caller.
 *		  Consult the implementation of the generic driver routines (open, close, ...)
 *		  below or the examples in i2c-2b-eeprom.c and i2c-2b-ds1621.c
 *
 * Drivers just pass the minor number on to these routines...
 */
extern rtems_status_code rtems_libi2c_send_start (rtems_device_minor_number minor);

extern rtems_status_code rtems_libi2c_send_stop (rtems_device_minor_number minor);

extern rtems_status_code
rtems_libi2c_send_addr (rtems_device_minor_number minor, int rw);

/* the read/write routines return the number of bytes transferred
 * or -(status_code) on error.
 */
extern int
rtems_libi2c_read_bytes (rtems_device_minor_number minor,
                         unsigned char *bytes, int nbytes);

extern int
rtems_libi2c_write_bytes (rtems_device_minor_number minor,
                          const unsigned char *bytes, int nbytes);

/* Send start, send address and read bytes */
extern int
rtems_libi2c_start_read_bytes (rtems_device_minor_number minor,
			       unsigned char *bytes,
                               int nbytes);

/* Send start, send address and write bytes */
extern int
rtems_libi2c_start_write_bytes (rtems_device_minor_number minor,
				const unsigned char *bytes,
                                int nbytes);


/* call misc iocontrol function */
extern int
rtems_libi2c_ioctl (rtems_device_minor_number minor,
		    int cmd,
		    ...);
/*
 * NOTE: any low-level driver ioctl returning a negative
 * result for release the bus (perform a STOP condition)
 */
/*******************************
 * defined IOCTLs:
 *******************************/
#define RTEMS_LIBI2C_IOCTL_READ_WRITE  1
/*
 * retval = rtems_libi2c_ioctl(rtems_device_minor_number minor,
 *                             RTEMS_LIBI2C_IOCTL_READ_WRITE,
 *                              rtems_libi2c_read_write_t *arg);
 *
 * This call performs a simultanous read/write transfer,
 * which is possible (and sometimes needed) for SPI devices
 *
 *   arg is a pointer to a rd_wr info data structure
 *
 * This call is only needed for SPI devices
 */
#define RTEMS_LIBI2C_IOCTL_START_TFM_READ_WRITE  2
/*
 * retval = rtems_libi2c_ioctl(rtems_device_minor_number minor,
 *                             RTEMS_LIBI2C_IOCTL_START_READ_WRITE,
 *                             unsigned char *rd_buffer,
 *                             const unsigned char *wr_buffer,
 *                             int byte_cnt,
 *                             const rtems_libi2c_tfr_mode_t *tfr_mode_ptr);
 *
 * This call addresses a slave and then:
 * - sets the proper transfer mode,
 *  - performs a simultanous  read/write transfer,
 *    (which is possible and sometimes needed for SPI devices)
 *    NOTE: - if rd_buffer is NULL, receive data will be dropped
 *          - if wr_buffer is NULL, bytes with content 0 will transmitted
 *
 *   rd_buffer is a pointer to a receive buffer (or NULL)
 *   wr_buffer is a pointer to the data to be sent (or NULL)
 *
 * This call is only needed for SPI devices
 */

#define RTEMS_LIBI2C_IOCTL_SET_TFRMODE 3
/*
 * retval = rtems_libi2c_ioctl(rtems_device_minor_number minor,
 *                             RTEMS_LIBI2C_IOCTL_SET_TFRMODE,
 *                             const rtems_libi2c_tfr_mode_t *tfr_mode_ptr);
 *
 * This call sets an SPI device to the transfer mode needed (baudrate etc.)
 *
 *   tfr_mode is a pointer to a structure defining the SPI transfer mode needed
 *   (see below).
 *
 * This call is only needed for SPI devices
 */

#define RTEMS_LIBI2C_IOCTL_GET_DRV_T 4

/*
 * retval = rtems_libi2c_ioctl(rtems_device_minor_number minor,
 *                             RTEMS_LIBI2C_IOCTL_GET_DRV_T,
 *                             const rtems_libi2c_drv_t *drv_t_ptr);
 *
 * This call allows the a high-level driver to query its driver table entry,
 * including its private data appended to it during creation of the entry
 *
 */

/**
 * @brief IO control command for asynchronous read and write.
 *
 * @see rtems_libi2c_read_write_done_t and rtems_libi2c_read_write_async_t.
 *
 * @warning This is work in progress!
 */
#define RTEMS_LIBI2C_IOCTL_READ_WRITE_ASYNC 5

/*
 * argument data structures for IOCTLs defined above
 */
typedef struct {
  unsigned char       *rd_buf;
  const unsigned char *wr_buf;
  int                  byte_cnt;
} rtems_libi2c_read_write_t;

typedef struct {
  uint32_t baudrate;       /* maximum bits per second               */
                           /* only valid for SPI drivers:           */
  uint8_t  bits_per_char;  /* how many bits per byte/word/longword? */
  bool     lsb_first;      /* true: send LSB first                  */
  bool     clock_inv;      /* true: inverted clock (high active)    */
  bool     clock_phs;      /* true: clock starts toggling at start of data tfr */
  uint32_t  idle_char;     /* This character will be continuously transmitted in read only functions */
} rtems_libi2c_tfr_mode_t;

typedef struct {
  rtems_libi2c_tfr_mode_t   tfr_mode;
  rtems_libi2c_read_write_t rd_wr;
} rtems_libi2c_tfm_read_write_t;

/**
 * @brief Notification function type for asynchronous read and write.
 *
 * @see RTEMS_LIBI2C_IOCTL_READ_WRITE_ASYNC and
 * rtems_libi2c_read_write_async_t.
 *
 * @warning This is work in progress!
 */
typedef void (*rtems_libi2c_read_write_done_t) \
  ( int /* return value */, int /* nbytes */, void * /* arg */);

/**
 * @brief IO command data for asynchronous read and write.
 *
 * @see RTEMS_LIBI2C_IOCTL_READ_WRITE_ASYNC and
 * rtems_libi2c_read_write_done_t.
 *
 * @warning This is work in progress!
 */
typedef struct {
  unsigned char                 *rd_buf;
  const unsigned char           *wr_buf;
  int                            byte_cnt;
  rtems_libi2c_read_write_done_t done;
  void                          *arg;
} rtems_libi2c_read_write_async_t;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
