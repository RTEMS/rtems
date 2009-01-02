/**
 * @file aio.h
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _AIO_H
#define _AIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>

#if defined(_POSIX_ASYNCHRONOUS_IO)

/*
 *  6.7.1 Data Definitions for Asynchronous Input and Output,
 *        P1003.1b-1993, p. 151
 */

#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>

/*
 *  6.7.1.2 Manifest Constants, P1003.1b-1993, p. 153
 */

#define AIO_CANCELED    0 /* all requested operations have been canceled */
#define AIO_NOTCANCELED 1 /* some of the operations could not be canceled */
                          /*   since they are in progress */
#define AIO_ALLDONE     2 /* none of the requested operations could be */
                          /*   canceled since they are already complete */

/* lio_listio() options */

#define LIO_WAIT        0 /* calling process is to suspend until the */
                          /*   operation is complete */
#define LIO_NOWAIT      1 /* calling process is to continue execution while */
                          /*   the operation is performed and no notification */
                          /*   shall be given when the operation is completed */
#define LIO_READ        2 /* request a read() */
#define LIO_WRITE       3 /* request a write() */
#define LIO_NOP         4 /* no transfer is requested */

/*
 *  6.7.1.1 Asynchronous I/O Control Block, P1003.1b-1993, p. 151
 */

struct aiocb {
  int             aio_fildes;     /* File descriptor */
  off_t           aio_offset;     /* File offset */
  volatile void  *aio_buf;        /* Location of buffer */
  size_t          aio_nbytes;     /* Length of transfer */
  int             aio_reqprio;    /* Request priority offset */
  struct sigevent aio_sigevent;   /* Signal number and value */
  int             aio_lio_opcode; /* Operation to be performed */
};

/*
 *  6.7.2 Asynchronous Read, P1003.1b-1993, p. 154
 */

int aio_read(
  struct aiocb  *aiocbp
);

/*
 *  6.7.3 Asynchronous Write, P1003.1b-1993, p. 155
 */

int aio_write(
  struct aiocb  *aiocbp
);

/*
 *  6.7.4 List Directed I/O, P1003.1b-1993, p. 158
 */

int lio_listio(
  int                    mode,
  struct aiocb  * const  list[],
  int                    nent,
  struct sigevent       *sig
);

/*
 *  6.7.5 Retrieve Error of Asynchronous I/O Operation, P1003.1b-1993, p. 161
 */

int aio_error(
  const struct aiocb  *aiocbp
);

/*
 *  6.7.6 Retrieve Return Status of Asynchronous I/O Operation,
 *        P1003.1b-1993, p. 162
 */

int aio_return(
  const struct aiocb  *aiocbp
);

/*
 *  6.7.7 Cancel Asynchronous I/O Operation, P1003.1b-1993, p. 163
 */

int aio_cancel(
  int            filedes,
  struct aiocb  *aiocbp
);

/*
 *  6.7.7 Wait for Asynchronous I/O Request, P1003.1b-1993, p. 164
 */

int aio_suspend(
  const struct aiocb  * const   list[],
  int                     nent,
  const struct timespec  *timeout
);

#if defined(_POSIX_SYNCHRONIZED_IO)

/*
 *  6.7.9 Asynchronous File Synchronization, P1003.1b-1993, p. 166
 */

int aio_fsync(
  int            op,
  struct aiocb  *aiocbp
);

#endif /* _POSIX_SYNCHRONIZED_IO */

#endif /* _POSIX_ASYNCHRONOUS_IO */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
