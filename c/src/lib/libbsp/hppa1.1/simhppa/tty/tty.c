/*
 *  Tty IO Driver
 *  This is a "libio" driver based on libc/support/generic/libio interface
 *  which is on top of the RTEMS IO manager.
 *
 *  These provide UNIX-like read and write calls for the C library.
 *
 *  COPYRIGHT (c) 1994 by Division Incorporated
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>

#include <errno.h>

#define PRINT_BUFFER_SIZE    (16 * 1024)

/*
 * NOTE: this structure is dumplicated in print_dump.c utility
 */

struct {
    int  index;
    int  size;
    char buffer[PRINT_BUFFER_SIZE];
} print_buffer;

/* always use printf buffer if non-zero */
int  use_print_buffer;

static int host_read_syscall(int fd, char *buffer, int count);
static int host_write_syscall(int fd, char *buffer, int count);

rtems_device_driver
console_initialize(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
  )
{
    rtems_status_code status;
    
    status = rtems_io_register_name("/dev/tty00",
                                    major,
                                    (rtems_device_minor_number) 0);
    if (status != RTEMS_SUCCESSFUL)
        rtems_fatal_error_occurred(status);

    return RTEMS_SUCCESSFUL;
}

rtems_device_driver
console_open(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
  )
{
    return RTEMS_SUCCESSFUL;
}

rtems_device_driver
console_close(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
  )
{
    return RTEMS_SUCCESSFUL;
}

rtems_device_driver
console_control(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
  )
{
    return RTEMS_SUCCESSFUL;
}


rtems_device_driver
console_read(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
  )
{
    rtems_libio_rw_args_t *rw_args;
    int count = 0;

    rw_args = (rtems_libio_rw_args_t *) arg;

    /*
     * If we are printing to a buffer, then just return newline on all
     * read's.  If we return 0 bytes read, then the pause() calls in
     * the RTEMS tests get hosed (pause() does a gets())
     */

    if ( use_print_buffer )
    {
        *rw_args->buffer = '\n';
        count = 1;
    }
    else
    {
        count = host_read_syscall(0, rw_args->buffer, rw_args->count);
    }

    if (count >= 0)
    {
        rw_args->bytes_moved = count;
        return RTEMS_SUCCESSFUL;
    }
    return RTEMS_UNSATISFIED;
}

rtems_device_driver
console_write(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void                    * arg
  )
{
    unsigned32 level;
    rtems_libio_rw_args_t *rw_args;
    int count = 0;
    int fd = 1;                 /* XXX fixme; needs to be saved in iop */

    rw_args = (rtems_libio_rw_args_t *) arg;

    /*
     * HACK alert
     *
     * Some of the simulators have real problems when multi cpu and
     * using the system calls.  Until this is fixed, if we are multi
     * cpu then we write to a printf buffer
     */

    if ( use_print_buffer )
    {
        /* save size in memory for dumper */
        if (print_buffer.size == 0)
            print_buffer.size = PRINT_BUFFER_SIZE;

        while (rw_args->count-- > 0)
        {
            rtems_interrupt_disable(level);
            print_buffer.buffer[print_buffer.index] = *rw_args->buffer++;
            print_buffer.index++;
            print_buffer.index &= (PRINT_BUFFER_SIZE - 1);
            print_buffer.buffer[print_buffer.index] = 0;
            rtems_interrupt_enable(level);
            count++;
        }
    }
    else
    {
#if 1
        /*
         * if on a multi cpu system and writing to stdout, redirect to stderr
         * so we can keep them separate
         */

        if ((cpu_number == 1) && (fd == 1))
            fd = 2;
#endif
        count = host_write_syscall(fd, rw_args->buffer, rw_args->count);
    }

    if (count >= 0)
    {
        rw_args->bytes_moved = count;
        return RTEMS_SUCCESSFUL;
    }
    return RTEMS_UNSATISFIED;
}


/*
 * Host system call hack.
 * This little trick gets all the args in the right registers
 * for the system call and permits simpler inline asm.
 * Since this whole thing (syscalls under simulator) is a hack,
 * this little bit more is not going to hurt anything.
 */


static int
host_read_syscall(
    int fd,
    char *buffer,
    int count
  )
{                   
    unsigned32 level;
    int rc;

    rtems_interrupt_disable(level);

    /* This is an HPUX system call, with return value copied out */
    asm volatile (" stw     %%r19,-28(0,%%r30)\n\
                    ldil    L%%0xc0000000,%%r1\n\
                    ble     4(7,%%r1)\n\
                    ldi     3,%%r22\n\
                    ldw     -28(0,%%r30),%%r19\n\
                    copy    %%r28, %0"
                  : "=r" (rc)
                  : );

    rtems_interrupt_enable(level);
    return rc;
}

static int
host_write_syscall(
    int fd,
    char *buffer,
    int count
  )
{                   
    unsigned32 level;
    int rc;

    rtems_interrupt_disable(level);

    /* This is an HPUX system call, with return value copied out */
    asm volatile (" stw     %%r19,-28(0,%%r30)\n\
                    ldil    L%%0xc0000000,%%r1\n\
                    ble     4(7,%%r1)\n\
                    ldi     4,%%r22\n\
                    ldw     -28(0,%%r30),%%r19\n\
                    copy    %%r28, %0"
                  : "=r" (rc)
                  : );

    rtems_interrupt_enable(level);
    return rc;
}

