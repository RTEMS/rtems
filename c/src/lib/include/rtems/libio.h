/*
 *	@(#)libio.h	1.1 - 95/06/02
 *	
 *
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
    unsigned32           data1;         /* ... */
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

void rtems_libio_config(rtems_configuration_table *config, unsigned32 max_fds);
void rtems_libio_init(void);

int __open(const char  *pathname, unsigned32 flag, unsigned32 mode);
int __close(int  fd);
int __read(int fd, void *buffer, unsigned32 count);
int __write(int fd, const void *buffer, unsigned32 count);
int __ioctl(int fd, unsigned32  command, void *buffer);
int __lseek(int fd, rtems_libio_offset_t offset, int whence);

#endif /* _RTEMS_LIBIO_H */
