/*
 *  Provide UNIX/POSIX-like io system calls for RTEMS using the
 *  RTEMS IO manager
 *
 *  $Id$
 */

#include <rtems.h>
#include <rtems/assoc.h>                /* assoc.h not included by rtems.h */

#include <stdio.h>                      /* O_RDONLY, et.al. */
#include <fcntl.h>                      /* O_RDONLY, et.al. */
#include <assert.h>

#if ! defined(O_NDELAY)
# if defined(solaris2)
#  define O_NDELAY O_NONBLOCK
# elif defined(RTEMS_NEWLIB)
#  define O_NDELAY _FNBIO
# endif
#endif


#include <errno.h>
#include <string.h>                     /* strcmp */
#include <unistd.h>
#include <stdlib.h>                     /* calloc() */

#include "libio.h"                      /* libio.h not pulled in by rtems */

/*
 * Semaphore to protect the io table
 */

Objects_Id rtems_libio_semaphore;

#define RTEMS_LIBIO_SEM         rtems_build_name('L', 'B', 'I', 'O')
#define RTEMS_LIBIO_IOP_SEM(n)  rtems_build_name('L', 'B', 'I', n)

unsigned32     rtems_libio_number_iops;
rtems_libio_t *rtems_libio_iops;
rtems_libio_t *rtems_libio_last_iop;

#define rtems_libio_iop(fd)    ((((unsigned32)(fd)) < rtems_libio_number_iops) ? \
                                       &rtems_libio_iops[fd] : 0)

#define rtems_libio_check_fd(fd) \
    do { \
        if ((unsigned32) (fd) >= rtems_libio_number_iops) \
        { \
            errno = EBADF; \
            return -1; \
        } \
    } while (0)

#define rtems_libio_check_buffer(buffer) \
    do { \
        if ((buffer) == 0) \
        { \
            errno = EINVAL; \
            return -1; \
        } \
    } while (0)

#define rtems_libio_check_count(count) \
    do { \
        if ((count) == 0) \
        { \
            return 0; \
        } \
    } while (0)

#define rtems_libio_check_permissions(iop, flag) \
    do { \
        if (((iop)->flags & (flag)) == 0) \
        { \
              errno = EINVAL; \
              return -1; \
        } \
    } while (0)

/*
 * External I/O handlers
 * 
 * Space for all possible handlers is preallocated
 * to speed up dispatch to external handlers.
 */

static rtems_libio_handler_t handlers[15];

void
rtems_register_libio_handler(
    int                         handler_flag,
    const rtems_libio_handler_t *handler
)
{
  int handler_index = rtems_file_descriptor_type_index(handler_flag);

  if ((handler_index < 0) || (handler_index >= 15))
    rtems_fatal_error_occurred( RTEMS_INVALID_NUMBER );
  handlers[handler_index] = *handler;
}


void
rtems_libio_config(
    rtems_configuration_table *config,
    unsigned32                 max_fds
)
{
    rtems_libio_number_iops = max_fds;

    /*
     * tweak config to reflect # of semaphores we will need
     */

    /* one for iop table */
    config->RTEMS_api_configuration->maximum_semaphores += 1; 
    config->RTEMS_api_configuration->maximum_semaphores += max_fds;
}

/*
 * Called by bsp startup code to init the libio area.
 */

