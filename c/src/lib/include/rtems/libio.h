/*
 * General purpose communication channel for RTEMS to allow UNIX/POSIX
 *    system call behavior on top of RTEMS IO devices.
 *
 * TODO
 *      stat(2)
 *      unlink(2)
 *      rename(2)
 *
 *  $Id$
 */

#ifndef _RTEMS_LIBIO_H
#define _RTEMS_LIBIO_H

#include <sys/stat.h>

typedef unsigned32 rtems_libio_offset_t;

/*
 * An open file data structure, indexed by 'fd'
 * TODO:
 *    should really have a separate per/file data structure that this
 *    points to (eg: size, offset, driver, pathname should be in that)
 */

typedef struct {
    rtems_driver_name_t *driver;
    rtems_libio_offset_t size;          /* size of file */
    rtems_libio_offset_t offset;        /* current offset into the file */
    unsigned32           flags;
    char                *pathname;      /* opened pathname */
    Objects_Id           sem;
    unsigned32           data0;         /* private to "driver" */
    void                *data1;         /* ... */
} rtems_libio_t;


/*
 * param block for read/write
 * Note: it must include 'offset' instead of using iop's offset since
 *       we can have multiple outstanding i/o's on a device.
 */

typedef struct {
    rtems_libio_t          *iop;
    rtems_libio_offset_t    offset;
    unsigned8              *buffer;
    unsigned32              count;
    unsigned32              flags;
    unsigned32              bytes_moved;
} rtems_libio_rw_args_t;

/*
 * param block for open/close
 */

typedef struct {
    rtems_libio_t          *iop;
    unsigned32              flags;
    unsigned32              mode;
} rtems_libio_open_close_args_t;

/*
 * param block for ioctl
 */

typedef struct {
    rtems_libio_t          *iop;
    unsigned32              command;
    void                   *buffer;
    unsigned32              ioctl_return;
} rtems_libio_ioctl_args_t;


/*
 * Values for 'flag'
 */

#define LIBIO_FLAGS_NO_DELAY      0x0001  /* return immediately if no data */
#define LIBIO_FLAGS_READ          0x0002  /* reading */
#define LIBIO_FLAGS_WRITE         0x0004  /* writing */
#define LIBIO_FLAGS_LINE_BUFFERED 0x0008  /* line buffered io (^h, ^u, etc) */
#define LIBIO_FLAGS_OPEN          0x0100  /* device is open */
#define LIBIO_FLAGS_APPEND        0x0200  /* all writes append */
#define LIBIO_FLAGS_CREATE        0x0400  /* create file */

#define LIBIO_FLAGS_READ_WRITE    (LIBIO_FLAGS_READ | LIBIO_FLAGS_WRITE)

void rtems_libio_init(void);

int __rtems_open(const char  *pathname, unsigned32 flag, unsigned32 mode);
int __rtems_close(int  fd);
int __rtems_read(int fd, void *buffer, unsigned32 count);
int __rtems_write(int fd, const void *buffer, unsigned32 count);
int __rtems_ioctl(int fd, unsigned32  command, void *buffer);
int __rtems_lseek(int fd, rtems_libio_offset_t offset, int whence);
int __rtems_fstat(int _fd, struct stat* _sbuf);

/*
 * External I/O handlers
 */
typedef struct {
    int (*open)(const char  *pathname, unsigned32 flag, unsigned32 mode);
    int (*close)(int  fd);
    int (*read)(int fd, void *buffer, unsigned32 count);
    int (*write)(int fd, const void *buffer, unsigned32 count);
    int (*ioctl)(int fd, unsigned32  command, void *buffer);
    int (*lseek)(int fd, rtems_libio_offset_t offset, int whence);
} rtems_libio_handler_t;

void rtems_register_libio_handler(int handler_flag,
                                 const rtems_libio_handler_t *handler);

#define RTEMS_FILE_DESCRIPTOR_TYPE_FILE         0x0000
#define RTEMS_FILE_DESCRIPTOR_TYPE_SOCKET       0x1000
#define rtems_make_file_descriptor(fd,flags)    ((fd)|(flags))
#define rtems_file_descriptor_base(fd)          ((fd) & 0x0FFF)
#define rtems_file_descriptor_type(fd)          ((fd) & 0xF000)
#define rtems_file_descriptor_type_index(fd)    ((((fd) & 0xF000) >> 12) - 1)

/*
 *  IOCTL values
 */

#define       RTEMS_IO_GET_ATTRIBUTES 1
#define       RTEMS_IO_SET_ATTRIBUTES 2
#define       RTEMS_IO_TCDRAIN        3

/*
 * Callbacks from TERMIOS routines to device-dependent code
 */
#include <termios.h>
typedef struct rtems_termios_callbacks {
  int       (*firstOpen)(int major, int minor, void *arg);
  int       (*lastClose)(int major, int minor, void *arg);
  int       (*pollRead)(int minor);
  int       (*write)(int minor, const char *buf, int len);
  int       (*setAttributes)(int minor, const struct termios *t);
  int       (*stopRemoteTx)(int minor);
  int       (*startRemoteTx)(int minor);
  int       outputUsesInterrupts;
} rtems_termios_callbacks;

/*
 * Device-independent TERMIOS routines
 */
void rtems_termios_initialize (void);
rtems_status_code rtems_termios_open (
  rtems_device_major_number      major,
  rtems_device_minor_number      minor,
  void                          *arg,
  const rtems_termios_callbacks *callbacks
  );
rtems_status_code rtems_termios_close (void *arg);
rtems_status_code rtems_termios_read (void *arg);
rtems_status_code rtems_termios_write (void *arg);
rtems_status_code rtems_termios_ioctl (void *arg);
void rtems_termios_enqueue_raw_characters (void *ttyp, char *buf, int len);
void rtems_termios_dequeue_characters (void *ttyp, int len);
void rtems_termios_reserve_resources(
  rtems_configuration_table *configuration,
  rtems_unsigned32           number_of_devices
);

#endif /* _RTEMS_LIBIO_H */
