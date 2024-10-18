/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIX_AIO
 * 
 * @brief POSIX Asynchronous I/O Support
 * 
 * This file contains the definitions related to POSIX I/O.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _AIO_H
#define _AIO_H

#include <sys/cdefs.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup POSIX_AIO POSIX Asynchronous I/O Support
 * 
 * @ingroup POSIXAPI
 * @{
 */

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

/** All requested operations have been canceled */
#define AIO_CANCELED    0

/** Some operations could not be canceled since they are in progress */
#define AIO_NOTCANCELED 1

/**
 * None of the requested operations could be canceled since
 * they are already complete
 */
#define AIO_ALLDONE     2

/** Calling process is to be suspendes until the operation is complete */
#define LIO_WAIT        0

/**
 * Calling process is to continue execution while the operation is performed
 * and no notification shall be given when the operation is completed
 */
#define LIO_NOWAIT      1

/** No transfer is requested */
#define LIO_NOP         0

/** Request a read() */
#define LIO_READ        1

/** Request a write() */
#define LIO_WRITE       2

/**
 * @brief Asynchronous I/O Control Block
 * 
 * 6.7.1.1 Asynchronous I/O Control Block, P1003.1b-1993, p. 151
 */
struct aiocb {
  /** @name public */

  /** @brief File descriptor */
  int             aio_fildes;
  /** @brief File offset */
  off_t           aio_offset;
  /** @brief Location of buffer */
  volatile void  *aio_buf;
  /** @brief Length of transfer */
  size_t          aio_nbytes;
  /** @brief Request priority offset */
  int             aio_reqprio;
  /** @brief Signal number and value */
  struct sigevent aio_sigevent;
  /** @brief Operation to be performed */
  int             aio_lio_opcode;

  /** @name private */

  /** @brief Field used for aio_return() */
  int             return_status;
  /** @brief Field used for aio_error() */
  int             error_code;
  /** @brief Filed used for aio_return() */
  ssize_t         return_value;
};

/**
 * @brief Asynchronous Read
 * 
 * 6.7.2 Asynchronous Read, P1003.1b-1993, p. 154
 * 
 * @param[in,out] aiocbp is a pointer to the asynchronous I/O control block
 * 
 * @retval 0 The request has been successfuly enqueued.
 * @retval -1 The request has not been enqueued due to an error.
 *         The error is indicated in errno:
 *         - EBADF FD not opened for read.
 *         - EINVAL invalid aio_reqprio or aio_offset or aio_nbytes.
 *         - EAGAIN not enough memory.
 *         - EAGAIN the addition of a new request to the queue would
 *           violate the RTEMS_AIO_MAX limit.
 *         - EINVAL the starting position of the file is past the maximum offset.
 *           for this file.
 *         - EINVAL aiocbp is a NULL pointer.
 *         - EINVAL aiocbp->sigevent is not valid.
 */
int aio_read(
  struct aiocb  *aiocbp
);

/**
 * @brief Asynchronous Write
 *
 * 6.7.3 Asynchronous Write, P1003.1b-1993, p. 155
 * 
 * @param[in,out] aiocbp is a pointer to the asynchronous I/O control block
 * 
 * @retval 0 The request has been successfuly enqueued.
 * @retval -1 The request has not been enqueued due to an error.
 *         The error is indicated in errno:
 *         - EBADF FD not opened for write.
 *         - EINVAL invalid aio_reqprio or aio_offset or aio_nbytes.
 *         - EAGAIN not enough memory.
 *         - EAGAIN the addition of a new request to the queue would
 *           violate the RTEMS_AIO_MAX limit.
 *         - EINVAL aiocbp is a NULL pointer.
 *         - EINVAL aiocbp->sigevent is not valid.
 */
int aio_write(
  struct aiocb  *aiocbp
);

/**
 * @brief List Directed I/O
 *
 * 6.7.4 List Directed I/O, P1003.1b-1993, p. 158
 * 
 * @param[in] mode can be LIO_WAIT or LIO_NOWAIT
 * @param[in,out] list is a pointer to the array of aiocb
 * @param[in] nent is the number of element in list
 * @param[in] sig if mode is LIO_NOWAIT, specifies how to notify list completion.
 *
 * @retval 0 the call to lio_listio() has completed successfuly.
 * @retval -1 The call did not complete successfully.
 *         The error is indicated in errno:
 *         - ENOSYS the project has been built with RTEMS_POSIX_API not defined.
 *         - EAGAIN the call failed due to resources limitations.
 *         - EAGAIN the number of entries indicated by nent value would cause
 *                  the RTEMS_AIO_MAX limit to be excedeed.
 *         - EINVAL list is a NULL pointer.
 *         - EINVAL mode is not a valid value.
 *         - EINVAL the value of nent is not valid or higher than AIO_LISTIO_MAX.
 *         - EINVAL list is a NULL pointer.
 *         - EINVAL the sigevent struct pointed by sig is not valid.
 *         - EINTR the wait for list completion during a LIO_WAIT operation was
 *                 interrupted by an external event.
 *         - EIO One or more of the individual I/O operations failed.
 */
int lio_listio(
  int              mode,
  struct aiocb    *__restrict const  list[__restrict],
  int              nent,
  struct sigevent *__restrict sig
);

/**
 * @brief Retrieve Error of Asynchronous I/O Operation
 * 
 * 6.7.5 Retrieve Error of Asynchronous I/O Operation, P1003.1b-1993, p. 161
 * 
 * @param[in,out] aiocbp is a pointer to the asynchronous I/O control block
 * 
 * @retval 0 The operation has completed succesfully.
 * @retval EINPROGRESS The operation has not yet completed.
 * @retval EINVAL The return status for the request associated with aiocbp
 *                has already been retrieved.
 * @retval EINVAL aiocbp is a NULL pointer.
 * @return The error status as described for the various operations.
 */
int aio_error(
  const struct aiocb  *aiocbp
);

/**
 * @brief Retrieve Return Status of Asynchronous I/O Operation
 * 
 * 6.7.6 Retrieve Return Status of Asynchronous I/O Operation,
 *       P1003.1b-1993, p. 162
 * 
 * @param[in,out] aiocbp is a pointer to the asynchronous I/O control block
 * 
 * @retval -1 The operation returned with an error. errno is set to indicate
 *            the error,as described for the various operations.
 * @retval EINVAL The return status for the request associated with aiocbp
 *                has already been retrieved.
 * @retval EINVAL aiocbp is a NULL pointer.
 * @return The operation return status, stored in aiocbp->return_value
 */
ssize_t aio_return(
  struct aiocb  *aiocbp
);

/**
 * @brief Cancel asynchronous I/O operation.
 * 
 * 6.7.7 Cancel Asynchronous I/O Operation, P1003.1b-1993, p. 163
 * 
 * @param[in] filedes Is the file descriptor
 * @param[in,out] aiocbp  Is a pointer to the asynchronous I/O control block
 * 
 * @retval AIO_CANCELED     The requested operation(s) were canceled. 
 * @retval AIO_NOTCANCELED  Some of the requested operation(s) cannot be
 *                          canceled since they are in progress.
 * @retval AIO_ALLDONE      None of the requested operation(s) could be
 *                          canceled since they are already complete.
 * @retval -1               An error has occured, errno indicates the error:
 *                          - EBADF fildes is not a valid file descriptor.
 */
int aio_cancel(
  int            filedes,
  struct aiocb  *aiocbp
);

/**
 * @brief Wait for Asynchronous I/O Request
 *
 * 6.7.7 Wait for Asynchronous I/O Request, P1003.1b-1993, p. 164
 * 
 * @param list Is a pointer to the array of aiocbs.
 * @param nent Is the number of aiocbs in the array.
 * @param timeout Specifies the timeout time.
 *
 * @retval 0 One of the operation specified in list completed.
 * @retval EINVAL Invalid nent value.
 * @retval EAGAIN Could not suspend due to resource limitation.
 * @retval ENOMEM Could not suspend due to memory limitation.
 * @retval EAGAIN No asynchronous I/O indicated in the list referenced by list
           completed in the time interval indicated by timeout.
 * @retval EINTR An event interrupted the aio_suspend() function.
 */
int aio_suspend(
  const struct aiocb  * const   list[],
  int                     nent,
  const struct timespec  *timeout
);

#if defined(_POSIX_SYNCHRONIZED_IO)

/**
 * @brief Asynchronous File Synchronization
 * 
 * 6.7.9 Asynchronous File Synchronization, P1003.1b-1993, p. 166
 * 
 * @param[in] op     O_SYNC or O_DSYNC
 * @param[in,out] aiocbp is a pointer to the asynchronous I/O control block
 * 
 * @retval  0 The request was correctly enqueued. 
 * @retval -1 An error occured. errno indicated the error:
 *            - EAGAIN The requested asynchronous operation was not queued
 *              due to temporary resource limitations.
 *            - EAGAIN the addition of a new request to the queue would
 *              violate the RTEMS_AIO_MAX limit.
 *            - EBADF The aio_fildes member of the aiocb structure referenced
 *              by the aiocbp argument is not a valid file descriptor.
 *            - EINVAL A value of op other than O_SYNC or O_DSYNC was specified.
 *            - EINVAL aiocbp is a NULL pointer.
 *            - EINVAL aiocbp->sigevent is not valid.
 */
int aio_fsync(
  int            op,
  struct aiocb  *aiocbp
);

#endif /* _POSIX_SYNCHRONIZED_IO */

#endif /* _POSIX_ASYNCHRONOUS_IO */

/** @} */

#ifdef __cplusplus
}
#endif

#endif

/* end of include file */