void
rtems_libio_init(void)
{
    rtems_status_code rc;

    if (rtems_libio_number_iops > 0)
    {
        rtems_libio_iops = (rtems_libio_t *) calloc(rtems_libio_number_iops,
                                                    sizeof(rtems_libio_t));
        if (rtems_libio_iops == NULL)
            rtems_fatal_error_occurred(RTEMS_NO_MEMORY);

        rtems_libio_last_iop = rtems_libio_iops + (rtems_libio_number_iops - 1);
    }

    rc = rtems_semaphore_create(
      RTEMS_LIBIO_SEM,
      1,
      RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY,
      RTEMS_NO_PRIORITY,
      &rtems_libio_semaphore
    );
    if (rc != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred(rc);
}

/*
 * Convert RTEMS status to a UNIX errno
 */

rtems_assoc_t errno_assoc[] = {
    { "OK",                 RTEMS_SUCCESSFUL,                0 },
    { "TIMEOUT",            RTEMS_TIMEOUT,                   ETIME },
    { "NO MEMORY",          RTEMS_NO_MEMORY,                 ENOMEM },
    { "NO DEVICE",          RTEMS_UNSATISFIED,               ENOSYS },
    { "INVALID NUMBER",     RTEMS_INVALID_NUMBER,            EBADF},
    { "NOT RESOURCE OWNER", RTEMS_NOT_OWNER_OF_RESOURCE,     EPERM},
    { "IO ERROR",           RTEMS_IO_ERROR,                  EIO},
    { 0, 0, 0 },
};

static unsigned32
rtems_libio_errno(rtems_status_code code)
{
    int rc;
    
    if ((rc = rtems_assoc_remote_by_local(errno_assoc, (unsigned32) code)))
    {
        errno = rc;
        return -1;
    }
    return -1;
}

/*
 * Convert UNIX fnctl(2) flags to ones that RTEMS drivers understand
 */

rtems_assoc_t access_modes_assoc[] = {
    { "READ",       LIBIO_FLAGS_READ,  O_RDONLY },
    { "WRITE",      LIBIO_FLAGS_WRITE, O_WRONLY },
    { "READ/WRITE", LIBIO_FLAGS_READ_WRITE, O_RDWR },
    { 0, 0, 0 },
};

rtems_assoc_t status_flags_assoc[] = {
    { "NO DELAY",  LIBIO_FLAGS_NO_DELAY,  O_NDELAY },
    { "APPEND",    LIBIO_FLAGS_APPEND,    O_APPEND },
    { "CREATE",    LIBIO_FLAGS_CREATE,    O_CREAT },
    { 0, 0, 0 },
};

static unsigned32
rtems_libio_fcntl_flags(unsigned32 fcntl_flags)
{
    unsigned32 flags = 0;
    unsigned32 access_modes;

    /*
     * Access mode is a small integer
     */
    
    access_modes = fcntl_flags & O_ACCMODE;
    fcntl_flags &= ~O_ACCMODE;
    flags = rtems_assoc_local_by_remote(access_modes_assoc, access_modes);

    /*
     * Everything else is single bits
     */

    flags |= rtems_assoc_local_by_remote_bitfield(status_flags_assoc, fcntl_flags);
    return flags;
}


static rtems_libio_t *
rtems_libio_allocate(void)
{
    rtems_libio_t *iop;
    rtems_status_code rc;
    
    rtems_semaphore_obtain(rtems_libio_semaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT);

    for (iop = rtems_libio_iops; iop <= rtems_libio_last_iop; iop++)
        if ((iop->flags & LIBIO_FLAGS_OPEN) == 0)
        {
            /*
             * Got one; create a semaphore for it
             */

            rc = rtems_semaphore_create(
              RTEMS_LIBIO_IOP_SEM(iop - rtems_libio_iops),
              1,
              RTEMS_BINARY_SEMAPHORE | RTEMS_INHERIT_PRIORITY | RTEMS_PRIORITY,
              RTEMS_NO_PRIORITY,
              &iop->sem
            );
            if (rc != RTEMS_SUCCESSFUL)
                goto failed;
            
            iop->flags = LIBIO_FLAGS_OPEN;
            goto done;
        }
    
failed:
    iop = 0;
    
done:
    rtems_semaphore_release(rtems_libio_semaphore);
    return iop;
}

static void
rtems_libio_free(rtems_libio_t *iop)
{
    rtems_semaphore_obtain(rtems_libio_semaphore, RTEMS_WAIT, RTEMS_NO_TIMEOUT);

    if (iop->sem)
        rtems_semaphore_delete(iop->sem);
    (void) memset(iop, 0, sizeof(*iop));

    rtems_semaphore_release(rtems_libio_semaphore);
}

int
__rtems_open(
    const char   *pathname,
    unsigned32    flag,
    unsigned32    mode)
{
    rtems_status_code rc;
    rtems_libio_t *iop = 0;
    rtems_driver_name_t *np;
    rtems_libio_open_close_args_t args;

    /*
     * Additional external I/O handlers would be supported by
     * adding code to pick apart the pathname appropriately.
     * The networking code does not require changes here since
     * network file descriptors are obtained using socket(), not
     * open().
     */

    if ((rc = rtems_io_lookup_name(pathname, &np)) != RTEMS_SUCCESSFUL)
        goto done;

    iop = rtems_libio_allocate();
    if (iop == 0)
    {
        rc = RTEMS_TOO_MANY;
        goto done;
    }
    
    iop->driver = np;
    iop->pathname = (char *) pathname;
    iop->flags |= rtems_libio_fcntl_flags(flag);

    args.iop = iop;
    args.flags = iop->flags;
    args.mode = mode;

    rc = rtems_io_open(np->major, np->minor, (void *) &args);
    
done:
  
    if (rc != RTEMS_SUCCESSFUL)
    {
        if (iop)
            rtems_libio_free(iop);
        return rtems_libio_errno(rc);
    }
    
    return iop - rtems_libio_iops;
}
    
int
__rtems_close(
    int  fd
  )    
{
    rtems_status_code rc;
    rtems_driver_name_t *np;
    rtems_libio_t *iop;
    rtems_libio_open_close_args_t args;
    int status;

    if (rtems_file_descriptor_type(fd)) {
        int (*fp)(int fd);

        fp = handlers[rtems_file_descriptor_type_index(fd)].close;
        if (fp == NULL) {
            errno = EBADF;
            return -1;
        }
        status = (*fp)(fd);
        return status;
    }
    iop = rtems_libio_iop(fd);
    rtems_libio_check_fd(fd);

    np = iop->driver;

    args.iop = iop;
    args.flags = 0;
    args.mode = 0;
    
    rc = rtems_io_close(np->major, np->minor, (void *) &args);

    rtems_libio_free(iop);

    if (rc != RTEMS_SUCCESSFUL)
        return rtems_libio_errno(rc);
    return 0;
}
    
int
__rtems_read(
    int       fd,
    void *    buffer,
    unsigned32 count
  )
{
    rtems_status_code rc;
    rtems_driver_name_t *np;
    rtems_libio_t *iop;
    rtems_libio_rw_args_t args;

    if (rtems_file_descriptor_type(fd)) {
        int (*fp)(int fd, void *buffer, unsigned32 count);

        fp = handlers[rtems_file_descriptor_type_index(fd)].read;
        if (fp == NULL) {
            errno = EBADF;
            return -1;
        }
        return (*fp)(fd, buffer, count);
    }
    iop = rtems_libio_iop(fd);
    rtems_libio_check_fd(fd);
    rtems_libio_check_buffer(buffer);
    rtems_libio_check_count(count);
    rtems_libio_check_permissions(iop, LIBIO_FLAGS_READ);

    np = iop->driver;

    args.iop = iop;
    args.offset = iop->offset;
    args.buffer = buffer;
    args.count = count;
    args.flags = iop->flags;
    args.bytes_moved = 0;

    rc = rtems_io_read(np->major, np->minor, (void *) &args);

    iop->offset += args.bytes_moved;

    if (rc != RTEMS_SUCCESSFUL)
        return rtems_libio_errno(rc);

    return args.bytes_moved;
}

int
__rtems_write(
    int         fd,
    const void *buffer,
    unsigned32  count
  )
{
    rtems_status_code rc;
    rtems_driver_name_t *np;
    rtems_libio_t *iop;
    rtems_libio_rw_args_t args;

    if (rtems_file_descriptor_type(fd)) {
        int (*fp)(int fd, const void *buffer, unsigned32 count);

        fp = handlers[rtems_file_descriptor_type_index(fd)].write;
        if (fp == NULL) {
            errno = EBADF;
            return -1;
        }
        return (*fp)(fd, buffer, count);
    }
    iop = rtems_libio_iop(fd);
    rtems_libio_check_fd(fd);
    rtems_libio_check_buffer(buffer);
    rtems_libio_check_count(count);
    rtems_libio_check_permissions(iop, LIBIO_FLAGS_WRITE);

    np = iop->driver;

    args.iop = iop;
    args.offset = iop->offset;
    args.buffer = (void *) buffer;
    args.count = count;
    args.flags = iop->flags;
    args.bytes_moved = 0;

    rc = rtems_io_write(np->major, np->minor, (void *) &args);

    iop->offset += args.bytes_moved;

    if (rc != RTEMS_SUCCESSFUL)
        return rtems_libio_errno(rc);

    return args.bytes_moved;
}

int
__rtems_ioctl(
    int         fd,
    unsigned32  command,
    void *      buffer)
{
    rtems_status_code rc;
    rtems_driver_name_t *np;
    rtems_libio_t *iop;
    rtems_libio_ioctl_args_t args;

    if (rtems_file_descriptor_type(fd)) {
        int (*fp)(int fd, unsigned32 command, void *buffer);

        fp = handlers[rtems_file_descriptor_type_index(fd)].ioctl;
        if (fp == NULL) {
            errno = EBADF;
            return -1;
        }
        return (*fp)(fd, command, buffer);
    }
    iop = rtems_libio_iop(fd);
    rtems_libio_check_fd(fd);

    np = iop->driver;

    args.iop = iop;
    args.command = command;
    args.buffer = buffer;

    rc = rtems_io_control(np->major, np->minor, (void *) &args);

    if (rc != RTEMS_SUCCESSFUL)
        return rtems_libio_errno(rc);

    return args.ioctl_return;
}
    
/*
 * internal only??
 */


int
__rtems_lseek(
    int                  fd,
    rtems_libio_offset_t offset,
    int                  whence
  )    
{
    rtems_libio_t *iop;

    if (rtems_file_descriptor_type(fd)) {
        int (*fp)(int fd, rtems_libio_offset_t offset, int whence);

        fp = handlers[rtems_file_descriptor_type_index(fd)].lseek;
        if (fp == NULL) {
            errno = EBADF;
            return -1;
        }
        return (*fp)(fd, offset, whence);
    }
    iop = rtems_libio_iop(fd);
    rtems_libio_check_fd(fd);

    switch (whence)
    {
        case SEEK_SET:
            iop->offset = offset;
            break;

        case SEEK_CUR:
            iop->offset += offset;
            break;

        case SEEK_END:
            iop->offset = iop->size - offset;
            break;

        default:
            errno = EINVAL;
            return -1;
    }
    return 0;
}
